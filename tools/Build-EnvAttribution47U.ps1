$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrAfter=Join-Path $rrRoot 'evidence/combined-race47T-20260930-173405-793/race.exit'
while(-not (Test-Path $rrAfter)){Start-Sleep -Seconds 5}
if((Get-Content $rrAfter).Trim() -ne '0'){throw 'Combined full race must pass first'}
$rrEvidence=Join-Path $rrRoot ('evidence/env47U-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_env_attribution47u.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Attribution preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/combined47T-20260930-172845-390/build.cmd') -Raw).Replace('LegacyHook47T','LegacyHook47U').Replace('--target racer_combined47t_fixed_probe','--target racer_env47u_fixed_probe racer_env47u_test')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 20;throw 'Attribution build failed'}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
foreach($rrName in @('racer_env47u_fixed_probe.exe','racer_env47u_test.exe')){Copy-Item (Join-Path $rrBuild $rrName) $rrEvidence}
Copy-Item (Join-Path $rrRoot 'build/env47U/native.map') $rrEvidence
$env:RR_ENV_ATTRIBUTION47U=Join-Path $rrEvidence 'test-env'
& (Join-Path $rrEvidence 'racer_env47u_test.exe') *> (Join-Path $rrEvidence 'test.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'test.exit')
if($rrCode -ne 0){throw 'Environment forwarding test failed'}
& $rrPython (Join-Path $PSScriptRoot 'check_env_attribution47u.py') $rrEvidence *> (Join-Path $rrEvidence 'count-check.json')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'count-check.exit')
if($rrCode -ne 0){throw 'Environment caller count check failed'}
Remove-Item Env:RR_ENV_ATTRIBUTION47U
$rrGate=Join-Path $rrRoot 'evidence/47U';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash (Join-Path $rrEvidence 'racer_env47u_fixed_probe.exe')).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Diagnostic import attribution, standalone forwarding/count checks only; native validation pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
