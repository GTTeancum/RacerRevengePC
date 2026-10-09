$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
if((Get-Content (Join-Path $rrRoot 'evidence/47Q/startup.exit')).Trim() -ne '0'){throw 'Require cache off/on startup comparisons'}
$rrEvidence=Join-Path $rrRoot ('evidence/launchEnv-race47Q-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$rrBin=Join-Path $rrEvidence 'bin';New-Item -ItemType Directory -Path $rrBin | Out-Null
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt';$rrExe=Join-Path $rrBin 'racer_launch47q_fixed_probe.exe'
Copy-Item (Join-Path $rrBuild 'racer_launch47q_fixed_probe.exe') $rrExe
Get-ChildItem $rrBuild -Filter '*.dll' | ForEach-Object {Copy-Item $_.FullName $rrBin}
$rrHash=(Get-FileHash $rrExe).Hash
$rrGate=Get-Content (Join-Path $rrRoot 'evidence/47Q/build-gate.json') -Raw | ConvertFrom-Json
if($rrHash.ToLowerInvariant() -ne $rrGate.binary_sha256){throw 'Frozen candidate hash differs'}
@{utc=[DateTime]::UtcNow.ToString('o');binary_sha256=$rrHash;modes=@('0','1');vblank=7000;status='pending state/timing comparison';scope='Same binary; only IOP diagnostic environment lookup cache differs'} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'queued.json')
Write-Output "Cache race pair: $rrEvidence"
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$rrRuns=@()
try {
    foreach($rrMode in @('0','1')){
        & (Join-Path $PSScriptRoot 'Run-Native47Q.ps1') -DiagnosticCache $rrMode -DisableGsProfile -Seconds 2400 -Vblank 7000 -QuietTrace -BinarySource $rrExe | Tee-Object (Join-Path $rrEvidence "$rrMode.log")
        $rrRun=Get-ChildItem (Join-Path $rrRoot 'work') -Directory -Filter "native-run47q-$rrMode-0-1-1-1-1-*" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        $rrRuns+=@{mode=$rrMode;run=$rrRun.FullName};$rrRuns | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'runs.json')
        & $rrPython (Join-Path $PSScriptRoot 'compare_fixed47f.py') (Join-Path $rrRoot 'work/native-run47n-memory-1-1-1-1-20260930-160745-045') $rrRun.FullName --output (Join-Path $rrEvidence "state-$rrMode.json") *> (Join-Path $rrEvidence "state-$rrMode.log")
        $rrCode=$LASTEXITCODE;$rrCode | Set-Content (Join-Path $rrEvidence "state-$rrMode.exit")
        if($rrCode -ne 0){throw "Cache mode $rrMode changed full race state"}
    }
    & $rrPython (Join-Path $PSScriptRoot 'analyze_fixed_pair47i.py') --baseline $rrRuns[0].run --candidate $rrRuns[1].run --expected-sha256 $rrHash --output (Join-Path $rrEvidence 'timing.json') *> (Join-Path $rrEvidence 'timing.log')
    if($LASTEXITCODE -ne 0){throw 'Timing analysis failed'}
    0 | Set-Content (Join-Path $rrEvidence 'race.exit')
} catch {
    $_ | Out-String | Set-Content (Join-Path $rrEvidence 'error.log');1 | Set-Content (Join-Path $rrEvidence 'race.exit');throw
}
