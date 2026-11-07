# Single native x64 development-client build. Never launches the client or tests.
# Run only after the consolidated source and dependencies are approved.
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$SourceRoot,
    [Parameter(Mandatory=$true)][string]$BuildDirectory,
    [Parameter(Mandatory=$true)][string]$Compiler,
    [Parameter(Mandatory=$true)][string]$Linker,
    [Parameter(Mandatory=$true)][string]$SDL2IncludeDirectory,
    [string]$SDL2RuntimeDll,
    [string]$SourceRevision = 'unverified-working-tree',
    [ValidateRange(1, 4)][int]$MaxCompileJobs = 1,
    [switch]$CollectSourceErrors
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ($env:OS -ne 'Windows_NT') { throw 'Windows x64 MSVC environment required.' }
# Compiler include/library lookup is inherited only from an already configured
# x64 MSVC developer shell. This script installs and initializes nothing.
if ($env:VSCMD_ARG_TGT_ARCH -ne 'x64') { throw 'Use an already authorized x64 MSVC developer shell.' }
if ($env:CL -or $env:_CL_ -or $env:LINK -or $env:_LINK_) {
    throw 'Clear CL, _CL_, LINK and _LINK_ override variables; build flags must be explicit.'
}
function ExistingFile([string]$Path) {
    $p = (Resolve-Path -LiteralPath $Path).Path
    if (-not (Test-Path -LiteralPath $p -PathType Leaf)) { throw "Not a file: $p" }
    return $p
}
function ExistingDirectory([string]$Path) {
    $p = (Resolve-Path -LiteralPath $Path).Path
    if (-not (Test-Path -LiteralPath $p -PathType Container)) { throw "Not a directory: $p" }
    return $p
}
function RspQuote([string]$Value) {
    if ($Value.Contains('"') -or $Value.Contains("`r") -or $Value.Contains("`n")) { throw 'Invalid response-file path.' }
    return '"' + $Value + '"'
}
function RequireAmd64PE([string]$Path) {
    $stream = [IO.File]::OpenRead($Path)
    $reader = New-Object IO.BinaryReader($stream)
    try {
        if ($reader.ReadUInt16() -ne 0x5a4d) { throw "Not PE: $Path" }
        $stream.Position = 0x3c
        $offset = $reader.ReadUInt32()
        if ($offset -gt $stream.Length - 26) { throw "Invalid PE offset: $Path" }
        $stream.Position = $offset
        if ($reader.ReadUInt32() -ne 0x4550 -or $reader.ReadUInt16() -ne 0x8664) { throw "Expected AMD64 PE: $Path" }
        $stream.Position = $offset + 24
        if ($reader.ReadUInt16() -ne 0x20b) { throw "Expected PE32+: $Path" }
    } finally { $reader.Dispose() }
}
$root = ExistingDirectory $SourceRoot
$compilerPath = ExistingFile $Compiler
$linkerPath = ExistingFile $Linker
$zlibInclude = ExistingDirectory (Join-Path $root 'third_party/native-zlib')
$null = ExistingFile (Join-Path $zlibInclude 'zlib.h')
$null = ExistingFile (Join-Path $zlibInclude 'zconf.h')
# Existing pinned FFmpeg headers support the actual IPU decoder source owner.
$ffmpegInclude = ExistingDirectory (Join-Path $root 'third_party/ffmpeg-7.1.5/include')
$null = ExistingFile (Join-Path $ffmpegInclude 'libavcodec/avcodec.h')
$null = ExistingFile (Join-Path $ffmpegInclude 'libavutil/cpu.h')
$ffmpegProvenance = ExistingFile (Join-Path $root 'third_party/ffmpeg-7.1.5/UPSTREAM.json')
$sdlInclude = ExistingDirectory $SDL2IncludeDirectory
foreach ($header in @('SDL_audio.h', 'SDL_events.h', 'SDL_version.h')) {
    $null = ExistingFile (Join-Path $sdlInclude $header)
}
$runtimeDlls = @()
if ($SDL2RuntimeDll) {
    $sdlDll = ExistingFile $SDL2RuntimeDll
    if ([IO.Path]::GetFileName($sdlDll) -cne 'SDL2.dll') { throw 'Provide the approved runtime with exact name SDL2.dll.' }
    RequireAmd64PE $sdlDll
    $runtimeDlls += $sdlDll
}
$manifestPath = ExistingFile (Join-Path $PSScriptRoot 'interactive50du.sources.json')
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($manifest.schema -ne 1 -or $manifest.sources.Count -ne 608 -or $manifest.nativeDependencySources.Count -ne 6) { throw 'Review changed source manifest before changing this gate.' }
foreach ($entry in (@($manifest.sourceListProvenance.PSObject.Properties) + @($manifest.nativeDependencySourceProvenance.PSObject.Properties))) {
    $actual = (Get-FileHash -LiteralPath (Join-Path $root $entry.Name) -Algorithm SHA256).Hash
    if ($actual -ine $entry.Value) { throw "Generator source changed; reconcile manifest first: $($entry.Name)" }
}
$sources = @()
$seen = @{}
foreach ($relative in (@($manifest.sources) + @($manifest.nativeDependencySources))) {
    if ($relative -notmatch '^((src/(game|assets)|platform/linux/native|tests/native)/[A-Za-z0-9_]+\.cpp|third_party/native-zlib/(adler32|crc32|inflate|inffast|inftrees|zutil)\.c)$') { throw "Unexpected source path: $relative" }
    if ($relative -match '(offscreen|joystick_transport|pcsx2)' -or $seen.ContainsKey($relative)) { throw "Forbidden or duplicate source: $relative" }
    $seen[$relative] = $true
    $sources += ExistingFile (Join-Path $root $relative)
}
$includes = @((Join-Path $root 'src/assets'), (Join-Path $root 'src/game'), (Join-Path $root 'third_party/native-json'), $zlibInclude, $ffmpegInclude, $sdlInclude)
foreach ($inc in $includes) { $null = ExistingDirectory $inc }
$null = ExistingFile (Join-Path $root 'platform/linux/native/native_dynamic_library50win.h')
$out = [IO.Path]::GetFullPath($BuildDirectory)
if (Test-Path -LiteralPath $out) { throw 'BuildDirectory must be new; existing outputs are never reused or deleted.' }
# Capture source and header inputs, including local retained fixture headers.
$inputFiles = @($sources) + @($manifestPath, $PSCommandPath, $compilerPath, $linkerPath, $ffmpegProvenance) + $runtimeDlls
foreach ($dir in @('src', 'tests/native', 'platform/linux/native', 'platform/windows/native', 'third_party/native-json', 'third_party/native-zlib', 'third_party/ffmpeg-7.1.5/include')) {
    $inputFiles += Get-ChildItem -LiteralPath (Join-Path $root $dir) -Recurse -File | Where-Object { $_.Extension -in '.h', '.hpp', '.inc', '.inl' } | ForEach-Object { $_.FullName }
}
$inputFiles += Get-ChildItem -LiteralPath $sdlInclude -Recurse -File | Where-Object { $_.Extension -in '.h', '.hpp', '.inc', '.inl' } | ForEach-Object { $_.FullName }
$inputHashes = @($inputFiles | Sort-Object -Unique | ForEach-Object { Get-FileHash -LiteralPath $_ -Algorithm SHA256 } | Select-Object Path, Hash)
$null = New-Item -ItemType Directory -Path $out
$null = New-Item -ItemType Directory -Path (Join-Path $out 'obj')
$receiptPath = Join-Path $out 'build-receipt.json'
$receipt = [ordered]@{
    status = 'started'; requestedSourceRevision = $SourceRevision
    reviewedManifestRevision = $manifest.reviewedSourceRevision
    sourceRevisionVerifiedByScript = $false; sourceRoot = $root
    architecture = 'x64'; configuration = 'Release'; floatingPoint = '/fp:strict'
    runtime = '/MD'; stackReserveBytes = 8388608; compiler = $compilerPath; linker = $linkerPath
    inputHashes = $inputHashes; compilerIncludeEnvironment = $env:INCLUDE
    linkerLibraryEnvironment = $env:LIB; sourceCount = $sources.Count
    collectSourceErrors = [bool]$CollectSourceErrors; compileFailures = @(); linkAttempted = $false
    acceptance = 'not run'; runtimeDependencyClosureVerified = $false
    distributionReady = $false; crtPrerequisite = 'Approved x64 Visual C++ /MD runtime; not installed or verified'
    packagingEntry = 'Package-Interactive50DU.ps1; separate approved inventory required'
    sdlRuntimeStaged = ($runtimeDlls.Count -gt 0); zlib = 'six vendored inflate C sources, same /MD runtime'
    ffmpegHeaders = 'existing vendored FFmpeg 7.1.5 include tree and UPSTREAM.json, hashed as inputs'
    ffmpegRuntimeStaged = $false
}
$receipt | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $receiptPath -Encoding UTF8
$guard = Join-Path $out 'native-x64-guard.h'
@'
#if !defined(_MSC_VER) || !defined(_M_X64) || defined(_M_ARM64EC)
#error This entry requires native MSVC AMD64; x86 and ARM64 are unsupported.
#endif
#ifdef __cplusplus
#include <limits>
static_assert(sizeof(void*) == 8 && sizeof(float) == 4 &&
    std::numeric_limits<float>::is_iec559 && std::numeric_limits<float>::digits == 24,
    "Requires native x64 and IEEE binary32");
#endif
'@ | Set-Content -LiteralPath $guard -Encoding ASCII
$common = @('/nologo', '/c', '/std:c++20', '/EHsc', '/O2', '/MD', '/fp:strict', '/bigobj', '/utf-8', '/Zc:__cplusplus', '/DNDEBUG', '/DNOMINMAX', '/DWIN32_LEAN_AND_MEAN', ('/FI' + (RspQuote $guard)))
$common += $includes | ForEach-Object { '/I' + (RspQuote $_) }
$cCommon = @('/nologo', '/c', '/TC', '/O2', '/MD', '/fp:strict', '/utf-8', '/DNDEBUG', '/D_CRT_SECURE_NO_WARNINGS', ('/FI' + (RspQuote $guard)))
$cCommon += $includes | ForEach-Object { '/I' + (RspQuote $_) }
$objects = New-Object 'string[]' $sources.Count
$workItems = New-Object 'Collections.Generic.List[object]'
$active = New-Object 'Collections.Generic.List[object]'
$compileProgress = [ordered]@{
    status = 'compiling'; maxCompileJobs = $MaxCompileJobs; sourceCount = $sources.Count
    startedSourceCount = 0; completedSourceCount = 0; failedSourceCount = 0
    cancelledSourceCount = 0; runningSourceCount = 0; notStartedSourceCount = $sources.Count
}
$receipt['compilation'] = $compileProgress
function WriteCompileProgress {
    foreach ($key in @('startedSourceCount', 'completedSourceCount', 'failedSourceCount', 'cancelledSourceCount', 'runningSourceCount')) {
        $compileProgress[$key] = 0
    }
    $compileProgress.notStartedSourceCount = $sources.Count - $workItems.Count
    foreach ($work in $workItems) {
        if ($work.Started) { ++$compileProgress.startedSourceCount }
        switch ($work.State) {
            'completed' { ++$compileProgress.completedSourceCount }
            'failed' { ++$compileProgress.failedSourceCount }
            'cancelled' { ++$compileProgress.cancelledSourceCount }
            'running' { ++$compileProgress.runningSourceCount }
        }
    }
    # Ordinary stdout only: no progress file, replacement, temporary file or retry.
    # Calls are bounded by source transitions (at most 2 * sourceCount + 3).
    # The final build receipt uses the authoritative in-memory counters above.
    Write-Output ('compile-progress ' + ($compileProgress | ConvertTo-Json -Depth 4 -Compress))
}
function RecordCompileExit($Work) {
    $Work.ExitCode = $Work.Process.ExitCode
    if ($Work.State -ne 'cancelled') {
        $Work.State = 'failed'
        if ($Work.ExitCode -eq 0 -and (Test-Path -LiteralPath $Work.Object -PathType Leaf)) {
            $Work.State = 'completed'
        }
    }
}
function TestOrdinarySourceFailure($Work) {
    # Only source diagnostics are collectable. Fatal/resource/toolchain/unknown
    # failures remain immediate aborts. Large/undecodable logs fail closed.
    if ($Work.ExitCode -notin @(1, 2)) { return $false }
    $text = ''
    foreach ($path in @($Work.StdoutLog, $Work.StderrLog)) {
        if ((Get-Item -LiteralPath $path).Length -gt 16MB) { return $false }
        $text += [IO.File]::ReadAllText($path) + "`n"
    }
    if ($text -match '(?i)fatal error|internal compiler error|out of memory|cannot allocate|memory exhausted|disk full|not enough space|permission denied|access is denied|error C3859\b') { return $false }
    $errors = @($text -split "`r?`n" | Where-Object { $_ -match '(?i)\berror\b' })
    if ($errors.Count -eq 0) { return $false }
    foreach ($line in $errors) {
        if ($line -notmatch '^.+\([0-9]+(?:,[0-9]+)?\)\s*:\s*error C[234][0-9]{3}\s*:') { return $false }
    }
    return $true
}
function CloseCompileLogs($Work) {
    # Both pipe copies run concurrently into separate files, without buffering
    # an entire compiler's diagnostics or calling PowerShell on worker threads.
    try {
        if ($null -ne $Work.OutputTask) { $Work.OutputTask.GetAwaiter().GetResult() }
    } finally {
        try {
            if ($null -ne $Work.ErrorTask) { $Work.ErrorTask.GetAwaiter().GetResult() }
        } finally {
            try {
                if ($null -ne $Work.OutputStream) { $Work.OutputStream.Dispose() }
            } finally {
                if ($null -ne $Work.ErrorStream) { $Work.ErrorStream.Dispose() }
            }
        }
    }
}
try {
    WriteCompileProgress
    $next = 0
    while ($next -lt $sources.Count -or $active.Count -gt 0) {
        # Observe every exited compiler before launching another source. Only
        # explicitly opted-in ordinary source errors permit later admissions.
        foreach ($work in $active.ToArray()) {
            if (-not $work.Process.HasExited) { continue }
            $work.Process.WaitForExit()
            RecordCompileExit $work
            CloseCompileLogs $work
            $work.Process.Dispose()
            $work.Process = $null
            $null = $active.Remove($work)
            WriteCompileProgress
            if ($work.State -eq 'failed') {
                $ordinary = TestOrdinarySourceFailure $work
                $receipt.compileFailures += [ordered]@{ source = $work.Source; exitCode = $work.ExitCode; ordinarySourceError = $ordinary; stdoutLog = $work.StdoutLog; stderrLog = $work.StderrLog }
                if (-not $CollectSourceErrors -or -not $ordinary) {
                    throw "Compilation failed (exit $($work.ExitCode), object required): $($work.Source)"
                }
                Write-Warning "Collecting source failure; link will be skipped: $($work.Source)"
            }
        }
        if ($next -ge $sources.Count -or $active.Count -ge $MaxCompileJobs) {
            if ($active.Count -gt 0) { Start-Sleep -Milliseconds 50 }
            continue
        }
        if ($CollectSourceErrors) {
            $drive = [IO.DriveInfo]::new([IO.Path]::GetPathRoot($out))
            if ($drive.AvailableFreeSpace -lt 256MB) { throw 'Insufficient build disk space; stopping admissions.' }
        }
        $i = $next
        # Unique object names avoid collisions between same-name asset/game owners.
        $stem = '{0:D4}-{1}' -f $i, [IO.Path]::GetFileNameWithoutExtension($sources[$i])
        $obj = Join-Path $out ('obj/' + $stem + '.obj')
        $rsp = Join-Path $out ($stem + '.rsp')
        $flags = $common
        if ([IO.Path]::GetExtension($sources[$i]) -eq '.c') { $flags = $cCommon }
        $compileArgs = @($flags)
        $compileArgs += '/Fo' + (RspQuote $obj)
        $compileArgs += RspQuote $sources[$i]
        $compileArgs | Set-Content -LiteralPath $rsp -Encoding Unicode
        $work = [pscustomobject]@{
            Source = $sources[$i]; Object = $obj; State = 'running'; Started = $false; ExitCode = $null
            StdoutLog = (Join-Path $out ($stem + '.log')); StderrLog = (Join-Path $out ($stem + '.stderr.log'))
            Process = (New-Object Diagnostics.Process)
            OutputStream = $null; ErrorStream = $null; OutputTask = $null; ErrorTask = $null
        }
        # Register ownership BEFORE Start, including the interrupt window before
        # Started is assigned. Retain this exact Process/handle until reaped;
        # never rediscover by name/PID or kill a process tree.
        $workItems.Add($work)
        $active.Add($work)
        $work.OutputStream = [IO.File]::Open($work.StdoutLog, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        $work.ErrorStream = [IO.File]::Open($work.StderrLog, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        $work.Process.StartInfo.FileName = $compilerPath
        $work.Process.StartInfo.Arguments = '@' + (RspQuote $rsp)
        $work.Process.StartInfo.UseShellExecute = $false
        $work.Process.StartInfo.CreateNoWindow = $true
        $work.Process.StartInfo.RedirectStandardOutput = $true
        $work.Process.StartInfo.RedirectStandardError = $true
        if (-not $work.Process.Start()) { throw "Compiler did not start: $($work.Source)" }
        $work.Started = $true
        $work.OutputTask = $work.Process.StandardOutput.BaseStream.CopyToAsync($work.OutputStream)
        $work.ErrorTask = $work.Process.StandardError.BaseStream.CopyToAsync($work.ErrorStream)
        # Index by manifest order, never by nondeterministic completion order.
        $objects[$i] = $obj
        ++$next
        WriteCompileProgress
    }
    if ($compileProgress.completedSourceCount -ne $sources.Count -or $compileProgress.failedSourceCount -ne 0 -or $compileProgress.cancelledSourceCount -ne 0) {
        throw 'Refusing to link an incomplete compilation set.'
    }
    foreach ($obj in $objects) {
        if (-not (Test-Path -LiteralPath $obj -PathType Leaf)) { throw "Missing link input: $obj" }
    }
    $compileProgress.status = 'completed'
    WriteCompileProgress
    $exe = Join-Path $out 'rr_interactive_vehicle50du.exe'
    $linkRsp = Join-Path $out 'link.rsp'
    $linkArgs = @('/NOLOGO', '/MACHINE:X64', '/SUBSYSTEM:WINDOWS', '/ENTRY:mainCRTStartup', '/STACK:8388608', '/INCREMENTAL:NO', ('/OUT:' + (RspQuote $exe)))
    $linkArgs += $objects | ForEach-Object { RspQuote $_ }
    $linkArgs += 'kernel32.lib'
    $linkArgs += 'user32.lib'
    $linkArgs | Set-Content -LiteralPath $linkRsp -Encoding Unicode
    $receipt.linkAttempted = $true
    & $linkerPath ('@' + $linkRsp) 2>&1 | Tee-Object -FilePath (Join-Path $out 'link.log')
    if ($LASTEXITCODE -ne 0) { throw "Link failed ($LASTEXITCODE)." }
    RequireAmd64PE $exe
    foreach ($input in $inputHashes) {
        if ((Get-FileHash -LiteralPath $input.Path -Algorithm SHA256).Hash -ne $input.Hash) { throw "Input changed during build: $($input.Path)" }
    }
    foreach ($dll in $runtimeDlls) { Copy-Item -LiteralPath $dll -Destination $out }
    $receipt.status = 'built-not-run'
    $receipt['executable'] = $exe
    $receipt['executableSha256'] = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash
    $receipt['runtimeFiles'] = @($runtimeDlls | ForEach-Object {
        $staged = Join-Path $out ([IO.Path]::GetFileName($_))
        [ordered]@{ name = [IO.Path]::GetFileName($_); sha256 = (Get-FileHash -LiteralPath $staged -Algorithm SHA256).Hash }
    })
    if ($runtimeDlls.Count -eq 0) { Write-Warning 'SDL2.dll was not supplied. Build-only output is not runnable until the approved x64 runtime is staged.' }
    Write-Host "Built only: $exe. No tests or client execution performed; acceptance remains pending."
} catch {
    $receipt.status = 'failed'
    $receipt['error'] = $_.Exception.Message
    if ($compileProgress.status -eq 'compiling') { $compileProgress.status = 'failed' }
    throw
} finally {
    # PowerShell's normal exception/Ctrl+C unwinding reaches this block. Hard
    # termination of the PowerShell host cannot run script-level cleanup.
    if ($receipt.status -eq 'started') {
        $receipt.status = 'interrupted'
        $receipt['error'] = 'Build interrupted before completion.'
    }
    if ($compileProgress.status -eq 'compiling') { $compileProgress.status = 'interrupted' }
    $cleanupErrors = New-Object 'Collections.Generic.List[string]'
    foreach ($work in $active.ToArray()) {
        $reaped = $false
        try {
            if ($null -eq $work.Process) { continue }
            try {
                $exited = $work.Process.HasExited
                $work.Started = $true
            } catch [InvalidOperationException] {
                # Start failed or was interrupted before a process was created.
                $work.State = 'failed'
                $reaped = $true
                continue
            }
            if (-not $exited) {
                try {
                    $work.Process.Kill()
                    $work.State = 'cancelled'
                } catch [InvalidOperationException] {
                    if (-not $work.Process.HasExited) { throw }
                }
            }
            $work.Process.WaitForExit()
            $reaped = $true
            RecordCompileExit $work
        } catch {
            $cleanupErrors.Add("$($work.Source): $($_.Exception.Message)")
        } finally {
            try {
                if ($reaped) { CloseCompileLogs $work }
            } catch {
                $cleanupErrors.Add("$($work.Source) logs: $($_.Exception.Message)")
            } finally {
                try {
                    if ($null -ne $work.Process) { $work.Process.Dispose() }
                } catch {
                    $cleanupErrors.Add("$($work.Source) handle: $($_.Exception.Message)")
                }
            }
        }
    }
    if ($cleanupErrors.Count -gt 0) {
        $receipt.status = 'failed'
        $receipt['cleanupErrors'] = $cleanupErrors.ToArray()
    }
    $receipt['compileResults'] = @($workItems | ForEach-Object {
        [ordered]@{ source = $_.Source; state = $_.State; exitCode = $_.ExitCode; stdoutLog = $_.StdoutLog; stderrLog = $_.StderrLog }
    })
    try { WriteCompileProgress } finally {
        $receipt | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $receiptPath -Encoding UTF8
    }
    if ($cleanupErrors.Count -gt 0) { throw 'Owned compiler cleanup/logging failed; see build-receipt.json.' }
}

