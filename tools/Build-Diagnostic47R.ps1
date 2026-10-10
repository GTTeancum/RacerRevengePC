param([string]$AfterRun='work/native-run47q-0-0-1-1-1-1-20260930-164842-716')
$ErrorActionPreference='Stop';$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/diagnostic47R-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
if($AfterRun){
    $rrRun=Join-Path $rrRoot $AfterRun
    while($true){
        $rrFrame=Get-ChildItem $rrRun -Filter 'frame-*.json' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        if($rrFrame){try {$rrState=Get-Content $rrFrame.FullName -Raw | ConvertFrom-Json} catch {Start-Sleep -Seconds 2;continue};if($rrState.vsync -ge 6950){break}}
        if(Test-Path (Join-Path $rrRun 'native.exit')){if((Get-Content (Join-Path $rrRun 'native.exit')).Trim() -ne '0'){throw 'Reference run failed'};break}
        Start-Sleep -Seconds 2
    }
    @{run=$rrRun;utc=[DateTime]::UtcNow.ToString('o');vsync=$rrState.vsync;frame=$rrFrame.Name} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'timing-window-gate.json')
}
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_diagnostic_lines47r.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Probe preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47R').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_diagnostic47r_fixed_probe racer_diagnostic47r_test')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 15;throw 'Diagnostic build failed'}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
Copy-Item (Join-Path $rrBuild 'racer_diagnostic47r_fixed_probe.exe') $rrEvidence
Copy-Item (Join-Path $rrBuild 'racer_diagnostic47r_test.exe') $rrEvidence
& (Join-Path $rrBuild 'racer_diagnostic47r_test.exe') *> (Join-Path $rrEvidence 'test.log')
$rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence 'test.exit')
if($rrCode -ne 0){throw 'Diagnostic byte-preservation checks failed'}
$rrGate=Join-Path $rrRoot 'evidence/47R';New-Item -ItemType Directory -Path $rrGate | Out-Null
@{binary_sha256=(Get-FileHash (Join-Path $rrEvidence 'racer_diagnostic47r_fixed_probe.exe')).Hash.ToLowerInvariant();build_evidence=$rrEvidence;scope='Built diagnostic candidate; byte-preservation checks passed; native validation pending'} | ConvertTo-Json | Set-Content (Join-Path $rrGate 'build-gate.json')
Write-Output $rrEvidence
