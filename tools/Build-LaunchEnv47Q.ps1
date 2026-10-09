$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/launchEnv47Q-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_launch_env47q.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'IOP clone preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47Q').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_launch47q_fixed_probe racer_iop47q_differential')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 15;throw "Build failed: $rrEvidence"}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
Copy-Item (Join-Path $rrBuild 'racer_launch47q_fixed_probe.exe') $rrEvidence
Copy-Item (Join-Path $rrBuild 'racer_iop47q_differential.exe') $rrEvidence
Copy-Item (Join-Path $rrRoot 'build/iop47Q/native.map') $rrEvidence
foreach($rrMode in @('0','1')){
    $env:RR_CACHE_DIAGNOSTICS47Q=$rrMode
    & (Join-Path $rrBuild 'racer_iop47q_differential.exe') *> (Join-Path $rrEvidence "differential-$rrMode.log")
    $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "differential-$rrMode.exit")
    if($rrCode -ne 0){throw 'IOP differential regression failed'}
}
$rrGate=Join-Path $rrRoot 'evidence/47Q';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash (Join-Path $rrEvidence 'racer_launch47q_fixed_probe.exe')).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Built candidate; IOP fixture regression passed; native startup/race pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
