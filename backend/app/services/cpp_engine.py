import asyncio
import json
import logging
from pathlib import Path
from typing import Any, Dict, List, Optional
from backend.app.config import get_settings

logger = logging.getLogger(__name__)


class EngineError(Exception):
    """Base exception for C++ engine integration failures."""
    def __init__(self, code: str, message: str, details: Optional[Any] = None):
        super().__init__(message)
        self.code = code
        self.message = message
        self.details = details


class EngineNotFoundError(EngineError):
    """Raised when the C++ CLI binary does not exist at the configured path."""
    def __init__(self, path: str):
        super().__init__(
            code="ENGINE_NOT_FOUND",
            message=f"GreedyBreaker C++ CLI binary not found at: {path}. Ensure the engine is compiled.",
            details={"path": path}
        )


class EngineTimeoutError(EngineError):
    """Raised when the C++ engine process exceeds the configured timeout."""
    def __init__(self, timeout_seconds: float):
        super().__init__(
            code="ENGINE_TIMEOUT",
            message=f"C++ investigation engine timed out after {timeout_seconds:.1f}s.",
            details={"timeout_seconds": timeout_seconds}
        )


class EngineExecutionError(EngineError):
    """Raised when the C++ engine reports an execution error or exits non-zero."""
    def __init__(self, code: str, message: str, exit_code: Optional[int] = None, stderr: Optional[str] = None):
        super().__init__(
            code=code,
            message=message,
            details={"exit_code": exit_code, "stderr": stderr}
        )


class EngineMalformedJsonError(EngineError):
    """Raised when stdout from the C++ engine cannot be parsed as valid JSON."""
    def __init__(self, raw_output: str, parse_error: str):
        super().__init__(
            code="ENGINE_MALFORMED_JSON",
            message=f"Failed to parse C++ engine JSON output: {parse_error}",
            details={"raw_output_snippet": raw_output[:500]}
        )


class CppEngineService:
    """
    Authoritative service communicating with the C++20 greedybreaker_cli via
    standard input / standard output JSON IPC.
    """

    def __init__(self, cli_path: Optional[str] = None, timeout: Optional[float] = None):
        settings = get_settings()
        self.cli_path = cli_path or settings.greedybreaker_cli_path
        self.timeout = timeout if timeout is not None else settings.subprocess_timeout_seconds

    def check_binary_exists(self) -> bool:
        p = Path(self.cli_path)
        return p.exists() and p.is_file()

    async def _execute_subprocess(
        self,
        args: List[str],
        input_data: Optional[str] = None,
        custom_timeout: Optional[float] = None
    ) -> Dict[str, Any]:
        """
        Executes greedybreaker_cli safely using asyncio subprocess without shell interpolation.
        Sends input_data through stdin and captures stdout as structured JSON.
        """
        if not self.check_binary_exists():
            raise EngineNotFoundError(self.cli_path)

        timeout_sec = custom_timeout or self.timeout
        cmd = [self.cli_path] + args

        logger.debug(f"Launching C++ engine: {cmd}")

        proc = await asyncio.create_subprocess_exec(
            *cmd,
            stdin=asyncio.subprocess.PIPE if input_data is not None else None,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )

        try:
            stdin_bytes = input_data.encode("utf-8") if input_data is not None else None
            stdout_bytes, stderr_bytes = await asyncio.wait_for(
                proc.communicate(input=stdin_bytes),
                timeout=timeout_sec
            )
        except asyncio.TimeoutError:
            logger.error(f"C++ engine timed out after {timeout_sec}s. Terminating process.")
            try:
                proc.terminate()
                await asyncio.sleep(0.1)
                if proc.returncode is None:
                    proc.kill()
            except Exception as kill_err:
                logger.warning(f"Error terminating process: {kill_err}")
            raise EngineTimeoutError(timeout_sec)

        stdout_text = stdout_bytes.decode("utf-8", errors="replace").strip()
        stderr_text = stderr_bytes.decode("utf-8", errors="replace").strip()
        return_code = proc.returncode

        # Attempt to parse stdout as JSON
        parsed_json: Optional[Dict[str, Any]] = None
        if stdout_text:
            try:
                parsed_json = json.loads(stdout_text)
            except json.JSONDecodeError as jde:
                if return_code != 0:
                    raise EngineExecutionError(
                        code="ENGINE_NONZERO_EXIT",
                        message=stderr_text or f"C++ engine exited with code {return_code}",
                        exit_code=return_code,
                        stderr=stderr_text
                    )
                raise EngineMalformedJsonError(stdout_text, str(jde))

        if return_code != 0:
            error_code = "ENGINE_ERROR"
            error_msg = f"C++ engine process exited with code {return_code}"

            if isinstance(parsed_json, dict) and "error" in parsed_json and parsed_json["error"]:
                err_obj = parsed_json["error"]
                error_code = err_obj.get("code", error_code)
                error_msg = err_obj.get("message", error_msg)
            elif stderr_text:
                error_msg = stderr_text

            raise EngineExecutionError(
                code=error_code,
                message=error_msg,
                exit_code=return_code,
                stderr=stderr_text
            )

        if parsed_json is None:
            raise EngineMalformedJsonError("", "Engine returned empty output")

        return parsed_json

    async def get_problem_catalog(self) -> List[Dict[str, Any]]:
        """Queries the C++ engine for metadata of all registered DAA problems."""
        res = await self._execute_subprocess(["--list-problems", "--json"])
        if isinstance(res, list):
            return res
        raise EngineExecutionError("INVALID_CATALOG_FORMAT", "Expected JSON array from --list-problems")

    async def run_investigation(self, config_dict: Dict[str, Any]) -> Dict[str, Any]:
        """
        Runs a counterexample search in the C++ engine using configuration sent via stdin.
        """
        payload = json.dumps(config_dict)
        return await self._execute_subprocess(["--config-stdin", "--json"], input_data=payload)

    async def verify_candidate(self, verification_dict: Dict[str, Any]) -> Dict[str, Any]:
        """
        Directly validates and evaluates a single candidate instance via stdin without searching.
        """
        payload = json.dumps(verification_dict)
        return await self._execute_subprocess(["--verify-stdin", "--json"], input_data=payload)
