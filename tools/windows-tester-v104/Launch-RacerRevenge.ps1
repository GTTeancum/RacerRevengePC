#requires -Version 5.1
# Optional explicit original-asset-root entry. Native EXE owns RacerRevenge.log.
[CmdletBinding()]
param([string]$OriginalAssetRoot = $PSScriptRoot)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$exe = Join-Path $PSScriptRoot 'RacerRevengePC.exe'
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) { throw 'RacerRevengePC.exe is missing. Extract the complete package.' }
$assets = (Get-Item -LiteralPath $OriginalAssetRoot -ErrorAction Stop).FullName
if ($assets -match '[\x00\r\n"]') { throw 'The original asset path contains unsupported characters.' }
$start = New-Object Diagnostics.ProcessStartInfo
$start.FileName = $exe
$start.WorkingDirectory = $PSScriptRoot
$argumentRoot = $assets.TrimEnd([char[]]'\/') + '\.'
$start.Arguments = '--assets "' + $argumentRoot + '"'
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
foreach ($name in @($start.EnvironmentVariables.Keys)) {
    if ($name -imatch '^(RR_|SDL_|MESA_|LIBGL_)') { $start.EnvironmentVariables.Remove($name) }
}
$start.EnvironmentVariables['RR_NATIVE_FIXED_WINDOW'] = '1'
$start.EnvironmentVariables['RR_ORIGINAL_RIDER_PRESENTATION'] = 'allow-native-pc-deadline60hz'
$child = New-Object Diagnostics.Process
$child.StartInfo = $start
try {
    if (-not $child.Start()) { throw 'Windows did not start the native executable.' }
    $child.WaitForExit()
    $code = $child.ExitCode
} finally { $child.Dispose() }
exit $code
