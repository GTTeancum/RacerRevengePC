$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot 'evidence/47L/startup'
New-Item -ItemType Directory -Path $rrEvidence -ErrorAction Stop | Out-Null
if((Get-Content (Join-Path $rrRoot 'evidence/47L/differential-v3.exit')).Trim() -ne '0'){throw 'Require GS differential pass'}
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrReference=Join-Path $rrRoot 'work/native-run47j2-1-1-1-20260930-143813-579'
$rrRuns=@()
foreach($rrMode in @('0','1','2')){
    & (Join-Path $PSScriptRoot 'Run-Native47L.ps1') -RawMode $rrMode -Vblank 500 -Seconds 180 -QuietTrace
    $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47l-$rrMode-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if(-not $rrRun){throw 'Expected mode-specific run is missing'}
    & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrReference $rrRun.FullName --output (Join-Path $rrEvidence "mode-$rrMode.json") *> (Join-Path $rrEvidence "mode-$rrMode.log")
    $rrExit=$LASTEXITCODE
    $rrExit | Set-Content (Join-Path $rrEvidence "mode-$rrMode.exit")
    if($rrExit -ne 0){throw "Startup state differs in mode $rrMode"}
    $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName;reference=$rrReference}
    $rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'runs.json')
    Write-Output "GS47L mode $rrMode matches accepted 47J2 at VBlank 500."
}
'0' | Set-Content (Join-Path $rrEvidence 'all.exit')
