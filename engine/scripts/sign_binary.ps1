param(
    [Parameter(Mandatory=$true)]
    [string]$FilePath
)

$cert = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert | Select-Object -First 1
if (-not $cert) {
    $cert = New-SelfSignedCertificate -Type CodeSigningCert -Subject "CN=GreedyBreakerDev" -CertStoreLocation "Cert:\CurrentUser\My"
}

if (Test-Path $FilePath) {
    Set-AuthenticodeSignature -FilePath $FilePath -Certificate $cert | Out-Null
}
