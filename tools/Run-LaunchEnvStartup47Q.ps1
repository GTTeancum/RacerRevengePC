$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot 'evidence/47Q'
$rrGate=Get-Content (Join-Path $rrEvidence 'build-gate.json') -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@()
foreach($rrMode in @('0','1')){
    & (Join-Path $PSScriptRoot 'Run-Native47Q.ps1') -Seconds 180 -Vblank 500 -DiagnosticCache $rrMode -DisableGsProfile -QuietTrace | Tee-Object (Join-Path $rrEvidence "$rrMode-startup.log")
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47q-$rrMode-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName}
    $rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'startup-runs.json')
    & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot 'work/native-run47n-memory-1-1-1-1-20260930-155829-832') $rrRun.FullName --output (Join-Path $rrEvidence "$rrMode-startup-state.json") *> (Join-Path $rrEvidence "$rrMode-state.log")
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrMode-state.exit")
    if($rrCode -ne 0){throw 'Diagnostic cache changed startup saved state'}
}
0 | Set-Content (Join-Path $rrEvidence 'startup.exit')
Write-Output 'Diagnostic-cache off/on startup saved states match.'
