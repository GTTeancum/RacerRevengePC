$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot 'evidence/47L'
New-Item -ItemType Directory -Path $rrEvidence -ErrorAction Stop | Out-Null
$rrCommands=Get-Content (Join-Path $rrRoot 'evidence/47J2/build.cmd') -First 4
$rrCommands += '"'+$rrRoot+'\work\build-tools\cmake\data\bin\cmake.exe" -S "'+$rrRoot+'\work\native\sources\src\PS2Recomp" -B "'+$rrRoot+'\work\native\ninja-build-release-opt" -DCMAKE_PROJECT_PS2RetroX_INCLUDE="'+($rrRoot -replace '\\','/')+'/tools/LegacyHook47L.cmake" -DRR_SHARED_ROOT="'+($rrRoot -replace '\\','/')+'"'
$rrCommands += 'if errorlevel 1 exit /b %errorlevel%'
$rrCommands += '"'+$rrRoot+'\work\build-tools\cmake\data\bin\cmake.exe" --build "'+$rrRoot+'\work\native\ninja-build-release-opt" --target racer_gs47l_fixed_probe racer_gs47l_original_check racer_gs47l_candidate_check --parallel 2'
$rrCommands += 'exit /b %errorlevel%'
$rrBatch=Join-Path $rrEvidence 'build.cmd'
$rrCommands | Set-Content -LiteralPath $rrBatch -Encoding ascii
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'build-start.txt')
& $env:ComSpec /d /c $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrExit=$LASTEXITCODE
$rrExit | Set-Content (Join-Path $rrEvidence 'build.exit')
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'build-end.txt')
if($rrExit -ne 0){Get-Content (Join-Path $rrEvidence 'build.log') -Tail 15;throw 'GS47L build failed'}
Get-ChildItem (Join-Path $rrRoot 'work/native/ninja-build-release-opt') -Filter 'racer_gs47l_*.exe' | Get-FileHash -Algorithm SHA256 | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'build-hashes.json')
Write-Output "GS47L candidate built; differential and game tests pending."
