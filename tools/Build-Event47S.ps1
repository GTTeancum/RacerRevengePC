param([Parameter(Mandatory=$true)][string]$AfterRun)
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/event47S-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrRun=(Resolve-Path -LiteralPath (Join-Path $rrRoot $AfterRun)).Path
while($true){
    $rrFrame=Get-ChildItem $rrRun -Filter 'frame-*.json' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if($rrFrame){try {$rrState=Get-Content $rrFrame.FullName -Raw | ConvertFrom-Json} catch {Start-Sleep -Seconds 2;continue};if($rrState.vsync -ge 6950){break}}
    if(Test-Path (Join-Path $rrRun 'native.exit')){if((Get-Content (Join-Path $rrRun 'native.exit')).Trim() -ne '0'){throw 'Candidate run failed'};break}
    Start-Sleep -Seconds 2
}
@{run=$rrRun;utc=[DateTime]::UtcNow.ToString('o');vsync=$rrState.vsync;frame=$rrFrame.Name} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'timing-window-gate.json')
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_event_gate47s.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Scheduler preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47S').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_event47s_fixed_probe racer_event47s_test')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 20;throw 'Event gate build failed'}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
foreach($name in @('racer_event47s_fixed_probe.exe','racer_event47s_test.exe')){Copy-Item (Join-Path $rrBuild $name) $rrEvidence}
Remove-Item Env:RR_STOP_AT_VBLANK47F,Env:RR_PCSX2_GS_DLL,Env:RR_CAPTURE_SEQUENCE,Env:RR_PROFILE_HOST,Env:RR_PROFILE_EE -ErrorAction SilentlyContinue
$env:RR_CACHE_DIAGNOSTICS47Q='1'
foreach($rrMode in @('0','1')){
    $env:RR_EE_EVENT_GATE47S=$rrMode
    & (Join-Path $rrBuild 'racer_event47s_test.exe') *> (Join-Path $rrEvidence "fixture-$rrMode.log")
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "fixture-$rrMode.exit")
    if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence "fixture-$rrMode.log") -Tail 12;throw 'Actual scheduler fixture failed'}
}
$rrA=Get-Content (Join-Path $rrEvidence 'fixture-0.log') | Where-Object {$_ -like 'EVENT47S *'}
$rrB=Get-Content (Join-Path $rrEvidence 'fixture-1.log') | Where-Object {$_ -like 'EVENT47S *'}
if(-not $rrA -or $rrA -cne $rrB){throw 'Event traces differ between modes'}
$rrCounter=Select-String -Path (Join-Path $rrEvidence 'fixture-1.log') -Pattern '\[rr:event47s\] fast=([1-9][0-9]*) full=([0-9]+)'
if(-not $rrCounter){throw 'Fixture did not exercise event fast return'}
0 | Set-Content (Join-Path $rrEvidence 'fixture-compare.exit')
$rrGate=Join-Path $rrRoot 'evidence/47S';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash (Join-Path $rrEvidence 'racer_event47s_fixed_probe.exe')).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Built event gate; real scheduler alarm, masked/unmasked GS/VIF and asynchronous stop fixture passed off/on; native game validation pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
