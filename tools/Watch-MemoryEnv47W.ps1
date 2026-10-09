$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrGate=Get-Content (Join-Path $rrRoot 'evidence/47W/build-gate.json') -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
foreach($rrMode in @('0','1')){
    $rrRun=$null
    while(-not $rrRun){
        foreach($rrCandidate in (Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47w-$rrMode-1-1-1-1-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending)){
            try {
                $rrLaunch=Get-Content (Join-Path $rrCandidate.FullName 'launch.json') -Raw | ConvertFrom-Json
                $rrProc=Get-Content (Join-Path $rrCandidate.FullName 'process-launch.json') -Raw | ConvertFrom-Json
            } catch {continue}
            if($rrLaunch.stop_at_vblank -eq 7000 -and $rrProc.binary_sha256.ToLowerInvariant() -eq $rrGate.binary_sha256){$rrRun=$rrCandidate;break}
        }
        if(-not $rrRun){Start-Sleep -Seconds 3}
    }
    $rrOut=Join-Path $rrRoot "evidence/observer47W-cpu-$rrMode"
    & $rrPython (Join-Path $PSScriptRoot 'watch_process_io47q.py') $rrRun.FullName --output $rrOut *> "$rrOut.log"
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content "$rrOut.exit"
    if($rrCode -ne 0){throw 'Memory candidate CPU/IO observer failed'}
    Write-Output "Completed CPU/IO observer for memory mode $rrMode"
}
