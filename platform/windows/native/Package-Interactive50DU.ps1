#requires -Version 5.1
# Source-only implementation: invocation stages approved files, never builds or launches.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$Manifest,
    [Parameter(Mandatory=$true)][string]$Destination
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ($env:OS -ne 'Windows_NT') { throw 'Windows packaging entry required.' }
if ($MyInvocation.InvocationName -eq '.') { throw 'Do not dot-source this script.' }

function Require-Text($Value, [string]$Name) {
    if ($Value -isnot [string] -or [string]::IsNullOrWhiteSpace($Value)) { throw "Missing text: $Name" }
}
function Require-File([string]$Path) {
    # Windows PowerShell 5.1/.NET Framework lacks Path.IsPathFullyQualified.
    # Accept ordinary drive-absolute or UNC file paths on both 5.1 and 7;
    # reject drive-relative/current-drive-rooted and device namespace forms.
    $normalized = $Path.Replace('/', '\')
    $driveAbsolute = $normalized -match '^[A-Za-z]:\\'
    $uncAbsolute = $false
    if ($normalized -match '^\\\\([^\\]+)\\([^\\]+)\\') {
        $server = $Matches[1]
        $share = $Matches[2]
        $uncAbsolute = $server -notin @('.', '..') -and $share -notin @('.', '..') -and
            $server -notmatch '[:*?"<>|\x00-\x1f]' -and $share -notmatch '[:*?"<>|\x00-\x1f]'
    }
    if (-not ($driveAbsolute -or $uncAbsolute)) { throw "Source paths must be fully qualified drive or UNC file paths: $Path" }
    $item = Get-Item -LiteralPath $Path -Force
    if ($item.PSProvider.Name -ne 'FileSystem' -or $item.PSIsContainer -or $item.Length -eq 0) { throw "Expected nonempty file: $Path" }
    # Reject aliases via links/junctions, including any ancestor of the source.
    $cursor = $item
    while ($null -ne $cursor) {
        if (($cursor.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw "Reparse points are unsupported: $($cursor.FullName)" }
        if ($cursor -is [IO.FileInfo]) { $cursor = $cursor.Directory } else { $cursor = $cursor.Parent }
    }
    return $item.FullName
}
function Require-Relative([string]$Path) {
    # Portable conservative components; reject traversal, ADS, wildcards, device names.
    if ($Path -notmatch '^[A-Za-z0-9_-][A-Za-z0-9_.-]*(/[A-Za-z0-9_-][A-Za-z0-9_.-]*)*$') { throw "Invalid relative path: $Path" }
    foreach ($part in $Path.Split('/')) {
        if ($part.EndsWith('.') -or $part -match '^(?i:CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\.|$)') { throw "Reserved path: $Path" }
    }
}
function Require-Hash([string]$Path, [string]$Expected) {
    if ($Expected -notmatch '^[a-fA-F0-9]{64}$' -or (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash -ine $Expected) { throw "SHA256 mismatch: $Path" }
}

$manifestPath = Require-File $Manifest
$manifestHash = (Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash
$plan = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($plan.schema -ne 1) { throw 'Expected packaging manifest schema 1.' }
foreach ($field in @('sourceRevision', 'approvalReference', 'assetAuthenticationReference', 'runtimeClosureReference', 'crtPrerequisite', 'sdlProvenance', 'sdlVersion')) {
    Require-Text $plan.$field $field
}
if ($plan.sourceRevision -notmatch '^[a-fA-F0-9]{40}$') { throw 'Expected full source commit identity.' }
# These references are supplied attestations, never inferred from a hash/presence check.
if ($plan.audioProfile -notin @('muted', 'enabled')) { throw 'audioProfile must explicitly be muted or enabled.' }
Require-Relative $plan.corpus
if ($null -ne $plan.riderCache -and $plan.riderCache -ne '') { Require-Relative $plan.riderCache }
$out = [IO.Path]::GetFullPath($Destination)
if (Test-Path -LiteralPath $out) { throw 'Destination must be new; no reuse, overwrite or deletion.' }
$parent = Get-Item -LiteralPath ([IO.Path]::GetDirectoryName($out)) -Force
if (-not $parent.PSIsContainer) { throw 'Destination parent must already exist.' }
$cursor = $parent
while ($null -ne $cursor) {
    if (($cursor.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Destination ancestors must not be reparse points.' }
    $cursor = $cursor.Parent
}
$entries = @()
$targets = @{}
foreach ($file in $plan.files) {
    if ($file.domain -notin @('original', 'generated', 'rider', 'runtime')) { throw 'Unknown file domain.' }
    Require-Relative $file.path
    if ($file.domain -eq 'generated' -and $file.path -match '^(?i:work/native/project/disc)(/|$)') { throw 'Historical generated disc copies must not be packaged; use the original domain.' }
    Require-Text $file.provenance 'file.provenance'
    $source = Require-File $file.source
    Require-Hash $source $file.sha256
    $domain = $file.domain
    if ($domain -eq 'runtime') {
        # Native EXE and SDL remain adjacent to originals, never in a bin directory.
        if ($file.path -notmatch '^[A-Za-z0-9_.-]+\.(exe|dll)$' -or
            ($file.path -like '*.exe' -and $file.path -cne 'rr_interactive_vehicle50du.exe')) { throw 'Unexpected native runtime filename.' }
        $domain = 'original'
    } elseif ($file.path -match '(?i)\.(exe|dll|ps1|cmd|bat)$') {
        throw 'Executable/script files must not be disguised as assets.'
    }
    $relative = $domain + '/' + $file.path
    if ($targets.ContainsKey($relative)) { throw "Duplicate destination: $relative" }
    # A file cannot also be a parent directory of another entry.
    foreach ($existing in $targets.Keys) {
        if ($relative.StartsWith($existing + '/', [StringComparison]::OrdinalIgnoreCase) -or
            $existing.StartsWith($relative + '/', [StringComparison]::OrdinalIgnoreCase)) { throw 'File/directory collision.' }
    }
    $targets[$relative] = $file.domain
    $entries += [pscustomobject]@{ source = $source; path = $relative; domain = $file.domain; sha256 = $file.sha256; provenance = $file.provenance }
}
foreach ($required in @('original/rr_interactive_vehicle50du.exe', 'original/SDL2.dll')) {
    if (-not $targets.ContainsKey($required) -or $targets[$required] -ne 'runtime') { throw "Missing native runtime: $required" }
}
foreach ($required in @('SLUS_202.68', 'APPSTART.RES', 'LOC.RES', 'UI/UIFONTS.RES', 'UI/UIMAIN/UIMAIN.RES', 'UI/UIOPTION/UIOPTION.RES', 'UI/CONTPOP.RES', 'UI/TEXTURES/OVERLAYS.RES')) {
    if (-not $targets.ContainsKey('original/' + $required)) { throw "Missing original: $required" }
}
if (-not $targets.ContainsKey('generated/' + $plan.corpus)) { throw 'Corpus must be an inventoried generated file.' }
foreach ($prefix in @('generated/evidence/', 'generated/work/')) {
    if (@($targets.Keys | Where-Object { $_.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase) }).Count -eq 0) { throw "Missing prepared tree: $prefix" }
}
if ($plan.riderCache) {
    if (@($targets.Keys | Where-Object { $_.StartsWith('rider/' + $plan.riderCache + '/', [StringComparison]::OrdinalIgnoreCase) }).Count -eq 0) { throw 'Rider cache has no inventoried files.' }
    foreach ($required in @('UI/UIRIDER/UIRIDER.RES', 'UI/UIBKG.RES', 'UI/UIPODTEX.RES', 'UI/PODPHYS.RES', 'UI/UICOURSE/UICOURSE.RES')) {
        if (-not $targets.ContainsKey('original/' + $required)) { throw "Missing Rider/Course original: $required" }
    }
} elseif (@($entries | Where-Object { $_.domain -eq 'rider' }).Count -gt 0) { throw 'Rider files require an explicit riderCache path.' }
if ($plan.audioProfile -eq 'enabled' -or $plan.riderCache) {
    if (@($targets.Keys | Where-Object { $_.StartsWith('original/AUDIO/', [StringComparison]::OrdinalIgnoreCase) }).Count -eq 0) { throw 'Authentic AUDIO inventory is required even for muted ALL8 Course.' }
}
$buildReceipt = Require-File $plan.buildReceipt.source
Require-Hash $buildReceipt $plan.buildReceipt.sha256
$build = Get-Content -LiteralPath $buildReceipt -Raw | ConvertFrom-Json
$exeEntry = @($entries | Where-Object { $_.path -eq 'original/rr_interactive_vehicle50du.exe' })[0]
if ($build.status -ne 'built-not-run' -or $build.requestedSourceRevision -ine $plan.sourceRevision -or $build.executableSha256 -ine $exeEntry.sha256) { throw 'Build receipt does not identify the approved executable/revision.' }
# Record scripts as part of the distribution; do not run either entry.
foreach ($name in @('Launch-Interactive50DU.ps1', 'README-tester50du.md')) {
    $source = Require-File (Join-Path $PSScriptRoot $name)
    $entryPath = $name
    if ($name -eq 'README-tester50du.md') { $entryPath = 'platform/windows/native/' + $name }
    $entries += [pscustomobject]@{ source = $source; path = $entryPath; domain = 'entry'; sha256 = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash; provenance = 'packaging source tree; caller must review local changes' }
}
# Preserve the tester guide's repository-relative contract link inside the package.
$contractRelative = 'docs/recovery/windows-release-packaging-20261014.md'
$contractSource = Require-File ([IO.Path]::GetFullPath((Join-Path $PSScriptRoot ('../../../' + $contractRelative))))
$entries += [pscustomobject]@{ source = $contractSource; path = $contractRelative; domain = 'entry'; sha256 = (Get-FileHash -LiteralPath $contractSource -Algorithm SHA256).Hash; provenance = 'packaging contract from source tree; caller must review local changes' }
$entries += [pscustomobject]@{ source = $manifestPath; path = 'package-manifest.json'; domain = 'receipt'; sha256 = $manifestHash; provenance = 'approved input manifest' }
$entries += [pscustomobject]@{ source = $buildReceipt; path = 'build-receipt.json'; domain = 'receipt'; sha256 = $plan.buildReceipt.sha256; provenance = 'build identity; acceptance not run' }
# Check the complete copy list, including manifest, build receipt, launcher, contract and
# guide, before creating output. Trim first so a drive/share root has one boundary.
foreach ($entry in $entries) {
    $sourceDirectory = [IO.Path]::GetDirectoryName($entry.source).TrimEnd([char[]]'\/')
    $destinationDirectory = $out.TrimEnd([char[]]'\/')
    if ([StringComparer]::OrdinalIgnoreCase.Equals($destinationDirectory, $sourceDirectory) -or
        $destinationDirectory.StartsWith(($sourceDirectory + [IO.Path]::DirectorySeparatorChar), [StringComparison]::OrdinalIgnoreCase)) {
        throw "Destination must be separate from every source directory: $sourceDirectory"
    }
}
$receipt = [ordered]@{
    schema = 1; status = 'staging'; sourceRevision = $plan.sourceRevision
    manifestSha256 = $manifestHash; approvalReference = $plan.approvalReference
    assetAuthenticationReference = $plan.assetAuthenticationReference
    runtimeClosureReference = $plan.runtimeClosureReference
    crtPrerequisite = $plan.crtPrerequisite; sdlProvenance = $plan.sdlProvenance; sdlVersion = $plan.sdlVersion
    sourceRevisionVerifiedByScript = $false; archiveAuthenticationPerformed = $false
    runtimeDependencyClosureVerified = $false; runtimeAcceptance = 'not run'
    audioProfile = $plan.audioProfile; allOriginalAi = $true; hostCourseDriver = $false
    inventoryComplete = $false; inventoryScope = 'incomplete; retained files may be unlisted'
    files = @(); error = $null
}
$null = New-Item -ItemType Directory -Path $out
try {
    foreach ($entry in $entries) {
        Require-Hash $entry.source $entry.sha256
        $target = Join-Path $out $entry.path
        $directory = [IO.Path]::GetDirectoryName($target)
        if (-not (Test-Path -LiteralPath $directory)) { $null = New-Item -ItemType Directory -Path $directory -Force }
        # No overwrite; a failed staging tree is retained for review, never reused.
        [IO.File]::Copy($entry.source, $target, $false)
        Require-Hash $target $entry.sha256
        $receipt.files += [ordered]@{ path = $entry.path; domain = $entry.domain; sha256 = $entry.sha256; bytes = (Get-Item -LiteralPath $target).Length; provenance = $entry.provenance }
    }
    $launch = [ordered]@{
        NativeExecutable = 'original/rr_interactive_vehicle50du.exe'; OriginalAssetRoot = 'original'
        GeneratedRuntimeRoot = 'generated'; Corpus = ('generated/' + $plan.corpus); AudioProfile = $plan.audioProfile
    }
    if ($plan.riderCache) { $launch['RiderIpumCache'] = 'rider/' + $plan.riderCache }
    $launchPath = Join-Path $out 'launch-parameters.json'
    $launch | ConvertTo-Json | Set-Content -LiteralPath $launchPath -Encoding UTF8
    $receipt.files += [ordered]@{ path = 'launch-parameters.json'; domain = 'entry'; sha256 = (Get-FileHash -LiteralPath $launchPath -Algorithm SHA256).Hash; bytes = (Get-Item -LiteralPath $launchPath).Length; provenance = 'derived from manifest; paths relative to package root' }
    $receipt.status = 'staged-not-run'
} catch {
    $receipt.status = 'failed'
    $receipt.error = $_.Exception.Message
    throw
} finally {
    # Normal cancellation may unwind without entering catch. Never leave staging
    # as a final receipt status, or imply a partial copy/hash inventory is closure.
    if ($receipt.status -eq 'staging') {
        $receipt.status = 'interrupted'
        $receipt.error = 'Staging interrupted before completion.'
    }
    $receipt.inventoryComplete = ($receipt.status -eq 'staged-not-run')
    if ($receipt.inventoryComplete) {
        $receipt.inventoryScope = 'complete declared staged inventory excluding this receipt; runtime closure unverified'
    } else {
        $receipt.inventoryScope = 'incomplete; failed/interrupted staging may retain unlisted or partially written files; no closure claim'
    }
    $receipt | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath (Join-Path $out 'package-receipt.json') -Encoding UTF8
}
Write-Host "Staged declared inventory at $out. Runtime closure and acceptance remain unverified."

