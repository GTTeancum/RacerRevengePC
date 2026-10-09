$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
# Wait for the current capture-off timing experiment to finish.
while($true){
    $rrPrior=Get-ChildItem (Join-Path $rrRoot 'evidence') -Directory -Filter 'captureOff47V-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if($rrPrior -and (Test-Path (Join-Path $rrPrior.FullName 'native.exit'))){break}
    Start-Sleep -Seconds 5
}
if((Get-Content (Join-Path $rrPrior.FullName 'native.exit')).Trim() -ne '0'){throw 'Require accepted capture-off run'}
$rrEvidence=Join-Path $rrRoot ('evidence/memoryEnv47W-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_memory_env47w.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Memory diagnostic preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/combined47T-20260930-172845-390/build.cmd') -Raw).Replace('LegacyHook47T','LegacyHook47W').Replace('--target racer_combined47t_fixed_probe','--target racer_memoryenv47w_fixed_probe racer_memoryenv47w_test')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 20;throw 'Memory diagnostic build failed'}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
foreach($rrName in @('racer_memoryenv47w_fixed_probe.exe','racer_memoryenv47w_test.exe')){Copy-Item (Join-Path $rrBuild $rrName) $rrEvidence}
Copy-Item (Join-Path $rrRoot 'build/memory47W/native.map') $rrEvidence
foreach($rrMode in @('0','1')){
    $env:RR_CACHE_MEMORY_DIAGNOSTICS47W=$rrMode
    & (Join-Path $rrEvidence 'racer_memoryenv47w_test.exe') $rrMode *> (Join-Path $rrEvidence "test-$rrMode.log")
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "test-$rrMode.exit")
    if($rrCode -ne 0){throw 'Memory environment ownership/control test failed'}
}
$rrGate=Join-Path $rrRoot 'evidence/47W';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash (Join-Path $rrEvidence 'racer_memoryenv47w_fixed_probe.exe')).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Memory/IPU diagnostic cache candidate; unit tests passed; native state acceptance pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
