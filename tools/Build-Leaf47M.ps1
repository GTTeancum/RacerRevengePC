param([string]$AfterRunWindow='')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/leaf47M-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
if($AfterRunWindow){
    $rrWaitRun=(Resolve-Path -LiteralPath $AfterRunWindow).Path
    Write-Output 'Waiting for VBlank 6950 before native compiler work.'
    while($true){
        $rrFrame=Get-ChildItem -LiteralPath $rrWaitRun -Filter 'frame-*.json' | Sort-Object LastWriteTime -Descending | Select-Object -First 1
        if($rrFrame){
            try {$rrState=Get-Content -LiteralPath $rrFrame.FullName -Raw | ConvertFrom-Json} catch {Start-Sleep -Seconds 2;continue}
            if($rrState.vsync -ge 6950){break}
        }
        if(Test-Path (Join-Path $rrWaitRun 'native.exit')){throw 'Run stopped before the required capture'}
        Start-Sleep -Seconds 2
    }
    @{frame=$rrFrame.FullName;vsync=$rrState.vsync;utc=[DateTime]::UtcNow.ToString('o')} | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'timing-window-gate.json')
}
$rrPython='C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $rrPython (Join-Path $PSScriptRoot 'prepare_leaf_native47m.py') *> (Join-Path $rrEvidence 'prepare.log')
if($LASTEXITCODE -ne 0){throw 'Leaf preparation failed'}
$rrCommand=(Get-Content (Join-Path $rrRoot 'evidence/47L/build.cmd') -Raw).Replace('LegacyHook47L','LegacyHook47M').Replace('racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check','racer_leaf47m_check racer_leaf47m_fixed_probe')
$rrBatch=Join-Path $rrEvidence 'build.cmd'
$rrCommand | Set-Content -LiteralPath $rrBatch -Encoding ascii
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrExit=$LASTEXITCODE;$rrExit | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrExit -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 20;throw "Leaf build failed: $rrEvidence"}
$rrBuild=Join-Path $rrRoot 'work/native/ninja-build-release-opt'
Copy-Item -LiteralPath (Join-Path $rrBuild 'racer_leaf47m_check.exe'),(Join-Path $rrBuild 'racer_leaf47m_fixed_probe.exe') -Destination $rrEvidence
& (Join-Path $rrBuild 'racer_leaf47m_check.exe') *> (Join-Path $rrEvidence 'check.log')
$rrExit=$LASTEXITCODE;$rrExit | Set-Content (Join-Path $rrEvidence 'check.exit')
Get-Content (Join-Path $rrEvidence 'check.log')
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
Get-FileHash -LiteralPath (Join-Path $rrBuild 'racer_leaf47m_check.exe'),(Join-Path $rrBuild 'racer_leaf47m_fixed_probe.exe') | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'hashes.json')
if($rrExit -ne 0){throw "Original-function differential failed: $rrEvidence"}
Write-Output "Leaf original-function differential passed: $rrEvidence"
