$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
$rrEvidence=Join-Path $rrRoot ('evidence/iop-x86-47J2-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrCmake=Join-Path $rrRoot 'work/build-tools/cmake/data/bin/cmake.exe'
$rrNinja=Join-Path $rrRoot 'work/build-tools/bin/ninja.exe'
$rrVc='C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.44.35207'
$rrSdk='C:\Program Files (x86)\Windows Kits\10'
$rrSdkVersion='10.0.26100.0'
$rrBuild=Join-Path $rrRoot 'build/iop47J2-x86'
$rrCommand=@"
@echo off
set "PATH=$rrVc\bin\HostX64\x86;$rrSdk\bin\$rrSdkVersion\x64;%PATH%"
set "INCLUDE=$rrVc\include;$rrSdk\Include\$rrSdkVersion\ucrt;$rrSdk\Include\$rrSdkVersion\shared;$rrSdk\Include\$rrSdkVersion\um"
set "LIB=$rrVc\lib\x86;$rrSdk\Lib\$rrSdkVersion\ucrt\x86;$rrSdk\Lib\$rrSdkVersion\um\x86"
"$rrCmake" -S "$rrRoot" -B "$rrBuild" -G Ninja -DCMAKE_MAKE_PROGRAM="$rrNinja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="$rrVc\bin\HostX64\x86\cl.exe" -DRR_BUILD_IOP47J2_X86_CHECKS=ON
if errorlevel 1 exit /b %errorlevel%
"$rrCmake" --build "$rrBuild" --target racer_iop47j2_differential racer_iop47j2_scheduler racer_iop47j2_emulator racer_iop47j2_import racer_iop47j2_import_version racer_iop47j2_compatibility racer_iop47j2_import_cache racer_iop47j2_memory --parallel 2
exit /b %errorlevel%
"@
$rrBatch=Join-Path $rrEvidence 'build.cmd'
$rrCommand | Set-Content -LiteralPath $rrBatch -Encoding ascii
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
& $rrBatch *> (Join-Path $rrEvidence 'build.log')
$rrExit=$LASTEXITCODE
$rrExit | Set-Content (Join-Path $rrEvidence 'build.exit')
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
if($rrExit -ne 0){throw "IOP x86 build failed: $rrEvidence"}
Get-ChildItem -LiteralPath $rrBuild -Filter 'racer_iop47j2_*.exe' | Get-FileHash -Algorithm SHA256 | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'binary-hashes.json')
Write-Output "IOP x86 candidate built with /arch:IA32; tests and object ISA audit pending. Evidence: $rrEvidence"
