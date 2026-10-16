#requires -Version 5.1
# Manual entry for an already prepared native Windows client. No build or test runner.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string]$NativeExecutable,
    [Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string]$OriginalAssetRoot,
    [Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string]$GeneratedRuntimeRoot,
    [Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string]$Corpus,
    [ValidateNotNullOrEmpty()][string]$RiderIpumCache,
    [ValidateSet('muted', 'enabled')][string]$AudioProfile = 'muted'
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-RequiredPath([string]$Path, [bool]$Directory, [string]$Hint) {
    if ([string]::IsNullOrWhiteSpace($Path) -or $Path -match '[\x00\r\n"]') {
        throw "Invalid path '$Path'. $Hint"
    }
    try { $item = Get-Item -LiteralPath $Path -Force -ErrorAction Stop }
    catch { throw "Cannot access '$Path'. $Hint Detail: $($_.Exception.Message)" }
    if ($item.PSProvider.Name -ne 'FileSystem' -or $item.PSIsContainer -ne $Directory) {
        throw "Wrong path type: '$Path'. $Hint"
    }
    if (-not $Directory -and $item.Length -eq 0) {
        throw "Empty file: '$Path'. $Hint"
    }
    return $item.FullName
}

function Assert-NativeExecutable([string]$Path, [bool]$IsDll = $false) {
    if (-not $IsDll -and [IO.Path]::GetExtension($Path) -ine '.exe') {
        throw 'NativeExecutable must be the approved Windows .exe, never SLUS or a script.'
    }
    $reader = $null
    try {
        $reader = [IO.BinaryReader]::new([IO.File]::OpenRead($Path))
        if ($reader.BaseStream.Length -lt 64 -or $reader.ReadUInt16() -ne 0x5a4d) {
            throw 'Missing DOS/PE header.'
        }
        $reader.BaseStream.Position = 0x3c
        $offset = $reader.ReadUInt32()
        if ($offset -gt $reader.BaseStream.Length - 26) { throw 'Invalid PE offset.' }
        $reader.BaseStream.Position = $offset
        if ($reader.ReadUInt32() -ne 0x4550 -or $reader.ReadUInt16() -ne 0x8664) {
            throw 'Expected a native AMD64 PE executable.'
        }
        $reader.BaseStream.Position = $offset + 22
        if (((($reader.ReadUInt16() -band 0x2000) -ne 0) -ne $IsDll) -or $reader.ReadUInt16() -ne 0x20b) {
            throw 'Expected PE32+ with the requested executable/DLL type.'
        }
    } catch {
        throw "Cannot use '$Path': $($_.Exception.Message) Supply the approved native x64 build; renaming SLUS or a Linux binary is not a port."
    } finally {
        if ($null -ne $reader) { $reader.Dispose() }
    }
}

if ([Environment]::OSVersion.Platform -ne [PlatformID]::Win32NT -or
    -not [Environment]::Is64BitOperatingSystem) {
    throw 'Use Windows with support for the approved native x64 client.'
}
if ($MyInvocation.InvocationName -eq '.') {
    throw 'Invoke this launcher with & or -File; do not dot-source it.'
}

# Resolve caller inputs before setting the child's cwd. No asset traversal or preparation.
$exe = Get-RequiredPath $NativeExecutable $false 'Set -NativeExecutable to your approved native x64 build.'
Assert-NativeExecutable $exe
$assets = Get-RequiredPath $OriginalAssetRoot $true 'Set -OriginalAssetRoot to the original disc folder beside SLUS_202.68.'
# The approved native EXE must be beside the original SLUS, not merely use its cwd.
$exeDirectory = [IO.Path]::GetDirectoryName($exe)
if (-not [StringComparer]::OrdinalIgnoreCase.Equals(
    $exeDirectory.TrimEnd([char[]]'\/'), $assets.TrimEnd([char[]]'\/'))) {
    throw "NativeExecutable is in '$exeDirectory', but OriginalAssetRoot is '$assets'. Point both parameters to the same original asset directory containing SLUS_202.68. Ask the runtime owner to stage the approved native EXE and SDL2.dll there; do not move or rename original assets."
}
$generated = Get-RequiredPath $GeneratedRuntimeRoot $true 'Set -GeneratedRuntimeRoot to the already prepared folder containing evidence and work.'
$corpusPath = Get-RequiredPath $Corpus $false 'Set -Corpus to the already prepared collision corpus JSON file.'
$sdl = Get-RequiredPath (Join-Path ([IO.Path]::GetDirectoryName($exe)) 'SDL2.dll') $false 'Supply the approved x64 SDL2.dll beside the native executable; this launcher installs or copies nothing.'

Assert-NativeExecutable $sdl $true

# Presence checks only. The native client owns archive/corpus authentication.
foreach ($relative in @('SLUS_202.68', 'APPSTART.RES', 'LOC.RES', 'UI/UIFONTS.RES',
    'UI/UIMAIN/UIMAIN.RES', 'UI/UIOPTION/UIOPTION.RES', 'UI/CONTPOP.RES', 'UI/TEXTURES/OVERLAYS.RES')) {
    $null = Get-RequiredPath (Join-Path $assets $relative) $false 'Choose the intact original asset root with its original directory layout. Do not substitute generated files.'
}
foreach ($relative in @('evidence', 'work')) {
    $null = Get-RequiredPath (Join-Path $generated $relative) $true 'Choose the prepared generated-runtime root, not its evidence/work subfolder. Ask the runtime owner for missing preparation.'
}
$riderCache = $null
if ($PSBoundParameters.ContainsKey('RiderIpumCache')) {
    $riderCache = Get-RequiredPath $RiderIpumCache $true 'Set -RiderIpumCache to the matching prepared original Rider image cache; omit it for Main/Options only.'
    foreach ($relative in @('UI/UIRIDER/UIRIDER.RES', 'UI/UIBKG.RES', 'UI/UIPODTEX.RES',
        'UI/PODPHYS.RES', 'UI/UICOURSE/UICOURSE.RES')) {
        $null = Get-RequiredPath (Join-Path $assets $relative) $false 'Rider/Course entry needs the original UI archives beside the same SLUS. Ask the runtime owner for missing assets.'
    }
    $null = Get-RequiredPath (Join-Path $assets 'AUDIO') $true 'Muted ALL8 driving still needs authentic AUDIO for logical sound/sample ownership.'
}

# Both enabled consumers use the same authentic original AUDIO directory.
if ($AudioProfile -eq 'enabled') {
    $null = Get-RequiredPath (Join-Path $assets 'AUDIO') $true 'Enabled audio requires authentic original AUDIO; no fallback or generated substitute.'
}
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $exe
$start.WorkingDirectory = $assets
$start.UseShellExecute = $false
$start.CreateNoWindow = $false
# One nonempty file path, without quotes/newlines or a trailing directory separator.
# Quoting this single argument works in both Windows PowerShell 5.1 and PowerShell 7.
$start.Arguments = '"' + $corpusPath + '"'

# Modify only this ProcessStartInfo copy; never write Env:, SetEnvironmentVariable or PATH.
foreach ($name in @($start.EnvironmentVariables.Keys)) {
    if ($name -imatch '^(RR_|SDL_|MESA_|LIBGL_)') {
        $start.EnvironmentVariables.Remove($name)
    }
}
$settings = @{
    RR_ORIGINAL_ASSET_ROOT = $assets
    RR_ORIGINAL_UI_ROOT = $assets
    RR_GENERATED_RUNTIME_ROOT = $generated
    RR_ORIGINAL_MAIN_MENU = '1'
    RR_START_MENU = '0'
    RR_ORIGINAL_MAIN_AUDIO = 'muted'
    RR_AUDIO = '0'
    RR_AUDIO_DEVICE = '0'
    RR_AUDIO_ROOT = (Join-Path $assets 'AUDIO')
    RR_TRACK = 'TA'
    RR_RACE = '1'
    RR_RACE_MODE = '2'
    RR_ORIGINAL_AI_ALL_RACERS = '1'
    RR_AUTODRIVE = '0'
    RR_DISABLE_RACER_CONTACTS = '0'
}
if ($AudioProfile -eq 'enabled') {
    $settings['RR_ORIGINAL_MAIN_AUDIO'] = 'enabled'
    $settings['RR_ORIGINAL_MAIN_AUDIO_ROOT'] = Join-Path $assets 'AUDIO'
    $settings['RR_AUDIO'] = '1'
    $settings['RR_AUDIO_DEVICE'] = '1'
}
if ($null -ne $riderCache) { $settings['RR_ORIGINAL_RIDER_IPUM_CACHE'] = $riderCache }
foreach ($name in $settings.Keys) { $start.EnvironmentVariables[$name] = $settings[$name] }

Write-Host "Native executable: $exe"
Write-Host "Original asset cwd: $assets"
Write-Host "Prepared runtime: $generated"
Write-Host "Corpus: $corpusPath"
Write-Host "Opening original Main once, audio profile: $AudioProfile; all-eight original AI for admitted races."
if ($null -eq $riderCache) { Write-Host 'No Rider cache supplied: Rider/Course entry remains unavailable.' }

$child = [Diagnostics.Process]::new()
$child.StartInfo = $start
try {
    try {
        if (-not $child.Start()) { throw 'Windows did not start the native process.' }
    } catch {
        throw "Native launch failed: $($_.Exception.Message) Check the approved build, adjacent x64 SDL2.dll and Visual C++ runtime. This launcher does not install dependencies."
    }
    # Inherit the interactive console. Do not redirect/send stdin, capture frames, or retry.
    $child.WaitForExit()
    $nativeExitCode = $child.ExitCode
} finally {
    $child.Dispose()
}
Write-Host "Native client exited with code $nativeExitCode. Exit status alone is not a menu or gameplay pass."
if ($nativeExitCode -ne 0) {
    Write-Warning 'Read the native error above. Check the matching executable, authentic assets, prepared corpus/caches and graphics/runtime dependencies with the runtime owner before another manual launch.'
}
exit $nativeExitCode
