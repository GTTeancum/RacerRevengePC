param(
    [ValidateSet('x86','x64','both')][string]$Architecture = 'both',
    [string]$VisualStudioRoot = 'C:\Program Files\Microsoft Visual Studio\18\Community',
    [string]$VcVersion = '14.44.35207',
    [string]$SdkVersion = '10.0.26100.0'
)
$ErrorActionPreference = 'Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrCmake = Join-Path $rrRoot 'work\build-tools\cmake\data\bin\cmake.exe'
$rrNinja = Join-Path $rrRoot 'work\build-tools\bin\ninja.exe'
$rrVc = Join-Path $VisualStudioRoot "VC\Tools\MSVC\$VcVersion"
$rrSdk = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10'
$rrEvidence = Join-Path $rrRoot ('evidence\nativePc48F-build-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrTargets = if ($Architecture -eq 'both') { @('x64','x86') } else { @($Architecture) }
foreach ($rrArch in $rrTargets) {
    $rrSources = @('CMakeLists.txt','cmake/NativePc48B.cmake','cmake/NativePc48C.cmake',
        'src/assets/native_asset48b.h','src/assets/native_asset48b.cpp',
        'src/assets/import_decode48c.h','src/assets/import_decode48c.cpp',
        'src/game/control_filter48b.h','src/game/control_filter48b.cpp',
        'src/game/body_integration48c.h','src/game/body_integration48c.cpp',
        'platform/windows/native/main48b.cpp','platform/windows/native/import_service48c.h',
        'platform/windows/native/import_service48c.cpp','platform/windows/native/import_ui48c.h',
        'platform/windows/native/import_ui48c.cpp','tests/native/asset_loader48b.cpp',
        'tests/native/control_filter48b.cpp','tests/native/import_decode48c.cpp',
        'tools/gameplay/control_filter_words48b.h','tests/native/physics48c.cpp',
        'tools/gameplay/physics_words48c.h',
        'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h') | ForEach-Object { Join-Path $rrRoot $_ }
    $rrSources += Get-ChildItem -LiteralPath "$rrRoot\third_party\native-zlib","$rrRoot\third_party\native-json" -File | ForEach-Object FullName
    $rrSources += Get-ChildItem -Path "$rrRoot\src\assets\*48d.*","$rrRoot\src\game\*48d.*","$rrRoot\tests\native\*48d.*","$rrRoot\tools\gameplay\*48d.h" -File | ForEach-Object FullName
    $rrSources += Join-Path $rrRoot 'cmake/NativePc48D.cmake'
    $rrSources += Get-ChildItem -Path "$rrRoot\src\assets\*48e.*","$rrRoot\src\game\*48e.*","$rrRoot\tests\native\*48e.*","$rrRoot\tools\gameplay\*48e.h","$rrRoot\platform\windows\native\*48e.*" -File | ForEach-Object FullName
    $rrSources += Join-Path $rrRoot 'cmake/NativePc48E.cmake'
    $rrSources += Get-ChildItem -Path "$rrRoot\src\assets\*48f.*","$rrRoot\tests\native\*48f.*","$rrRoot\platform\windows\native\*48f.*" -File | ForEach-Object FullName
    $rrSources += Join-Path $rrRoot 'cmake/NativePc48F.cmake'
    $rrSources += Join-Path $rrRoot 'cmake/NativeWorld48F.cmake'
    $rrSourceHashes = Get-FileHash -LiteralPath $rrSources -Algorithm SHA256
    $rrCompiler = Join-Path $rrVc "bin\HostX64\$rrArch\cl.exe"
    if (!(Test-Path -LiteralPath $rrCompiler)) { throw "Missing compiler: $rrCompiler" }
    $rrBuild = Join-Path $rrRoot "build\native-pc48F-$rrArch"
    $rrBatch = Join-Path $rrEvidence "build-$rrArch.cmd"
    $rrCommands = @"
@echo off
set "PATH=$rrVc\bin\HostX64\$rrArch;$rrSdk\bin\$SdkVersion\x64;%PATH%"
set "INCLUDE=$rrVc\include;$rrSdk\Include\$SdkVersion\ucrt;$rrSdk\Include\$SdkVersion\shared;$rrSdk\Include\$SdkVersion\um;$rrSdk\Include\$SdkVersion\winrt"
set "LIB=$rrVc\lib\$rrArch;$rrSdk\Lib\$SdkVersion\ucrt\$rrArch;$rrSdk\Lib\$SdkVersion\um\$rrArch"
"$rrCmake" -S "$rrRoot" -B "$rrBuild" -G Ninja -DCMAKE_MAKE_PROGRAM="$rrNinja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl -DCMAKE_C_COMPILER=cl -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DRR_NATIVE_PC_ONLY=ON
if errorlevel 1 exit /b %errorlevel%
"$rrCmake" --build "$rrBuild" --parallel 2
if errorlevel 1 exit /b %errorlevel%
cd /d "$rrBuild"
"$rrCmake" -E env -- "${rrRoot}\work\build-tools\cmake\data\bin\ctest.exe" --output-on-failure
if errorlevel 1 exit /b %errorlevel%
"$rrVc\bin\HostX64\$rrArch\dumpbin.exe" /headers /imports "$rrBuild\racer_native_pc.exe" > "$rrEvidence\$rrArch-pe.txt"
exit /b %errorlevel%
"@
    Set-Content -LiteralPath $rrBatch -Value ($rrCommands -replace "`r?`n", "`r`n") -Encoding ascii
    & $env:ComSpec /d /c $rrBatch 2>&1 | Tee-Object -FilePath (Join-Path $rrEvidence "$rrArch.log")
    $rrExit = $LASTEXITCODE
    Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch.exit") -Value $rrExit
    if ($rrExit -ne 0) { throw "Native PC $rrArch build/check failed ($rrExit); logs: $rrEvidence" }
    foreach ($rrHash in $rrSourceHashes) {
        if ((Get-FileHash -LiteralPath $rrHash.Path -Algorithm SHA256).Hash -ne $rrHash.Hash) { throw "Source changed during build: $($rrHash.Path)" }
    }
    $rrSourceHashes | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch-source-hashes.json")
    Copy-Item -LiteralPath "$rrBuild\compile_commands.json" -Destination "$rrEvidence\$rrArch-compile-commands.json"
    Copy-Item -LiteralPath "$rrBuild\racer_native_pc.map" -Destination "$rrEvidence\$rrArch-native.map"
    Copy-Item -LiteralPath "$rrBuild\Testing\Temporary\LastTest.log" -Destination "$rrEvidence\$rrArch-tests.log"
    Copy-Item -LiteralPath "$rrBuild\texture-report48e.json" -Destination "$rrEvidence\$rrArch-textures.json"
    Copy-Item -LiteralPath "$rrBuild\world-report48f.json" -Destination "$rrEvidence\$rrArch-worlds.json"
    Copy-Item -LiteralPath "$rrBuild\world-fixtures48f" -Destination "$rrEvidence\$rrArch-world-fixtures" -Recurse
    New-Item -ItemType Directory -Path "$rrEvidence\$rrArch" | Out-Null
    $rrBinaries = Get-ChildItem -LiteralPath $rrBuild -Filter 'racer_native_*.exe' -File
    foreach ($rrBinary in $rrBinaries) { Copy-Item -LiteralPath $rrBinary.FullName -Destination "$rrEvidence\$rrArch\$($rrBinary.Name)" }
    Get-FileHash -LiteralPath ($rrBinaries | ForEach-Object FullName) -Algorithm SHA256 |
        ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch-hashes.json")
}
@{scope='native viewer, built-in partial geometry importer and recovered game functions';architecture=$Architecture;compiler=$VcVersion;sdk=$SdkVersion;evidence=$rrEvidence} |
    ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence 'summary.json')
Write-Output "Native PC builds and checks passed. Evidence: $rrEvidence"
