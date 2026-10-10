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
$rrEvidence = Join-Path $rrRoot ('evidence\nativePc48B-build-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrTargets = if ($Architecture -eq 'both') { @('x64','x86') } else { @($Architecture) }
foreach ($rrArch in $rrTargets) {
    $rrSources = @('CMakeLists.txt','cmake/NativePc48B.cmake','src/assets/native_asset48b.h',
        'src/assets/native_asset48b.cpp','src/game/control_filter48b.h','src/game/control_filter48b.cpp',
        'platform/windows/native/main48b.cpp','tests/native/asset_loader48b.cpp',
        'tests/native/control_filter48b.cpp','tools/gameplay/control_filter_words48b.h') |
        ForEach-Object { Join-Path $rrRoot $_ }
    $rrSourceHashes = Get-FileHash -LiteralPath $rrSources -Algorithm SHA256
    $rrCompiler = Join-Path $rrVc "bin\HostX64\$rrArch\cl.exe"
    if (!(Test-Path -LiteralPath $rrCompiler)) { throw "Missing compiler: $rrCompiler" }
    $rrBuild = Join-Path $rrRoot "build\native-pc48B-$rrArch"
    $rrBatch = Join-Path $rrEvidence "build-$rrArch.cmd"
    $rrCommands = @"
@echo off
set "PATH=$rrVc\bin\HostX64\$rrArch;$rrSdk\bin\$SdkVersion\x64;%PATH%"
set "INCLUDE=$rrVc\include;$rrSdk\Include\$SdkVersion\ucrt;$rrSdk\Include\$SdkVersion\shared;$rrSdk\Include\$SdkVersion\um;$rrSdk\Include\$SdkVersion\winrt"
set "LIB=$rrVc\lib\$rrArch;$rrSdk\Lib\$SdkVersion\ucrt\$rrArch;$rrSdk\Lib\$SdkVersion\um\$rrArch"
"$rrCmake" -S "$rrRoot" -B "$rrBuild" -G Ninja -DCMAKE_MAKE_PROGRAM="$rrNinja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DRR_NATIVE_PC_ONLY=ON
if errorlevel 1 exit /b %errorlevel%
"$rrCmake" --build "$rrBuild" --parallel 2
if errorlevel 1 exit /b %errorlevel%
cd /d "$rrBuild"
"$rrBuild\racer_native_asset_check.exe"
if errorlevel 1 exit /b %errorlevel%
"$rrBuild\racer_native_control_check.exe"
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
        if ((Get-FileHash -LiteralPath $rrHash.Path -Algorithm SHA256).Hash -ne $rrHash.Hash) {
            throw "Source changed during build: $($rrHash.Path)"
        }
    }
    $rrSourceHashes | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch-source-hashes.json")
    Copy-Item -LiteralPath "$rrBuild\compile_commands.json" -Destination "$rrEvidence\$rrArch-compile-commands.json"
    Copy-Item -LiteralPath "$rrBuild\racer_native_pc.map" -Destination "$rrEvidence\$rrArch-native.map"
    Get-FileHash -LiteralPath "$rrBuild\racer_native_pc.exe","$rrBuild\racer_native_asset_check.exe","$rrBuild\racer_native_control_check.exe" -Algorithm SHA256 |
        ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch-hashes.json")
}
@{scope='independent native asset viewer, not gameplay';architecture=$Architecture;compiler=$VcVersion;sdk=$SdkVersion;evidence=$rrEvidence} |
    ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence 'summary.json')
Write-Output "Native PC builds and asset checks passed. Evidence: $rrEvidence"
