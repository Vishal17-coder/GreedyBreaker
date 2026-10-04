param(
    [Parameter(Mandatory=$true)]
    [string]$FilePath
)

if (-not (Test-Path $FilePath)) {
    exit 0
}

try {
    $cert = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert -ErrorAction SilentlyContinue | Select-Object -First 1
    if (-not $cert) {
        $cert = New-SelfSignedCertificate -Type CodeSigningCert -Subject "CN=GreedyBreakerDev" -CertStoreLocation "Cert:\CurrentUser\My" -ErrorAction SilentlyContinue
    }
    if ($cert) {
        Set-AuthenticodeSignature -FilePath $FilePath -Certificate $cert -ErrorAction SilentlyContinue | Out-Null
    }
} catch {
    # Ignore signing errors in non-privileged environments
}
