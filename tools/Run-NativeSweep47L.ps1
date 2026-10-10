param([ValidateRange(1,7200)][int]$Seconds=2400,[ValidateSet(7000)][int]$Vblank=7000)
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
if((Get-Content (Join-Path $rrRoot 'evidence/47L/startup/all.exit')).Trim() -ne '0'){throw 'Require all startup state comparisons'}
$rrSweep=Join-Path $rrRoot ('evidence/native-sweep47L-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrBin=Join-Path $rrSweep 'bin'
New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrExe=Join-Path $rrBin 'racer_gs47l_fixed_probe.exe'
Copy-Item -LiteralPath (Join-Path $rrBuild 'racer_gs47l_fixed_probe.exe') -Destination $rrExe
Get-ChildItem -LiteralPath $rrBuild -Filter '*.dll' -File | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $rrBin $_.Name)}
$rrHash=(Get-FileHash -LiteralPath $rrExe).Hash
@{seconds_limit_each=$Seconds;vblank=$Vblank;utc=[DateTime]::UtcNow.ToString('o');order=@('0-original','1-batch-elision','2-batch-elision-reduced-history');binary=$rrExe;binary_sha256=$rrHash;host_profile=$false;comparison='Same frozen executable; mode 1 retains all history; mode 2 also reduces capture diagnostics. Report those effects separately.';acceptance='pending exact state and controlled race timing comparison'} | ConvertTo-Json | Set-Content (Join-Path $rrSweep 'sweep.json')
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@();$rrBaseline=''
try {
    foreach($rrMode in @('0','1','2')){
        if((Get-FileHash -LiteralPath $rrExe).Hash -cne $rrHash){throw 'Frozen executable changed'}
        & (Join-Path $PSScriptRoot 'Run-Native47L.ps1') -RawMode $rrMode -Seconds $Seconds -Vblank $Vblank -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrSweep "$rrMode.log")
        $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47l-$rrMode-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        if(-not $rrBaseline){$rrBaseline=$rrRun.FullName}
        $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName}
        $rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrSweep 'runs.json')
        $rrStateReference=if($rrMode -eq '0'){Join-Path $rrRoot 'work/native-run47j2-1-1-1-20260930-144756-248'}else{$rrBaseline}
        & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') $rrStateReference $rrRun.FullName --output (Join-Path $rrSweep "state-$rrMode.json") *> (Join-Path $rrSweep "state-$rrMode.log")
        if($LASTEXITCODE -ne 0){throw "Race state diverged in mode $rrMode"}
        if($rrMode -ne '0'){
            & $rrPython (Join-Path $PSScriptRoot 'analyze_fixed_pair47i.py') --baseline $rrBaseline --candidate $rrRun.FullName --expected-sha256 $rrHash --output (Join-Path $rrSweep "timing-0-$rrMode.json") *> (Join-Path $rrSweep "timing-0-$rrMode.log")
            if($LASTEXITCODE -ne 0){throw 'Timing report failed'}
        }
    }
    & $rrPython (Join-Path $PSScriptRoot 'analyze_fixed_pair47i.py') --baseline $rrRuns[1].run --candidate $rrRuns[2].run --expected-sha256 $rrHash --output (Join-Path $rrSweep 'timing-1-2.json') *> (Join-Path $rrSweep 'timing-1-2.log')
    if($LASTEXITCODE -ne 0){throw 'Diagnostic-overhead timing report failed'}
    0 | Set-Content (Join-Path $rrSweep 'sweep.exit')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrSweep 'error.log')
    1 | Set-Content (Join-Path $rrSweep 'sweep.exit')
    throw
}
