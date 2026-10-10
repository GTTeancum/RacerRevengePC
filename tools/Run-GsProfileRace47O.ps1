param([string]$AfterRace='evidence/memory-race47N-20260930-160118-731')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=(Resolve-Path -LiteralPath (Join-Path $rrRoot $AfterRace)).Path
$rrEvidence=Join-Path $rrRoot ('evidence/gsProfile-race47O-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrBin=Join-Path $rrEvidence 'bin';New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrExe=Join-Path $rrBin 'racer_gsprofile47o_fixed_probe.exe'
Copy-Item -LiteralPath (Join-Path $rrBuild 'racer_gsprofile47o_fixed_probe.exe') -Destination $rrExe
Get-ChildItem -LiteralPath $rrBuild -Filter '*.dll' | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $rrBin}
$rrGate=Get-Content (Join-Path $rrRoot 'evidence/47O/build-gate.json') -Raw | ConvertFrom-Json
if((Get-FileHash -LiteralPath $rrExe).Hash.ToLowerInvariant() -ne $rrGate.binary_sha256){throw 'Unverified profile binary'}
if((Get-Content (Join-Path $rrRoot 'evidence/47O/startup.exit')).Trim() -ne '0'){throw 'Require off/on startup and counter validation'}
@{queued_utc=[DateTime]::UtcNow.ToString('o');after_race=$rrAfter;binary_sha256=$rrGate.binary_sha256;vblank=7000;raw_mode=1;status='queued; no race profile or race acceptance yet'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'queued.json')
Write-Output "Renderer call profile queued after combined-registry race: $rrEvidence"
while(-not (Test-Path (Join-Path $rrAfter 'race.exit'))){Start-Sleep -Seconds 2}
if((Get-Content (Join-Path $rrAfter 'race.exit')).Trim() -ne '0'){throw 'Registry race failed; inspect before profile run'}
try {
    [DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
    & (Join-Path $PSScriptRoot 'Run-Native47O.ps1') -Seconds 2400 -Vblank 7000 -RawMode 1 -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence 'run.log')
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47o-1-1-1-1-1-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    @{run=$rrRun.FullName} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'run.json')
    $rrReference=(Get-Content (Join-Path $rrAfter 'run.json') -Raw | ConvertFrom-Json).run
    $rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
    & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrReference $rrRun.FullName --output (Join-Path $rrEvidence 'state.json') *> (Join-Path $rrEvidence 'state.log')
    if($LASTEXITCODE -ne 0){throw 'Profiled race saved state differs'}
    & $rrPython (Join-Path $PSScriptRoot 'analyze_gs_profile47o.py') $rrRun.FullName --output (Join-Path $rrEvidence 'profile.json') *> (Join-Path $rrEvidence 'profile.log')
    if($LASTEXITCODE -ne 0){throw 'Race profile validation failed'}
    $rrReport=Get-Content (Join-Path $rrEvidence 'profile.json') -Raw | ConvertFrom-Json
    if(-not $rrReport.terminal_vsync_reconciled -or $null -eq $rrReport.window){throw 'Missing reconciled race-window profile'}
    0 | Set-Content (Join-Path $rrEvidence 'race.exit')
    [DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrEvidence 'error.log')
    1 | Set-Content (Join-Path $rrEvidence 'race.exit')
    throw
}
