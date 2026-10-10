param([string]$AfterNativeRun='')
$ErrorActionPreference='Stop'
$rrRoot=Split-Path -Parent $PSScriptRoot
if($AfterNativeRun){
    $rrWaitRun=(Resolve-Path -LiteralPath $AfterNativeRun).Path
    Write-Output "Waiting for the profiling run to close before starting compiler work."
    while(-not (Test-Path -LiteralPath (Join-Path $rrWaitRun 'native.exit'))){Start-Sleep -Seconds 2}
    if((Get-Content (Join-Path $rrWaitRun 'native.exit')).Trim() -ne '0'){throw 'Profiling run did not exit successfully; inspect it before building'}
}
$rrEvidence=Join-Path $rrRoot ('evidence/scalar47M-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'start.txt')
$rrOldMsystem=$env:MSYSTEM
$rrOldRoot=$env:RR_PROJECT_ROOT
try {
    $env:MSYSTEM='MINGW64'; $env:RR_PROJECT_ROOT=$rrRoot
    & (Join-Path $rrRoot 'toolchains/msys64/usr/bin/bash.exe') --login -c 'set -e; rrroot=$(cygpath -u "$RR_PROJECT_ROOT"); export NXDK_DIR="$rrroot/toolchains/nxdk"; export PATH="$NXDK_DIR/bin:$PATH"; cd "$rrroot/platform/xbox-scalar"; make -j2' *> (Join-Path $rrEvidence 'xbox.log')
    $rrExit=$LASTEXITCODE
    $rrExit | Set-Content (Join-Path $rrEvidence 'xbox.exit')
    if($rrExit -ne 0){throw "scalar/leaf Xbox compilation failed: $rrEvidence"}
} finally {$env:MSYSTEM=$rrOldMsystem; $env:RR_PROJECT_ROOT=$rrOldRoot}
$rrCmake=Join-Path $rrRoot 'work/build-tools/cmake/data/bin/cmake.exe'
$rrNinja=Join-Path $rrRoot 'work/build-tools/bin/ninja.exe'
$rrVc='C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.44.35207'
$rrSdk='C:\Program Files (x86)\Windows Kits\10'
$rrSdkVersion='10.0.26100.0'
foreach($rrArch in @('x86','x64')){
    $rrBuild=Join-Path $rrRoot "build/scalar47M-$rrArch"
    $rrXboxOption=if($rrArch -eq 'x86'){'ON'}else{'OFF'}
    $rrXboxTarget=if($rrArch -eq 'x86'){'racer_scalar47m_xbox_objects_check'}else{''}
    $rrCommand=@"
@echo off
set "PATH=$rrVc\bin\HostX64\$rrArch;$rrSdk\bin\$rrSdkVersion\x64;%PATH%"
set "INCLUDE=$rrVc\include;$rrSdk\Include\$rrSdkVersion\ucrt;$rrSdk\Include\$rrSdkVersion\shared;$rrSdk\Include\$rrSdkVersion\um"
set "LIB=$rrVc\lib\$rrArch;$rrSdk\Lib\$rrSdkVersion\ucrt\$rrArch;$rrSdk\Lib\$rrSdkVersion\um\$rrArch"
"$rrCmake" -S "$rrRoot" -B "$rrBuild" -G Ninja -DCMAKE_MAKE_PROGRAM="$rrNinja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="$rrVc\bin\HostX64\$rrArch\cl.exe" -DRR_CHECK_SCALAR_XBOX_OBJECTS=$rrXboxOption
if errorlevel 1 exit /b %errorlevel%
"$rrCmake" --build "$rrBuild" --target racer_scalar47m_check $rrXboxTarget --parallel 2
if errorlevel 1 exit /b %errorlevel%
"$rrBuild\racer_scalar47m_check.exe"
if errorlevel 1 exit /b %errorlevel%
if "$rrXboxOption"=="ON" "$rrBuild\racer_scalar47m_xbox_objects_check.exe"
exit /b %errorlevel%
"@
    $rrBatch=Join-Path $rrEvidence "$rrArch.cmd"
    $rrCommand | Set-Content -LiteralPath $rrBatch -Encoding ascii
    & $rrBatch *> (Join-Path $rrEvidence "$rrArch.log")
    $rrExit=$LASTEXITCODE
    $rrExit | Set-Content (Join-Path $rrEvidence "$rrArch.exit")
    if($rrExit -ne 0){throw "scalar/leaf $rrArch build/check failed: $rrEvidence"}
}
Get-FileHash -LiteralPath (Join-Path $rrRoot 'src/core/scalar47m.obj'),(Join-Path $rrRoot 'tests/scalar_reference47m.obj'),(Join-Path $rrRoot 'tests/scalar_suite47m.obj'),(Join-Path $rrRoot 'build/xbox-scalar47M/default.xbe'),(Join-Path $rrRoot 'build/scalar47M-x86/racer_scalar47m_check.exe'),(Join-Path $rrRoot 'build/scalar47M-x64/racer_scalar47m_check.exe'),(Join-Path $rrRoot 'build/scalar47M-x86/racer_scalar47m_xbox_objects_check.exe') | ConvertTo-Json | Set-Content (Join-Path $rrEvidence 'hashes.json')
[DateTime]::UtcNow.ToString('o') | Set-Content (Join-Path $rrEvidence 'end.txt')
Write-Output "scalar/leaf Windows x86/x64 and exact nxdk-object checks passed. XBE execution remains unverified. Evidence: $rrEvidence"
