param([string]$BuildEvidence='evidence/memory47N-20260930-155547-464')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrBuildEvidence=Join-Path $rrRoot $BuildEvidence
if((Get-Content (Join-Path $rrBuildEvidence 'build.exit')).Trim() -ne '0'){throw 'Require successful integration build'}
$rrEvidence=Join-Path $rrRoot 'evidence/47N'
New-Item -ItemType Directory -Path $rrEvidence -ErrorAction Stop | Out-Null
$rrGate=@{}
foreach($rrVariant in @('registry','memory')){
    $rrExe=Join-Path $rrRoot "work/native/ninja-build-release-opt/racer_$($rrVariant)47n_fixed_probe.exe"
    $rrFrozen=Join-Path $rrBuildEvidence "racer_$($rrVariant)47n_fixed_probe.exe"
    $rrHash=(Get-FileHash -LiteralPath $rrFrozen).Hash
    if((Get-FileHash -LiteralPath $rrExe).Hash -ne $rrHash){throw 'Build output changed'}
    $rrGate[$rrVariant]=@{binary_sha256=$rrHash.ToLowerInvariant();build_evidence=$BuildEvidence;scope='Build and original-component provenance verified; game validation is separate'}
}
$rrGate | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $rrEvidence 'build-gate.json')
$rrRuns=@()
foreach($rrVariant in @('registry','memory')){
    & (Join-Path $PSScriptRoot 'Run-Native47N.ps1') -MemoryVariant $rrVariant -RawMode 1 -Seconds 180 -Vblank 500 -QuietTrace | Tee-Object (Join-Path $rrEvidence "$rrVariant-startup.log")
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47n-$rrVariant-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    $rrRuns+=@{variant=$rrVariant;run=$rrRun.FullName}
    $rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'startup-runs.json')
    $rrReference=if($rrVariant -eq 'registry'){'work/native-run47l-1-1-1-1-20260930-151558-369'}else{'work/native-run47m-1-1-1-1-20260930-154627-034'}
    & 'C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot $rrReference) $rrRun.FullName --output (Join-Path $rrEvidence "$rrVariant-startup-state.json") *> (Join-Path $rrEvidence "$rrVariant-state.log")
    $rrExit=$LASTEXITCODE;$rrExit | Set-Content (Join-Path $rrEvidence "$rrVariant-state.exit")
    if($rrExit -ne 0){throw "$rrVariant startup state differs"}
}
0 | Set-Content (Join-Path $rrEvidence 'startup.exit')
Write-Output 'Both registry integrations match their accepted native startup references.'
