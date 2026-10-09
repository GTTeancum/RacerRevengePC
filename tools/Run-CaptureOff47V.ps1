$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
# Serialize native pilots; do not compete with the combined timing pair or U.
$rrAfter=Join-Path $rrRoot 'evidence/47U/native.exit'
while(-not (Test-Path $rrAfter)){Start-Sleep -Seconds 5}
if((Get-Content $rrAfter).Trim() -ne '0'){throw 'Attribution validation must complete first'}
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_capture_off47v.py')
if($LASTEXITCODE -ne 0){throw 'Capture-off runner preparation failed'}
$rrEvidence=Join-Path $rrRoot ('evidence/captureOff47V-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrRace=Join-Path $rrRoot 'evidence/combined-race47T-20260930-173405-793'
$rrExe=Join-Path $rrRace 'bin/racer_combined47t_fixed_probe.exe'
$rrRuns=@();$rrReference=Join-Path $rrRoot 'work/native-run47q-1-0-1-1-1-1-20260930-170018-199'
try {
    foreach($rrStop in @(500,7000)){
        & (Join-Path $PSScriptRoot 'Run-Native47V.ps1') -Seconds 2400 -Vblank $rrStop -DiagnosticLines 1 -EventGate 1 -DiagnosticCache 1 -DisableGsProfile -DisableVuCapture -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence "$rrStop-run.log")
        $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47v-1-1-1-1-0-1-1-1-1-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        $rrRuns+=@{stop=$rrStop;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'runs.json')
        if(Test-Path (Join-Path $rrRun.FullName 'vu')){throw 'Unexpected VU capture output in disabled mode'}
        $rrRef=if($rrStop -eq 500){Join-Path $rrRoot 'work/native-run47t-1-1-1-0-1-1-1-1-20260930-172941-446'}else{$rrReference}
        & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrRef $rrRun.FullName --output (Join-Path $rrEvidence "$rrStop-state.json") *> (Join-Path $rrEvidence "$rrStop-state.log")
        $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrStop-state.exit")
        if($rrCode -ne 0){throw 'Capture-off native state differs'}
    }
    $rrControl=(Get-Content (Join-Path $rrRace 'runs.json') -Raw | ConvertFrom-Json | Where-Object mode -eq '1').run
    & $rrPython (Join-Path $PSScriptRoot 'analyze_fixed_pair47i.py') --baseline $rrControl --candidate $rrRun.FullName --expected-sha256 ((Get-FileHash $rrExe).Hash) --output (Join-Path $rrEvidence 'timing.json') *> (Join-Path $rrEvidence 'timing.log')
    if($LASTEXITCODE -ne 0){throw 'Capture-off timing analysis failed'}
    0 | Set-Content (Join-Path $rrEvidence 'native.exit')
} catch {$_ | Out-String | Set-Content (Join-Path $rrEvidence 'error.log');1 | Set-Content (Join-Path $rrEvidence 'native.exit');throw}
