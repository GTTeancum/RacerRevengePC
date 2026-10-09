$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
foreach($rrVersion in @('47R','47S2')){
    $rrStartup=Join-Path $rrRoot "evidence/$rrVersion/startup.exit"
    while(-not (Test-Path $rrStartup)){Start-Sleep -Seconds 3}
    if((Get-Content $rrStartup).Trim() -ne '0'){throw "Require accepted $rrVersion startup"}
}
$rrEvidence=Join-Path $rrRoot ('evidence/combined47T-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_combined47t.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Combined preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47T').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_combined47t_fixed_probe')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 15;throw 'Combined build failed'}
$rrExe=Join-Path $rrRoot 'work/native/ninja-build-release-opt/racer_combined47t_fixed_probe.exe';Copy-Item $rrExe $rrEvidence
$rrGate=Join-Path $rrRoot 'evidence/47T';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash $rrExe).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Composition built after R/S2 startups; combined validation pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
