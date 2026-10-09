param(
    [ValidateRange(1,7200)][int]$Seconds=3000,
    [ValidateRange(1,10000000)][int]$Vblank=7000
)
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrGate=Get-Content (Join-Path $rrRoot 'evidence/47H/corrected-vblank500-comparison.json') -Raw | ConvertFrom-Json
if(-not $rrGate.saved_state_equivalent){throw 'Corrected startup comparison is not verified'}
$rrPair=Join-Path $rrRoot ('evidence/native-pair47H-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrBin=Join-Path $rrPair 'bin'
New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrExe=Join-Path $rrBin 'racer_iop47h_fixed_probe.exe'
Copy-Item -LiteralPath (Join-Path $rrBuild 'racer_iop47h_fixed_probe.exe') -Destination $rrExe
Get-ChildItem -LiteralPath $rrBuild -Filter '*.dll' -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $rrBin $_.Name)
}
$rrHash=(Get-FileHash -LiteralPath $rrExe).Hash
@{seconds_limit_each=$Seconds;vblank=$Vblank;utc=[DateTime]::UtcNow.ToString('o');order=@('baseline-0-0','optimized-1-1');binary=$rrExe;binary_sha256=$rrHash;comparison='same frozen executable; corrected idle batching and ready-selection cache off/off versus on/on';acceptance='pending exact state and race timing comparison'} | ConvertTo-Json | Set-Content (Join-Path $rrPair 'pair.json')
try {
    foreach($rrMode in @('0','1')) {
        if((Get-FileHash -LiteralPath $rrExe).Hash -cne $rrHash){throw 'Frozen pair binary changed'}
        & (Join-Path $PSScriptRoot 'Run-Native47H.ps1') -Seconds $Seconds -Vblank $Vblank -ReadyCache $rrMode -IdleBatch $rrMode -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrPair ($rrMode+'.log'))
    }
    0 | Set-Content (Join-Path $rrPair 'pair.exit')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrPair 'error.log')
    1 | Set-Content (Join-Path $rrPair 'pair.exit')
    throw
}
