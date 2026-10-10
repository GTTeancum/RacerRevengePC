param([string]$BuildEvidence='evidence/gsProfile47O-20260930-160733-694')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrBuildEvidence=Join-Path $rrRoot $BuildEvidence
if((Get-Content (Join-Path $rrBuildEvidence 'build.exit')).Trim() -ne '0'){throw 'Require successful profiler build'}
$rrEvidence=Join-Path $rrRoot 'evidence/47O';New-Item -ItemType Directory -Path $rrEvidence -ErrorAction Stop | Out-Null
$rrFrozen=Join-Path $rrBuildEvidence 'racer_gsprofile47o_fixed_probe.exe'
$rrHash=(Get-FileHash -LiteralPath $rrFrozen).Hash.ToLowerInvariant()
@{binary_sha256=$rrHash;build_evidence=$BuildEvidence;scope='Built isolated GS observer; native game checks required'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'build-gate.json')
$rrRuns=@();$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
foreach($rrProfile in @(0,1)){
    $rrArguments=@{Seconds=180;Vblank=500;RawMode='1';QuietTrace=$true}
    if($rrProfile -eq 0){$rrArguments.DisableGsProfile=$true}
    & (Join-Path $PSScriptRoot 'Run-Native47O.ps1') @rrArguments | Tee-Object (Join-Path $rrEvidence "$rrProfile-startup.log")
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47o-$rrProfile-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    $rrRuns+=@{profile=$rrProfile;run=$rrRun.FullName}
    $rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'startup-runs.json')
    & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot 'work/native-run47n-memory-1-1-1-1-20260930-155829-832') $rrRun.FullName --output (Join-Path $rrEvidence "$rrProfile-startup-state.json") *> (Join-Path $rrEvidence "$rrProfile-state.log")
    $rrExit=$LASTEXITCODE;$rrExit | Set-Content (Join-Path $rrEvidence "$rrProfile-state.exit")
    if($rrExit -ne 0){throw 'Profiler changed saved startup state'}
    if($rrProfile -eq 0){if(Test-Path (Join-Path $rrRun.FullName 'gs-profile.csv')){throw 'Disabled profiler emitted a file'}}
    else {
        & $rrPython (Join-Path $PSScriptRoot 'analyze_gs_profile47o.py') $rrRun.FullName --output (Join-Path $rrEvidence 'startup-profile.json') *> (Join-Path $rrEvidence 'startup-profile.log')
        $rrExit=$LASTEXITCODE;$rrExit | Set-Content (Join-Path $rrEvidence 'startup-profile.exit')
        if($rrExit -ne 0){throw 'GS profile counters failed validation'}
        $rrProfileReport=Get-Content (Join-Path $rrEvidence 'startup-profile.json') -Raw | ConvertFrom-Json
        if(-not $rrProfileReport.terminal_vsync_reconciled){throw 'Backend VSync count differs from scheduler stop'}
    }
}
0 | Set-Content (Join-Path $rrEvidence 'startup.exit')
Write-Output 'Profiler off/on startup state matches; GS counters and scheduler VSync count reconcile.'
