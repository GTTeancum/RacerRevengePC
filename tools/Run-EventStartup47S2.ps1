param([string]$AfterRepeat='evidence/launchEnv-repeat47Q-20260930-171243-658')
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=Join-Path $rrRoot "$AfterRepeat/race.exit"
$rrEvidence=Join-Path $rrRoot 'evidence/47S2';$rrGatePath=Join-Path $rrEvidence 'build-gate.json'
while(-not (Test-Path $rrAfter) -or -not (Test-Path $rrGatePath)){Start-Sleep -Seconds 3}
if((Get-Content $rrAfter).Trim() -ne '0'){throw 'Require clean control repeat validation'}
$rrGate=Get-Content $rrGatePath -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@()
try {
    foreach($rrMode in @('0','1')){
        & (Join-Path $PSScriptRoot 'Run-Native47S2.ps1') -Seconds 180 -Vblank 500 -EventGate $rrMode -DiagnosticCache 1 -DisableGsProfile -QuietTrace | Tee-Object (Join-Path $rrEvidence "$rrMode-startup.log")
        $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47s2-$rrMode-1-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'startup-runs.json')
        & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot 'work/native-run47q-1-0-1-1-1-1-20260930-164739-945') $rrRun.FullName --output (Join-Path $rrEvidence "$rrMode-startup-state.json") *> (Join-Path $rrEvidence "$rrMode-state.log")
        $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrMode-state.exit")
        if($rrCode -ne 0){throw 'Conservative event gate changed native startup state'}
    }
    0 | Set-Content (Join-Path $rrEvidence 'startup.exit')
} catch {$_ | Out-String | Set-Content (Join-Path $rrEvidence 'startup-error.log');1 | Set-Content (Join-Path $rrEvidence 'startup.exit');throw}
Write-Output 'Conservative scheduler event gate off/on native startup states match.'
