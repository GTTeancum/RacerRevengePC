$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/map47P-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47P').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_gsprofile47o_fixed_probe')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){throw "Map relink failed: $rrEvidence"}
Copy-Item (Join-Path $rrRoot 'work/native/ninja-build-release-opt/racer_gsprofile47o_fixed_probe.exe') $rrEvidence
Copy-Item (Join-Path $rrRoot 'build/gsProfile47O/profile47p.map') $rrEvidence
Write-Output $rrEvidence
