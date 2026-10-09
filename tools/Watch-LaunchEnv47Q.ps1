param([ValidateSet('cpu','sample')][string]$Kind='cpu',[string]$RaceEvidence='evidence/launchEnv-race47Q-20260930-164842-029')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrRace=Get-Content (Join-Path $rrRoot "$RaceEvidence/queued.json") -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
foreach($rrMode in @('0','1')){
    $rrRun=$null
    while(-not $rrRun){
        foreach($rrCandidate in (Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47q-$rrMode-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending)){
            $rrMeta=Join-Path $rrCandidate.FullName 'process-launch.json'
            if(-not (Test-Path $rrMeta)){continue}
            try {$rrLaunch=Get-Content (Join-Path $rrCandidate.FullName 'launch.json') -Raw | ConvertFrom-Json;$rrProc=Get-Content $rrMeta -Raw | ConvertFrom-Json} catch {continue}
            if($rrLaunch.stop_at_vblank -eq 7000 -and ([DateTime]$rrProc.utc) -gt ([DateTime]$rrRace.utc) -and $rrProc.binary_sha256 -eq $rrRace.binary_sha256){$rrRun=$rrCandidate;break}
        }
        if(-not $rrRun){Start-Sleep -Seconds 3}
    }
    $rrOut=Join-Path $rrRoot "evidence/observer47Q-$Kind-$rrMode"
    if($Kind -eq 'cpu'){
        & $rrPython (Join-Path $PSScriptRoot 'watch_process_io47q.py') $rrRun.FullName --output $rrOut *> "$rrOut.log"
    } else {
        # Keep instrumented PSS capture well before the fixed 6000–6900 timing window.
        while($true){
            if(Test-Path (Join-Path $rrRun.FullName 'native.exit')){throw 'Process ended before sampling window'}
            $rrFrame=Get-ChildItem $rrRun.FullName -Filter 'frame-*.json' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
            if($rrFrame){try {$rrState=Get-Content $rrFrame.FullName -Raw | ConvertFrom-Json} catch {Start-Sleep -Seconds 2;continue};if($rrState.vsync -ge 5200){break}}
            Start-Sleep -Seconds 2
        }
        if($rrState.vsync -ge 5700){throw 'Sampling window was missed; do not disturb race timing'}
        & $rrPython (Join-Path $PSScriptRoot 'sample_process47q.py') $rrRun.FullName --output $rrOut --seconds 20 --interval-ms 100 *> "$rrOut.log"
    }
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content "$rrOut.exit"
    if($rrCode -ne 0){throw "Observer failed: $rrOut"}
    Write-Output "Closed $Kind observer for mode $rrMode : $rrOut"
}
