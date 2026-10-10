$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot 'evidence/47U';$rrGate=Join-Path $rrEvidence 'build-gate.json'
while(-not (Test-Path $rrGate)){Start-Sleep -Seconds 5}
$rrBuildGate=Get-Content $rrGate -Raw | ConvertFrom-Json
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@()
try {
    foreach($rrStop in @(500,7000)){
        & (Join-Path $PSScriptRoot 'Run-Native47U.ps1') -Seconds 3600 -Vblank $rrStop -DiagnosticLines 1 -EventGate 1 -DiagnosticCache 1 -DisableGsProfile -QuietTrace | Tee-Object (Join-Path $rrEvidence "$rrStop-run.log")
        $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter 'native-run47u-1-1-1-0-1-1-1-1-*' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        $rrRuns+=@{stop=$rrStop;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'runs.json')
        $rrReference=if($rrStop -eq 500){Join-Path $rrRoot 'work/native-run47t-1-1-1-0-1-1-1-1-20260930-172941-446'}else{Join-Path $rrRoot 'work/native-run47q-1-0-1-1-1-1-20260930-170018-199'}
        & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrReference $rrRun.FullName --output (Join-Path $rrEvidence "$rrStop-state.json") *> (Join-Path $rrEvidence "$rrStop-state.log")
        $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "$rrStop-state.exit")
        if($rrCode -ne 0){throw 'Instrumented native state differs'}
        & $rrPython (Join-Path $PSScriptRoot 'analyze_env_attribution47u.py') $rrRun.FullName --map (Join-Path $rrBuildGate.build_evidence 'native.map') --binary (Join-Path $rrBuildGate.build_evidence 'racer_env47u_fixed_probe.exe') --output (Join-Path $rrEvidence "$rrStop-attribution.json") *> (Join-Path $rrEvidence "$rrStop-attribution.log")
        if($LASTEXITCODE -ne 0){throw 'Attribution analysis failed'}
    }
    0 | Set-Content (Join-Path $rrEvidence 'native.exit')
} catch {$_ | Out-String | Set-Content (Join-Path $rrEvidence 'native-error.log');1 | Set-Content (Join-Path $rrEvidence 'native.exit');throw}
