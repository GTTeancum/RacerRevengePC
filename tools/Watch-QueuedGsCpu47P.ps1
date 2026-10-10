param([string]$RaceEvidence='evidence/gsProfile-race47O-20260930-161223-107')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrRace=Join-Path $rrRoot $RaceEvidence
$rrQueued=Get-Content (Join-Path $rrRace 'queued.json') -Raw | ConvertFrom-Json
Write-Output 'Waiting for the already queued renderer profile before observing CPU counters.'
while($true){
    if(Test-Path (Join-Path $rrRace 'race.exit')){throw 'Queued profile ended before observer attached'}
    if(Test-Path (Join-Path $rrRace 'start.txt')){
        $rrStart=[DateTimeOffset]::Parse((Get-Content (Join-Path $rrRace 'start.txt')).Trim())
        $rrCandidates=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47o-1-1-1-1-1-*' | Sort-Object LastWriteTime -Descending
        foreach($rrCandidate in $rrCandidates){
            if(-not (Test-Path (Join-Path $rrCandidate.FullName 'process-launch.json'))){continue}
            try {
                $rrMeta=Get-Content (Join-Path $rrCandidate.FullName 'process-launch.json') -Raw | ConvertFrom-Json
                $rrLaunch=Get-Content (Join-Path $rrCandidate.FullName 'launch.json') -Raw | ConvertFrom-Json
            } catch {continue}
            if([DateTimeOffset]::Parse($rrMeta.utc) -ge $rrStart -and $rrLaunch.stop_at_vblank -eq 7000 -and $rrMeta.binary_sha256.ToLowerInvariant() -eq $rrQueued.binary_sha256){$rrRun=$rrCandidate.FullName;break}
        }
        if($rrRun){break}
    }
    Start-Sleep -Seconds 2
}
$rrOutput=Join-Path $rrRoot 'evidence/threadCpu47P-gsProfile'
& 'C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' (Join-Path $PSScriptRoot 'watch_thread_cpu47p.py') $rrRun --output $rrOutput
if($LASTEXITCODE -ne 0){throw 'Thread CPU observer failed'}
