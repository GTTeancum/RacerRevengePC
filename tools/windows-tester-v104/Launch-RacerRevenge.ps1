#requires -Version 5.1
[CmdletBinding()]
param([string]$OriginalAssetRoot = $PSScriptRoot)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$exitCode = 1
$child = $null
$stdout = $null
$stderr = $null
$run = $null
$receipt = [ordered]@{
    format = 'RacerRevenge-Windows-diagnostics-1'
    startedUtc = [DateTime]::UtcNow.ToString('o')
    status = 'preparing'
    requestedWindow = '1280x896 windowed client area'
    originalCanvas = '640x448'
    telemetry = $false
    osVersion = [Environment]::OSVersion.VersionString
    os64Bit = [Environment]::Is64BitOperatingSystem
}
try {
    $runName = [DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff') + '-' + [Guid]::NewGuid().ToString('N').Substring(0,8)
    $logRoot = Join-Path $PSScriptRoot 'RacerRevenge-logs'
    try { $run = (New-Item -ItemType Directory -Path (Join-Path $logRoot $runName) -ErrorAction Stop).FullName }
    catch {
        if ([string]::IsNullOrWhiteSpace($env:LOCALAPPDATA)) { throw }
        $logRoot = Join-Path $env:LOCALAPPDATA 'RacerRevenge\logs'
        $run = (New-Item -ItemType Directory -Path (Join-Path $logRoot $runName) -ErrorAction Stop).FullName
    }
    Write-Host "Diagnostics for this run: $run"
    'Close the game, then ZIP this whole run folder and attach it in your conversation. Review files first: local paths, device/driver information and game selections may be included. Nothing is uploaded automatically. Do not include original game assets or your save/profile files.' | Set-Content -LiteralPath (Join-Path $run 'HOW-TO-RETURN-LOGS.txt') -Encoding UTF8
    $receiptPath = Join-Path $run 'launch.json'
    $receipt | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $receiptPath -Encoding UTF8
    if (-not [Environment]::Is64BitOperatingSystem) { throw 'This package requires 64-bit Windows.' }
    $exe = Join-Path $PSScriptRoot 'RacerRevengePC.exe'
    $sdl = Join-Path $PSScriptRoot 'SDL2.dll'
    $avcodec = Join-Path $PSScriptRoot 'avcodec-61.dll'
    $avutil = Join-Path $PSScriptRoot 'avutil-59.dll'
    $pthread = Join-Path $PSScriptRoot 'libwinpthread-1.dll'
    foreach ($file in @($exe, $sdl, $avcodec, $avutil, $pthread)) {
        if (-not (Test-Path -LiteralPath $file -PathType Leaf)) { throw "Required package file missing: $file. Extract the entire ZIP." }
    }
    $assets = (Get-Item -LiteralPath $OriginalAssetRoot -ErrorAction Stop).FullName
    if ($assets -match '[\x00\r\n"]') { throw 'The original asset path contains unsupported characters.' }
    foreach ($relative in @('SLUS_202.68','APPSTART.RES','LOC.RES','UI\UIFONTS.RES','UI\UIMAIN\UIMAIN.RES')) {
        if (-not (Test-Path -LiteralPath (Join-Path $assets $relative) -PathType Leaf)) {
            throw "Original disc file missing: $relative. Put the package beside your intact original disc files, or pass -OriginalAssetRoot to this launcher. No game assets are bundled."
        }
    }
    $receipt['executableSha256'] = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash
    $receipt['sdlSha256'] = (Get-FileHash -LiteralPath $sdl -Algorithm SHA256).Hash
    $receipt['avcodecSha256'] = (Get-FileHash -LiteralPath $avcodec -Algorithm SHA256).Hash
    $receipt['avutilSha256'] = (Get-FileHash -LiteralPath $avutil -Algorithm SHA256).Hash
    $receipt['winpthreadSha256'] = (Get-FileHash -LiteralPath $pthread -Algorithm SHA256).Hash
    $receipt['ffmpegAbi'] = 'FFmpeg 7.1.5: avcodec-61.dll + avutil-59.dll'
    $receipt['assetRoot'] = $assets
    $start = New-Object Diagnostics.ProcessStartInfo
    $start.FileName = $exe
    $start.WorkingDirectory = $assets
    # Avoid a trailing backslash before the Windows closing quote (e.g. D:\).
    $argumentRoot = $assets.TrimEnd([char[]]'\/') + '\.'
    $start.Arguments = '--assets "' + $argumentRoot + '"'
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    foreach ($name in @($start.EnvironmentVariables.Keys)) {
        if ($name -imatch '^(RR_|SDL_|MESA_|LIBGL_)') { $start.EnvironmentVariables.Remove($name) }
    }
    # No fixtures, unlock overrides, generated data, AI-player or unattended route.
    # --assets selects the actual Main and its ordinary human Course handoff.
    $start.EnvironmentVariables['RR_NATIVE_FIXED_WINDOW'] = '1'
    # Established host pacing only; no Rider/profile/resource guard changes.
    $start.EnvironmentVariables['RR_ORIGINAL_RIDER_PRESENTATION'] = 'allow-native-pc-deadline60hz'
    $receipt['riderPresentation'] = 'allow-native-pc-deadline60hz'
    $stdout = [IO.File]::Open((Join-Path $run 'stdout.log'), [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
    $stderr = [IO.File]::Open((Join-Path $run 'stderr.log'), [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
    $child = New-Object Diagnostics.Process
    $child.StartInfo = $start
    Write-Host 'Opening original Main in a 1280x896 window. Arrows select, Enter confirms, Escape backs out.'
    Write-Host 'The console stays open to record this run. Close the game window when finished.'
    if (-not $child.Start()) { throw 'Windows did not start the native executable.' }
    $receipt['status'] = 'running'
    $receipt['processId'] = $child.Id
    $receipt | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $receiptPath -Encoding UTF8
    # Drain both streams concurrently to disk. No growing in-memory log buffer,
    # serial pipe deadlock, event-handler thread, stdin injection or telemetry.
    $stdoutCopy = $child.StandardOutput.BaseStream.CopyToAsync($stdout)
    $stderrCopy = $child.StandardError.BaseStream.CopyToAsync($stderr)
    $child.WaitForExit()
    $stdoutCopy.GetAwaiter().GetResult()
    $stderrCopy.GetAwaiter().GetResult()
    $exitCode = $child.ExitCode
    $receipt['status'] = 'exited'
    $receipt['nativeExitCode'] = $exitCode
} catch {
    $receipt['status'] = 'launcher_error'
    $receipt['error'] = $_.Exception.ToString()
    Write-Host ("Launch failed: " + $_.Exception.Message) -ForegroundColor Red
    if ($null -ne $run) { $_ | Out-String | Set-Content -LiteralPath (Join-Path $run 'launcher-error.txt') -Encoding UTF8 }
} finally {
    if ($null -ne $stdout) { $stdout.Dispose() }
    if ($null -ne $stderr) { $stderr.Dispose() }
    if ($null -ne $child) { $child.Dispose() }
    $receipt['finishedUtc'] = [DateTime]::UtcNow.ToString('o')
    if ($null -ne $run) {
        $receipt | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $run 'launch.json') -Encoding UTF8
        Write-Host "Return logs by ZIPping this whole folder after closing the game: $run"
    }
}
Write-Host "Native/launcher exit code: $exitCode. This alone does not establish a gameplay pass."
exit $exitCode
