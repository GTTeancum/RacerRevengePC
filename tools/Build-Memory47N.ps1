param([string]$AfterRunWindow='')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/memory47N-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
if($AfterRunWindow){
    $rrWaitRun=(Resolve-Path -LiteralPath $AfterRunWindow).Path
    Write-Output 'Waiting for VBlank 6950 before registry integration build.'
    while($true){
        $rrFrame=Get-ChildItem -LiteralPath $rrWaitRun -Filter 'frame-*.json' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        if($rrFrame){
            try {$rrState=Get-Content -LiteralPath $rrFrame.FullName -Raw | ConvertFrom-Json} catch {Start-Sleep -Seconds 2;continue}
            if($rrState.vsync -ge 6950){break}
        }
        if(Test-Path (Join-Path $rrWaitRun 'native.exit')){throw 'Run stopped before required capture'}
        Start-Sleep -Seconds 2
    }
    @{frame=$rrFrame.FullName;vsync=$rrState.vsync;utc=[DateTime]::UtcNow.ToString('o')} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'timing-window-gate.json')
}
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_memory47n.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Registry preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47N').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_registry47n_fixed_probe racer_memory47n_fixed_probe')
$rrBatch=Join-Path $rrEvidence 'build.cmd';$rrCommand | Set-Content -LiteralPath $rrBatch -Encoding ascii
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrExit=$LASTEXITCODE;$rrExit | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrExit -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 20;throw "Registry integration failed: $rrEvidence"}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
$rrBins=@((Join-Path $rrBuild 'racer_registry47n_fixed_probe.exe'),(Join-Path $rrBuild 'racer_memory47n_fixed_probe.exe'))
Copy-Item -LiteralPath $rrBins -Destination $rrEvidence
foreach($rrVariant in @('registry','memory')){
    $rrMap=Join-Path $rrRoot "build/memory47N/$rrVariant.map"
    if(Select-String -LiteralPath $rrMap -SimpleMatch '?g_ps2RecompiledFunctionTable@@'){throw 'Dense registry remains linked'}
    Copy-Item -LiteralPath $rrMap -Destination $rrEvidence
}
Get-FileHash -LiteralPath $rrBins | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'hashes.json')
& $rrPython (Join-Path $PSScriptRoot 'inspect_images.py') --output (Join-Path $rrEvidence 'images.json') (Join-Path $rrBuild 'racer_gs47l_fixed_probe.exe') (Join-Path $rrBuild 'racer_leaf47m_fixed_probe.exe') @rrBins *> (Join-Path $rrEvidence 'images.log')
if($LASTEXITCODE -ne 0){throw 'Image analysis failed'}
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
Write-Output "Registry integration built, dense table absent in both maps; game checks pending: $rrEvidence"
