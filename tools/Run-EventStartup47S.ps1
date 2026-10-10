param([string]$AfterRepeat='evidence/launchEnv-repeat47Q-20260930-171243-658')
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=Join-Path $rrRoot "$AfterRepeat/race.exit"
while(-not (Test-Path $rrAfter)){Start-Sleep -Seconds 3}
if((Get-Content $rrAfter).Trim() -ne '0'){throw 'Require clean control repeat state validation'}
$rrEvidence=Join-Path $rrRoot 'evidence/47S'
$rrGate=Get-Content (Join-Path $rrEvidence 'build-gate.json') -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@()
try {
    foreach($rrMode in @('0','1')){
        & (Join-Path $PSScriptRoot 'Run-Native47S.ps1') -Seconds 180 -Vblank 500 -EventGate $rrMode -DiagnosticCache 1 -DisableGsProfile -QuietTrace | Tee-Object (Join-Path $rrEvidence "$rrMode-startup.log")
        $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47s-$rrMode-1-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'startup-runs.json')
        & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot 'work/native-run47q-1-0-1-1-1-1-20260930-164739-945') $rrRun.FullName --output (Join-Path $rrEvidence "$rrMode-startup-state.json") *> (Join-Path $rrEvidence "$rrMode-state.log")
        $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrMode-state.exit")
        if($rrCode -ne 0){throw 'Scheduler event gate changed startup state'}
    }
    0 | Set-Content (Join-Path $rrEvidence 'startup.exit')
} catch {$_ | Out-String | Set-Content (Join-Path $rrEvidence 'startup-error.log');1 | Set-Content (Join-Path $rrEvidence 'startup.exit');throw}
Write-Output 'Scheduler event gate off/on native startup states match.'
