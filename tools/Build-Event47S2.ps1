param([string]$AfterRun='work/native-run47q-0-0-1-1-1-1-20260930-171244-067')
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/event47S2-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrRun=Join-Path $rrRoot $AfterRun
while($true){
    $rrFrame=Get-ChildItem $rrRun -Filter 'frame-*.json' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if($rrFrame){try {$rrState=Get-Content $rrFrame.FullName -Raw | ConvertFrom-Json} catch {Start-Sleep -Seconds 2;continue};if($rrState.vsync -ge 6950){break}}
    if(Test-Path (Join-Path $rrRun 'native.exit')){if((Get-Content (Join-Path $rrRun 'native.exit')).Trim() -ne '0'){throw 'Clean control failed'};break}
    Start-Sleep -Seconds 2
}
@{utc=[DateTime]::UtcNow.ToString('o');run=$rrRun;vsync=$rrState.vsync;frame=$rrFrame.Name} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'timing-window-gate.json')
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrBase=Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw
function Build-Phase([string]$Phase,[string]$Hook,[string]$Targets){
    $rrCommand=$rrBase.Replace('LegacyHook47L',$Hook).Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check',$Targets)
    $rrBatch=Join-Path $rrEvidence "$Phase.cmd";$rrCommand | Set-Content $rrBatch -Encoding ascii
    & $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence "$Phase-build.log")
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$Phase-build.exit")
    if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence "$Phase-build.log") -Tail 15;throw "Build failed: $Phase"}
}
function Run-Fixture([string]$Phase,[string]$Name,[string]$Flag){
    foreach($rrMode in @('0','1')){
        Set-Item "Env:$Flag" $rrMode
        & (Join-Path $rrBuild "$Name.exe") *> (Join-Path $rrEvidence "$Phase-$rrMode.log")
        $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$Phase-$rrMode.exit")
        if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence "$Phase-$rrMode.log") -Tail 10;throw "Fixture failed: $Phase mode $rrMode"}
    }
}
Remove-Item Env:RR_STOP_AT_VBLANK47F,Env:RR_PCSX2_GS_DLL,Env:RR_CAPTURE_SEQUENCE,Env:RR_PROFILE_HOST,Env:RR_PROFILE_EE -ErrorAction SilentlyContinue
$env:RR_CACHE_DIAGNOSTICS47Q='1'
Build-Phase 'original' 'LegacyHook47S' 'racer_event47s_test'
Copy-Item (Join-Path $rrBuild 'racer_event47s_test.exe') (Join-Path $rrEvidence 'original-test.exe')
Copy-Item (Join-Path $rrRoot 'build/event47S/EeScheduler.cpp') (Join-Path $rrEvidence 'original-scheduler.cpp')
Copy-Item (Join-Path $rrRoot 'tests/event_gate47s.cpp') (Join-Path $rrEvidence 'zero-alarm-fixture.cpp')
Run-Fixture 'original' 'racer_event47s_test' 'RR_EE_EVENT_GATE47S'
$rrA=Get-Content (Join-Path $rrEvidence 'original-0.log') | Where-Object {$_ -like 'EVENT47S *'}
$rrB=Get-Content (Join-Path $rrEvidence 'original-1.log') | Where-Object {$_ -like 'EVENT47S *'}
if(-not $rrA -or -not $rrB){throw 'Missing expanded alarm trace'}
@{reference=$rrA;original_fast=$rrB;trace_divergence_reproduced=($rrA -cne $rrB);public_api='setAlarm normalizes zero ticks to one; no cycle-zero bug is claimed';scope='Expanded same-deadline alarm ordering regression'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'alarm-boundary.json')
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_event_gate47s2.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'S2 preparation failed'}
Build-Phase 'fixed' 'LegacyHook47S2' 'racer_event47s2_test racer_event47s2_fixed_probe'
foreach($rrName in @('racer_event47s2_test.exe','racer_event47s2_fixed_probe.exe')){Copy-Item (Join-Path $rrBuild $rrName) $rrEvidence}
Copy-Item (Join-Path $rrRoot 'build/event47S2/EeScheduler.cpp') (Join-Path $rrEvidence 'fixed-scheduler.cpp')
Run-Fixture 'fixed' 'racer_event47s2_test' 'RR_EE_EVENT_GATE47S2'
foreach($rrMode in @('0','1')){
    $rrFixed=Get-Content (Join-Path $rrEvidence "fixed-$rrMode.log") | Where-Object {$_ -like 'EVENT47S *'}
    if($rrFixed -cne $rrA){throw 'Fixed scheduler trace differs from original processing'}
}
if(-not (Select-String -Path (Join-Path $rrEvidence 'fixed-1.log') -Pattern '\[rr:event47s2\] fast=([1-9][0-9]*) full=')){throw 'Fixed fast path not exercised'}
0 | Set-Content (Join-Path $rrEvidence 'fixture-compare.exit')
$rrGate=Join-Path $rrRoot 'evidence/47S2';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash (Join-Path $rrEvidence 'racer_event47s2_fixed_probe.exe')).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Conservative zero-marker path and expanded alarm/IRQ/stop fixture match original event order and cycles; no public-API zero-cycle bug claimed; native game validation pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
