param([string]$AfterRace='evidence/leaf-race47M-20260930-154837-614')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=(Resolve-Path -LiteralPath (Join-Path $rrRoot $AfterRace)).Path
$rrEvidence=Join-Path $rrRoot ('evidence/memory-race47N-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrBin=Join-Path $rrEvidence 'bin';New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrExe=Join-Path $rrBin 'racer_memory47n_fixed_probe.exe'
Copy-Item -LiteralPath (Join-Path $rrBuild 'racer_memory47n_fixed_probe.exe') -Destination $rrExe
Get-ChildItem -LiteralPath $rrBuild -Filter '*.dll' | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $rrBin}
$rrGate=(Get-Content (Join-Path $rrRoot 'evidence/47N/build-gate.json') -Raw | ConvertFrom-Json).memory
if((Get-FileHash -LiteralPath $rrExe).Hash.ToLowerInvariant() -ne $rrGate.binary_sha256){throw 'Unverified binary'}
if((Get-Content (Join-Path $rrRoot 'evidence/47N/startup.exit')).Trim() -ne '0'){throw 'Require both startup comparisons'}
@{queued_utc=[DateTime]::UtcNow.ToString('o');after_race=$rrAfter;binary_sha256=$rrGate.binary_sha256;vblank=7000;raw_mode=1;status='queued; no full race acceptance'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'queued.json')
Write-Output "Combined registry/leaf race queued: $rrEvidence"
while(-not (Test-Path (Join-Path $rrAfter 'race.exit'))){Start-Sleep -Seconds 2}
if((Get-Content (Join-Path $rrAfter 'race.exit')).Trim() -ne '0'){throw 'Leaf race failed; inspect before registry run'}
try {
    [DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
    & (Join-Path $PSScriptRoot 'Run-Native47N.ps1') -MemoryVariant memory -Seconds 2400 -Vblank 7000 -RawMode 1 -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence 'run.log')
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47n-memory-1-1-1-1-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    @{run=$rrRun.FullName} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'run.json')
    $rrReference=(Get-Content (Join-Path $rrAfter 'run.json') -Raw | ConvertFrom-Json).run
    & 'C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrReference $rrRun.FullName --output (Join-Path $rrEvidence 'state.json') *> (Join-Path $rrEvidence 'state.log')
    if($LASTEXITCODE -ne 0){throw 'Combined registry/leaf state differs'}
    0 | Set-Content (Join-Path $rrEvidence 'race.exit')
    [DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrEvidence 'error.log')
    1 | Set-Content (Join-Path $rrEvidence 'race.exit')
    throw
}
