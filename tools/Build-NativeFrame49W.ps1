param(
    [ValidateSet('x64','x86','both')][string]$Architecture = 'x64',
    [string]$VisualStudioRoot = 'C:\Program Files\Microsoft Visual Studio\18\Community',
    [string]$VcVersion = '14.44.35207',
    [string]$SdkVersion = '10.0.26100.0',
    [string]$ToolsRoot = 'C:\Users\LRPC\Documents\Codex\2026-09-22\referenced-chatgpt-conversation-this-is-an\work\build-tools',
    [string]$PythonPath = 'C:\Users\LRPC\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
)
$ErrorActionPreference = 'Stop'
$rrRoot = Split-Path -Parent $PSScriptRoot
$rrReceipt = Join-Path $rrRoot 'work\handoff49w\integration49w\result.json'
if (!(Test-Path -LiteralPath $rrReceipt)) { throw 'Complete the verified 49W integration before building.' }
$rrCmake = Join-Path $ToolsRoot 'cmake\data\bin\cmake.exe'
$rrCtest = Join-Path $ToolsRoot 'cmake\data\bin\ctest.exe'
$rrNinja = Join-Path $ToolsRoot 'bin\ninja.exe'
$rrVc = Join-Path $VisualStudioRoot "VC\Tools\MSVC\$VcVersion"
$rrSdk = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10'
$rrEvidence = Join-Path $rrRoot ('evidence\nativeFrame49W-Windows-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $rrEvidence | Out-Null
$rrTargets = if ($Architecture -eq 'both') { @('x64','x86') } else { @($Architecture) }
$rrSummary = [ordered]@{ scope='Native staged vehicle frame; not gameplay FPS'; architecture=$Architecture; compiler=$VcVersion; sdk=$SdkVersion; evidence=$rrEvidence; results=@() }
function Invoke-RacerStep([string]$Program, [string[]]$StepArguments, [string]$LogPath) {
    & $Program @StepArguments 2>&1 | Tee-Object -FilePath $LogPath
    $rrStepExit = $LASTEXITCODE
    Set-Content -LiteralPath ($LogPath + '.exit') -Value $rrStepExit
    if ($rrStepExit -ne 0) { throw "Step failed ($rrStepExit): $LogPath" }
}
try {
    foreach ($rrArch in $rrTargets) {
        $rrCompiler = Join-Path $rrVc "bin\HostX64\$rrArch\cl.exe"
        $rrZlib = Join-Path $rrRoot "build\native-pc48K-$rrArch\racer_native_zlib.lib"
        foreach ($rrRequired in @($rrCompiler,$rrCmake,$rrCtest,$rrNinja,$rrZlib,$PythonPath)) {
            if (!(Test-Path -LiteralPath $rrRequired)) { throw "Missing build dependency: $rrRequired" }
        }
        $env:PATH = "$rrVc\bin\HostX64\$rrArch;$rrSdk\bin\$SdkVersion\x64;" + $env:PATH
        $env:INCLUDE = "$rrVc\include;$rrSdk\Include\$SdkVersion\ucrt;$rrSdk\Include\$SdkVersion\shared;$rrSdk\Include\$SdkVersion\um;$rrSdk\Include\$SdkVersion\winrt"
        $env:LIB = "$rrVc\lib\$rrArch;$rrSdk\Lib\$SdkVersion\ucrt\$rrArch;$rrSdk\Lib\$SdkVersion\um\$rrArch"
        $rrBuild = Join-Path $rrRoot "build\native-frame49W-$rrArch"
        $rrCxxFlags = if ($rrArch -eq 'x86') { '/utf-8 /arch:IA32' } else { '/utf-8' }
        Get-FileHash -LiteralPath $rrCompiler,$rrZlib,$rrCmake,$rrNinja,$PythonPath -Algorithm SHA256 |
            ConvertTo-Json | Set-Content -LiteralPath "$rrEvidence\$rrArch-tool-hashes.json"
        Invoke-RacerStep $rrCmake @('-S',$rrRoot,'-B',$rrBuild,'-G','Ninja',"-DCMAKE_MAKE_PROGRAM=$rrNinja",'-DCMAKE_BUILD_TYPE=Release','-DCMAKE_CXX_COMPILER=cl','-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded',"-DCMAKE_CXX_FLAGS=$rrCxxFlags",'-DCMAKE_EXPORT_COMPILE_COMMANDS=ON','-DRR_NATIVE_VEHICLE_FRAME_ONLY=ON',"-DZLIB_LIBRARY=$rrZlib","-DZLIB_INCLUDE_DIR=$rrRoot\third_party\native-zlib") "$rrEvidence\$rrArch-configure.log"
        $rrSnapshotTool = Join-Path $rrRoot 'tools/native_frame_inputs49y.py'
        Invoke-RacerStep $PythonPath @($rrSnapshotTool,"$rrBuild\compile_commands.json","$rrEvidence\$rrArch-source-hashes.json") "$rrEvidence\$rrArch-source-snapshot.log"
        Invoke-RacerStep $rrCmake @('--build',$rrBuild,'--parallel','2') "$rrEvidence\$rrArch-build.log"
        Invoke-RacerStep $rrCtest @('--test-dir',$rrBuild,'--output-on-failure','--output-junit',"$rrEvidence\$rrArch-tests.xml",'-j','1') "$rrEvidence\$rrArch-tests.log"
        Invoke-RacerStep $PythonPath @($rrSnapshotTool,"$rrBuild\compile_commands.json","$rrEvidence\$rrArch-source-hashes.json",'--verify') "$rrEvidence\$rrArch-source-verification.log"
        Copy-Item -LiteralPath "$rrBuild\compile_commands.json" -Destination "$rrEvidence\$rrArch-compile-commands.json"
        Get-ChildItem -LiteralPath $rrBuild -Filter '*.exe' -File | Get-FileHash -Algorithm SHA256 |
            ConvertTo-Json | Set-Content -LiteralPath "$rrEvidence\$rrArch-binary-hashes.json"
        $rrSummary.results += @{architecture=$rrArch;status='passed';build=$rrBuild}
    }
    $rrSummary.status = 'passed'
} catch {
    $rrSummary.status = 'failed'
    $rrSummary.error = $_.Exception.Message
    throw
} finally {
    $rrSummary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath "$rrEvidence\summary.json"
    Write-Output "Evidence: $rrEvidence"
}
