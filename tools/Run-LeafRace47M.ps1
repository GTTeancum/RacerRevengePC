param([string]$AfterSweep='evidence/native-sweep47L-20260930-152035-712')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrSweep=(Resolve-Path -LiteralPath (Join-Path $rrRoot $AfterSweep)).Path
$rrEvidence=Join-Path $rrRoot ('evidence/leaf-race47M-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrBin=Join-Path $rrEvidence 'bin'
New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrExe=Join-Path $rrBin 'racer_leaf47m_fixed_probe.exe'
Copy-Item -LiteralPath (Join-Path $rrBuild 'racer_leaf47m_fixed_probe.exe') -Destination $rrExe
Get-ChildItem -LiteralPath $rrBuild -Filter '*.dll' | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $rrBin}
$rrGate=Get-Content (Join-Path $rrRoot 'evidence/47M/accepted-native-build.json') -Raw | ConvertFrom-Json
if((Get-FileHash -LiteralPath $rrExe).Hash.ToLowerInvariant() -ne $rrGate.binary_sha256){throw 'Unverified leaf binary'}
if((Get-Content (Join-Path $rrRoot 'evidence/47M/startup-state.exit')).Trim() -ne '0'){throw 'Require startup equivalence'}
@{queued_utc=[DateTime]::UtcNow.ToString('o');after_sweep=$rrSweep;binary_sha256=$rrGate.binary_sha256;vblank=7000;raw_mode=1;status='queued; no race acceptance yet'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'queued.json')
Write-Output "Leaf race queued after graphics sweep: $rrEvidence"
while(-not (Test-Path (Join-Path $rrSweep 'sweep.exit'))){Start-Sleep -Seconds 2}
if((Get-Content (Join-Path $rrSweep 'sweep.exit')).Trim() -ne '0'){throw 'Graphics sweep failed; inspect before next run'}
try {
    [DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
    & (Join-Path $PSScriptRoot 'Run-Native47M.ps1') -Seconds 2400 -Vblank 7000 -RawMode 1 -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence 'run.log')
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47m-1-1-1-1-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    @{run=$rrRun.FullName} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'run.json')
    $rrReferences=Get-Content (Join-Path $rrSweep 'runs.json') -Raw | ConvertFrom-Json
    $rrReference=($rrReferences | Where-Object mode -eq '1').run
    & 'C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrReference $rrRun.FullName --output (Join-Path $rrEvidence 'state.json') *> (Join-Path $rrEvidence 'state.log')
    if($LASTEXITCODE -ne 0){throw 'Leaf race saved state differs'}
    0 | Set-Content (Join-Path $rrEvidence 'race.exit')
    [DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrEvidence 'error.log')
    1 | Set-Content (Join-Path $rrEvidence 'race.exit')
    throw
}
