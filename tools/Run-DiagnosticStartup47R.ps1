param([string]$AfterRace='evidence/launchEnv-race47Q-20260930-164842-029')
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=Join-Path $rrRoot "$AfterRace/race.exit"
while(-not (Test-Path $rrAfter)){Start-Sleep -Seconds 3}
if((Get-Content $rrAfter).Trim() -ne '0'){throw 'Require accepted IOP-cache race pair'}
$rrEvidence=Join-Path $rrRoot 'evidence/47R'
$rrGate=Get-Content (Join-Path $rrEvidence 'build-gate.json') -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@()
foreach($rrMode in @('0','1')){
    & (Join-Path $PSScriptRoot 'Run-Native47R.ps1') -Seconds 180 -Vblank 500 -DiagnosticLines $rrMode -DiagnosticCache 1 -DisableGsProfile -QuietTrace | Tee-Object (Join-Path $rrEvidence "$rrMode-startup.log")
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47r-$rrMode-1-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'startup-runs.json')
    & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot 'work/native-run47q-1-0-1-1-1-1-20260930-164739-945') $rrRun.FullName --output (Join-Path $rrEvidence "$rrMode-startup-state.json") *> (Join-Path $rrEvidence "$rrMode-state.log")
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrMode-state.exit")
    if($rrCode -ne 0){throw 'Diagnostic buffering changed startup state'}
}
0 | Set-Content (Join-Path $rrEvidence 'startup.exit')
Write-Output 'Diagnostic line-buffer off/on startup saved states match.'
