param(
    [ValidateSet('x86','x64','both')][string]$Architecture = 'both',
    [string]$VisualStudioRoot = 'C:\Program Files\Microsoft Visual Studio\18\Community',
    [string]$VcVersion = '14.44.35207',
    [string]$SdkVersion = '10.0.26100.0',
    [switch]$CheckXboxObjects,
    [switch]$CheckLegacyIop
)
$ErrorActionPreference = 'Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrCmake = Join-Path $rrRoot 'work\build-tools\cmake\data\bin\cmake.exe'
$rrNinja = Join-Path $rrRoot 'work\build-tools\bin\ninja.exe'
if (!(Test-Path -LiteralPath $rrCmake)) { $rrCmake = (Get-Command cmake.exe -ErrorAction Stop).Source }
if (!(Test-Path -LiteralPath $rrNinja)) { $rrNinja = (Get-Command ninja.exe -ErrorAction Stop).Source }
$rrVc = Join-Path $VisualStudioRoot "VC\Tools\MSVC\$VcVersion"
$rrSdk = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10'
$rrEvidence = Join-Path $rrRoot ('evidence\windows-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrTargets = if ($Architecture -eq 'both') { @('x86','x64') } else { @($Architecture) }
foreach ($rrArch in $rrTargets) {
    $rrCompiler = Join-Path $rrVc "bin\HostX64\$rrArch\cl.exe"
    if (!(Test-Path -LiteralPath $rrCompiler)) { throw "Missing compiler: $rrCompiler" }
    $rrBuild = Join-Path $rrRoot "build\windows-$rrArch"
    $rrBatch = Join-Path $rrEvidence "build-$rrArch.cmd"
    $rrXboxOption = if ($CheckXboxObjects -and $rrArch -eq 'x86') { 'ON' } else { 'OFF' }
    $rrIopOption = if ($CheckLegacyIop -and $rrArch -eq 'x64') { 'ON' } else { 'OFF' }
    $rrCommands = @"
@echo off
set "PATH=$rrVc\bin\HostX64\$rrArch;$rrSdk\bin\$SdkVersion\x64;%PATH%"
set "INCLUDE=$rrVc\include;$rrSdk\Include\$SdkVersion\ucrt;$rrSdk\Include\$SdkVersion\shared;$rrSdk\Include\$SdkVersion\um"
set "LIB=$rrVc\lib\$rrArch;$rrSdk\Lib\$SdkVersion\ucrt\$rrArch;$rrSdk\Lib\$SdkVersion\um\$rrArch"
"$rrCmake" -S "$rrRoot" -B "$rrBuild" -G Ninja -DCMAKE_MAKE_PROGRAM="$rrNinja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl -DRR_CHECK_XBOX_OBJECTS=$rrXboxOption -DRR_BUILD_LEGACY_IOP_CHECKS=$rrIopOption
if errorlevel 1 exit /b %errorlevel%
"$rrCmake" --build "$rrBuild"
if errorlevel 1 exit /b %errorlevel%
"$rrBuild\racer_core_check.exe"
if errorlevel 1 exit /b %errorlevel%
if "$rrXboxOption"=="ON" "$rrBuild\racer_xbox_objects_check.exe"
if errorlevel 1 exit /b %errorlevel%
if "$rrIopOption"=="ON" "$rrBuild\racer_legacy_iop_reference_check.exe" reference
if errorlevel 1 exit /b %errorlevel%
if "$rrIopOption"=="ON" "$rrBuild\racer_legacy_iop_idle_check.exe"
exit /b %errorlevel%
"@
    Set-Content -LiteralPath $rrBatch -Value ($rrCommands -replace "`r?`n", "`r`n") -Encoding ascii
    & $env:ComSpec /d /c $rrBatch 2>&1 | Tee-Object -FilePath (Join-Path $rrEvidence "$rrArch.log")
    $rrExit = $LASTEXITCODE
    Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch.exit") -Value $rrExit
    if ($rrExit -ne 0) { throw "Windows $rrArch build/check failed ($rrExit); logs: $rrEvidence" }
    Get-FileHash -LiteralPath "$rrBuild\racer_core_check.exe" -Algorithm SHA256 |
        ConvertTo-Json | Set-Content -LiteralPath (Join-Path $rrEvidence "$rrArch-hash.json")
}
Write-Output "Windows core checks passed. Evidence: $rrEvidence"
