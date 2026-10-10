$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrBaseline=Join-Path $rrRoot 'work/native-run47c-baseline-20260930-124035-421/native.exit'
if(!(Test-Path -LiteralPath $rrBaseline) -or (Get-Content $rrBaseline).Trim() -ne '0') {
    throw 'Wait for the baseline race timing window to finish before compiling this candidate.'
}
$rrEvidence=Join-Path $rrRoot 'evidence/47E'
New-Item -ItemType Directory -Path $rrEvidence -Force | Out-Null
if(Test-Path (Join-Path $rrEvidence 'build.exit')) { throw 'Preserve existing build evidence; use a new attempt script/log name.' }
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'build-overlap-start.utc')
$rrBatch=Join-Path $rrEvidence 'build.cmd'
$rrCommands=Get-Content (Join-Path $rrRoot 'evidence/47B/build-native.cmd') -First 4
$rrCommands += '"'+$rrRoot+'\work\build-tools\cmake\data\bin\cmake.exe" -S "'+$rrRoot+'\work\native\sources\src\PS2Recomp" -B "'+$rrRoot+'\work\native\ninja-build-release-opt" -DCMAKE_PROJECT_PS2RetroX_INCLUDE="'+($rrRoot -replace '\\','/')+'/tools/LegacyHook47E.cmake" -DRR_SHARED_ROOT="'+($rrRoot -replace '\\','/')+'"'
$rrCommands += 'if errorlevel 1 exit /b %errorlevel%'
$rrCommands += '"'+$rrRoot+'\work\build-tools\cmake\data\bin\cmake.exe" --build "'+$rrRoot+'\work\native\ninja-build-release-opt" --target racer_compact47e_contract racer_compact47e_probe --parallel 2'
$rrCommands += 'exit /b %errorlevel%'
$rrCommands | Set-Content $rrBatch -Encoding ascii
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrCode=$LASTEXITCODE
$rrCode | Set-Content (Join-Path $rrEvidence 'build.exit')
if($rrCode -ne 0) { Get-Content (Join-Path $rrEvidence 'build.log') -Tail 15; throw "Compact build failed: $rrCode" }
$rrBefore=Get-Content (Join-Path $rrEvidence 'all-generated-objects.txt')
$rrAfter=Get-Content (Join-Path $rrEvidence 'compact-generated-objects.txt')
$rrOmitted=@(Compare-Object $rrBefore $rrAfter | Where-Object SideIndicator -eq '<=' | ForEach-Object InputObject)
if($rrOmitted.Count -ne 3 -or ($rrOmitted | Where-Object {$_ -notmatch '(batch_055[.]cpp|recovered(10|14)[/\\]register[.]cpp)[.]obj$'})) { throw 'Unexpected instruction object exclusion' }
$rrOmitted | Set-Content (Join-Path $rrEvidence 'omitted-initializers.txt')
if(Select-String -LiteralPath (Join-Path $rrEvidence 'compact-probe.map') -SimpleMatch '?g_ps2RecompiledFunctionTable@@') { throw 'Dense pointer table is still linked' }
& (Join-Path $rrRoot 'work/native/ninja-build-release-opt/racer_compact47e_contract.exe') *> (Join-Path $rrEvidence 'contract.log')
$rrCode=$LASTEXITCODE
$rrCode | Set-Content (Join-Path $rrEvidence 'contract.exit')
Get-Content (Join-Path $rrEvidence 'contract.log') -Tail 8
if($rrCode -ne 0) { throw "Compact replacement contract failed: $rrCode" }
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'build-overlap-end.utc')
Get-FileHash (Join-Path $rrRoot 'work/native/ninja-build-release-opt/racer_native_probe.exe'),(Join-Path $rrRoot 'work/native/ninja-build-release-opt/racer_compact47e_probe.exe') | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'binary-hashes.json')
Write-Output "Compact native candidate and replacement contract built. Evidence: $rrEvidence"
