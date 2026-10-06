export function formatDate(iso: string): string {
  return new Date(iso).toLocaleString('en-GB', {
    day: '2-digit',
    month: 'short',
    year: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
  });
}

export function formatMs(ms: number): string {
  if (ms < 1) return `${(ms * 1000).toFixed(0)} µs`;
  if (ms < 1000) return `${ms.toFixed(1)} ms`;
  return `${(ms / 1000).toFixed(2)} s`;
}

export function formatNumber(n: number): string {
  return n.toLocaleString('en-US');
}

export function formatValue(val: number | null | undefined): string {
  if (val == null) return '—';
  return Number.isInteger(val) ? String(val) : val.toFixed(4);
}
