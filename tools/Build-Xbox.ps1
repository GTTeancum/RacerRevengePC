param([int]$Jobs = 4)
$ErrorActionPreference = 'Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrBash = Join-Path $rrRoot 'toolchains\msys64\usr\bin\bash.exe'
if (!(Test-Path -LiteralPath $rrBash)) { throw 'Project-local MSYS2 is not provisioned.' }
$rrEvidence = Join-Path $rrRoot ('evidence\xbox-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrOldMsystem = $env:MSYSTEM
$rrOldRoot = $env:RR_PROJECT_ROOT
$rrOldJobs = $env:RR_BUILD_JOBS
try {
    $env:MSYSTEM = 'MINGW64'
    $env:RR_PROJECT_ROOT = $rrRoot
    $env:RR_BUILD_JOBS = [string]$Jobs
    & $rrBash --login -c 'set -e; rrroot=$(cygpath -u "$RR_PROJECT_ROOT"); export NXDK_DIR="$rrroot/toolchains/nxdk"; export PATH="$NXDK_DIR/bin:$PATH"; clang --version; lld-link --version; make -C "$NXDK_DIR/tools/cxbe" CXX=clang++; cd "$rrroot/platform/xbox"; make -j"$RR_BUILD_JOBS"' 2>&1 |
        Tee-Object -FilePath (Join-Path $rrEvidence 'build.log')
    $rrExit = $LASTEXITCODE
    Set-Content -LiteralPath (Join-Path $rrEvidence 'build.exit') -Value $rrExit
    if ($rrExit -ne 0) { throw "Xbox cross-build failed ($rrExit); logs: $rrEvidence" }
    Get-FileHash -LiteralPath "$rrRoot\build\xbox-core\default.xbe" -Algorithm SHA256 |
        ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence 'xbe-hash.json')
    Write-Output "Xbox core validation image built; execution is unverified. Evidence: $rrEvidence"
} finally {
    $env:MSYSTEM = $rrOldMsystem
    $env:RR_PROJECT_ROOT = $rrOldRoot
    $env:RR_BUILD_JOBS = $rrOldJobs
}
