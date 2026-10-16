# Source-only candidate. Reads completed evidence; never runs a build or client.
#requires -Version 7.0
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$BuildReceipt,
    [Parameter(Mandatory=$true)][string]$ProcessReceipt,
    [Parameter(Mandatory=$true)][string]$EvidenceSummary,
    [Parameter(Mandatory=$true)][string]$FpsReceipt,
    [Parameter(Mandatory=$true)][string]$MainPng,
    [Parameter(Mandatory=$true)][string]$RiderPng,
    [Parameter(Mandatory=$true)][string]$CoursePng,
    [Parameter(Mandatory=$true)][string]$TrackPng,
    [Parameter(Mandatory=$true)][string]$OutputDirectory
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$culture = [Globalization.CultureInfo]::InvariantCulture
function Need($Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
function Field($Object, [string]$Name) {
    Need ($null -ne $Object -and $null -ne $Object.PSObject.Properties[$Name]) "Missing required field: $Name"
    return $Object.$Name
}
function Text($Object, [string]$Name) {
    $value = Field $Object $Name
    Need ($value -is [string] -and -not [string]::IsNullOrWhiteSpace($value)) "Expected nonempty string: $Name"
    return $value
}
function Count($Object, [string]$Name) {
    $value = Field $Object $Name
    Need (($value -is [long] -or $value -is [int]) -and $value -ge 0) "Expected nonnegative integer: $Name"
    return [long]$value
}
function Flag($Object, [string]$Name, [bool]$Expected) {
    $value = Field $Object $Name
    Need ($value -is [bool] -and $value -eq $Expected) "Unexpected flag: $Name"
}
function FilePath([string]$Path) {
    Need ($Path -match '^[A-Za-z]:[\\/]' -and [IO.Path]::IsPathFullyQualified($Path)) "Supply an absolute local drive path: $Path"
    $item = Get-Item -LiteralPath $Path
    Need ($item -is [IO.FileInfo] -and $item.Length -gt 0) "Missing or empty file: $Path"
    Need (($item.Attributes -band [IO.FileAttributes]::ReparsePoint) -eq 0) "Reparse file unsupported: $Path"
    return $item.FullName
}
function SamePath([string]$Left, [string]$Right) {
    Need ([IO.Path]::IsPathFullyQualified($Left) -and [IO.Path]::IsPathFullyQualified($Right)) 'Receipt paths must be absolute.'
    return [string]::Equals([IO.Path]::GetFullPath($Left), [IO.Path]::GetFullPath($Right), [StringComparison]::OrdinalIgnoreCase)
}
function InRun([string]$Path) {
    Need ($Path.StartsWith($runRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) "Evidence outside the process receipt's run: $Path"
    $directory = [IO.DirectoryInfo][IO.Path]::GetDirectoryName($Path)
    while ($null -ne $directory -and $directory.FullName.Length -ge $runRoot.Length) {
        Need (($directory.Attributes -band [IO.FileAttributes]::ReparsePoint) -eq 0) "Reparse directory unsupported: $($directory.FullName)"
        $directory = $directory.Parent
    }
}
$inputs = New-Object 'Collections.Generic.List[object]'
function JsonFile([string]$Path, [long]$MaximumBytes = 1048576, [switch]$PreserveRunTimestamps) {
    $resolved = FilePath $Path
    Need ((Get-Item -LiteralPath $resolved).Length -le $MaximumBytes) "Receipt exceeds size bound: $resolved"
    $hash = (Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash
    $jsonText = Get-Content -LiteralPath $resolved -Raw -Encoding UTF8
    $document = $jsonText | ConvertFrom-Json
    if ($PreserveRunTimestamps) {
        # Default ConvertFrom-Json can materialize dates. DateKind String needs
        # PowerShell 7.5; this DOM is included in PowerShell 7.0's .NET Core 3.1.
        $jsonDom = [System.Text.Json.JsonDocument]::Parse($jsonText, [System.Text.Json.JsonDocumentOptions]::new())
        try {
            foreach ($timestampName in @('startedUtc', 'endedUtc')) {
                $timestampToken = $jsonDom.RootElement.GetProperty($timestampName)
                Need ($timestampToken.ValueKind -eq [System.Text.Json.JsonValueKind]::String) "Expected JSON timestamp string: $timestampName"
                $document.$timestampName = $timestampToken.GetString()
            }
        } finally { $jsonDom.Dispose() }
    }
    $inputs.Add([pscustomobject]@{path=$resolved; sha256=$hash})
    return $document
}
function Html($Value) { return [Net.WebUtility]::HtmlEncode([string]$Value) }
function RelativeLink([string]$Path) {
    $relative = [IO.Path]::GetRelativePath($out, $Path)
    Need (-not [IO.Path]::IsPathRooted($relative)) 'Evidence must share the output volume.'
    return (($relative -split '[\\/]' | ForEach-Object { [Uri]::EscapeDataString($_) }) -join '/')
}
function Row([string]$Label, $Value) { return '<tr><th>' + (Html $Label) + '</th><td>' + (Html $Value) + '</td></tr>' }
function Stamp([string]$Value) {
    Need ($Value -match '(Z|[+-]\d\d:\d\d)$') 'Run timestamps must contain an explicit UTC offset.'
    return [DateTimeOffset]::Parse($Value, $culture, [Globalization.DateTimeStyles]::None)
}
$ProcessReceipt = FilePath $ProcessReceipt
$runRoot = [IO.Path]::GetDirectoryName($ProcessReceipt)
$out = [IO.Path]::GetFullPath($OutputDirectory)
Need ([IO.Path]::IsPathFullyQualified($OutputDirectory)) 'OutputDirectory must be absolute.'
Need (SamePath ([IO.Path]::GetDirectoryName($out)) $runRoot) 'OutputDirectory must be a new direct child of this run directory.'
Need (-not (Test-Path -LiteralPath $out)) 'OutputDirectory already exists; use a new run-specific directory.'
$process = JsonFile $ProcessReceipt -PreserveRunTimestamps
$build = JsonFile $BuildReceipt 16777216
$EvidenceSummary = FilePath $EvidenceSummary
$FpsReceipt = FilePath $FpsReceipt
InRun $ProcessReceipt
InRun $EvidenceSummary
InRun $FpsReceipt
$summary = JsonFile $EvidenceSummary
$fps = JsonFile $FpsReceipt
Need ((Text $build 'status') -ceq 'built-not-run') 'Build receipt is not built-not-run.'
$source = Text $process 'source'
Need ($source -match '^[0-9a-fA-F]{40}$' -and $source -ieq (Text $build 'requestedSourceRevision')) 'Source revision mismatch or invalid full commit.'
$exeHash = Text $build 'executableSha256'
Need ($exeHash -match '^[0-9a-fA-F]{64}$') 'Invalid executable SHA256.'
$command = @(Field $process 'command')
Need ($command.Count -gt 0 -and $command[0] -is [string]) 'Missing executable command.'
$placements = @((Field $process 'placements') | Where-Object { SamePath (Text $_ 'path') $command[0] })
Need ($placements.Count -eq 1 -and (Text $placements[0] 'sha256') -ieq $exeHash) 'Run placement does not match build executable hash.'
Need ([IO.Path]::GetFileName($command[0]) -ceq [IO.Path]::GetFileName((Text $build 'executable'))) 'Executable name mismatch.'
Need ((Count $process 'exitCode') -eq 0 -and (Count $process 'remainingOwnedProcesses') -eq 0) 'Run did not exit cleanly.'
Flag $process 'forcedTermination' $false
Flag $process 'cleanupConfirmed' $true
Flag $process 'headless' $false
Need ($null -eq (Field $process 'error')) 'Run receipt contains an error.'
if ($null -ne $process.PSObject.Properties['cleanupError']) { Need ($null -eq $process.cleanupError) 'Run contains a cleanup error.' }
$start = Stamp (Text $process 'startedUtc')
$end = Stamp (Text $process 'endedUtc')
Need ($end -gt $start) 'Invalid completed run time interval.'
$eastern = [TimeZoneInfo]::FindSystemTimeZoneById('Eastern Standard Time')
$startEastern = [TimeZoneInfo]::ConvertTime($start, $eastern).ToString('yyyy-MM-dd HH:mm:ss.fffffff zzz', $culture)
$endEastern = [TimeZoneInfo]::ConvertTime($end, $eastern).ToString('yyyy-MM-dd HH:mm:ss.fffffff zzz', $culture)
$captureRoot = Text (Field $process 'environment') 'RR_WINDOW_CAPTURE_DIR'
Need ([IO.Path]::IsPathFullyQualified($captureRoot)) 'Recorded capture directory must be absolute.'
Need (SamePath ([IO.Path]::GetDirectoryName($EvidenceSummary)) $captureRoot) 'Summary is not in the recorded capture directory.'
Need (SamePath (Text $summary 'baseline_file') $FpsReceipt) 'Summary/FPS receipt mismatch.'
Flag $summary 'completed' $true
Flag $summary 'cleanup_complete' $true
Flag $summary 'timed_out' $false
Need ((Text $summary 'evidence_mode') -ceq 'unattended_windows') 'Unsupported summary mode.'
Need ((Text $fps 'schema') -ceq 'native-windows-track-fps-v1' -and (Text $fps 'mode') -ceq 'unattended_windows') 'Unsupported FPS receipt.'
$embeddedSource = Field $fps 'source_revision'
Need ($null -eq $embeddedSource -or $embeddedSource -ieq $source) 'FPS source revision mismatch.'
$begin = Count $fps 'interval_begin_ns'; $finish = Count $fps 'interval_end_ns'; $elapsed = Count $fps 'elapsed_ns'
$swaps = Count $fps 'actual_completed_swap_submissions'
$beginSwaps = Count $fps 'begin_window_swaps'; $endSwaps = Count $fps 'end_window_swaps'
$beginTicks = Count $fps 'begin_native_tick'; $endTicks = Count $fps 'end_native_tick'
Need ($finish -gt $begin -and $elapsed -eq ($finish - $begin) -and $elapsed -ge (Count $fps 'measurement_required_ns')) 'Inconsistent FPS interval.'
Need ($swaps -gt 0 -and $swaps -eq ($endSwaps - $beginSwaps) -and $endTicks -gt $beginTicks) 'Inconsistent FPS swap/tick counters.'
$rate = Field $fps 'fps'
Need (($rate -is [double] -or $rate -is [long] -or $rate -is [int]) -and [double]::IsFinite([double]$rate) -and $rate -gt 0) 'Invalid FPS value.'
Need ([Math]::Abs([double]$rate - ($swaps * 1e9 / $elapsed)) -le 0.000001) 'FPS differs from recorded swaps/elapsed interval.'
$ai = Count $fps 'original_ai_count'; $human = Count $fps 'human_count'; $racers = Count $fps 'racer_count'
Flag $fps 'all_original_ai' $true
Need ($ai -eq $racers -and $racers -eq 8 -and $human -eq 0) 'Original-AI provenance mismatch.'
foreach ($key in @('host_driver_calls','keyboard_packets','controller_samples')) { Need ((Count $fps $key) -eq 0) "Unexpected input counter: $key" }
Flag $fps 'capture_readback_and_disk_intervals_discarded' $true
$cards = New-Object 'Collections.Generic.List[string]'
$frames = @{}
$pngs = [ordered]@{main=$MainPng; rider=$RiderPng; course=$CoursePng; track=$TrackPng}
$labels = @{main='Main'; rider='Rider / character'; course='Course'; track='Track — starting-grid screenshot (early race)'}
foreach ($stage in $pngs.Keys) {
    $entry = Field $summary $stage
    $receiptPath = FilePath (Text $entry 'receipt_file')
    InRun $receiptPath
    $frame = JsonFile $receiptPath
    $imagePath = Text $entry 'image_file'
    Need ([IO.Path]::IsPathFullyQualified($imagePath) -and (SamePath $imagePath (Text $frame 'image_file'))) "Image/receipt mismatch: $stage"
    Need (SamePath $receiptPath ([IO.Path]::ChangeExtension($imagePath, '.json'))) "Receipt stem mismatch: $stage"
    Need ((Text $frame 'stage') -ceq $stage -and (Text $frame 'evidence_mode') -ceq 'unattended_windows') "Frame stage mismatch: $stage"
    Flag $frame 'settled' $true
    Flag $frame 'window_swap_submitted' $true
    $png = FilePath $pngs[$stage]
    InRun $png
    Need ((Get-Item -LiteralPath $png).Length -le 33554432) "PNG exceeds 32 MiB bound: $stage"
    Need ([IO.Path]::GetExtension($png) -ieq '.png' -and (SamePath $png ([IO.Path]::ChangeExtension($imagePath, '.png')))) "PNG must be the existing same-directory/same-stem derivative: $stage"
    $hash = (Get-FileHash -LiteralPath $png -Algorithm SHA256).Hash
    $inputs.Add([pscustomobject]@{path=$png; sha256=$hash})
    $frames[$stage] = $frame
    Need ((Count $frame 'width') -gt 0 -and (Count $frame 'height') -gt 0) "Invalid recorded dimensions: $stage"
    $detail = 'Session ' + (Count $frame 'session') + '; sequence ' + (Count $frame 'sequence') + '; window swaps ' + (Count $frame 'window_swaps')
    if ($stage -eq 'track') { $detail += '; native ticks ' + (Count $frame 'world_ticks') + '; race seconds ' + (Field $frame 'race_seconds') + '; phase ' + (Count $frame 'race_phase') }
    $cards.Add('<figure><h2>' + (Html $labels[$stage]) + '</h2><img src="' + (Html (RelativeLink $png)) + '" alt="' + (Html $labels[$stage]) + '"><figcaption>' + (Html $detail) + '<br>PNG SHA256: <code>' + (Html $hash) + '</code></figcaption></figure>')
}
Need ((Count $frames.main 'session') -eq (Count $frames.rider 'session') -and (Count $frames.main 'session') -eq (Count $frames.course 'session')) 'Frontend sessions differ.'
Need ((Count $frames.main 'sequence') -lt (Count $frames.rider 'sequence') -and (Count $frames.rider 'sequence') -lt (Count $frames.course 'sequence')) 'Frontend capture sequence is inconsistent.'
Need ((Count $frames.track 'session') -gt (Count $frames.course 'session')) 'Track session must follow the Course session.'
Flag $frames.rider 'selection_attached' $true
Flag $frames.rider 'accepted_rider_source' $true
$courseSelection = Field $frames.course 'course_selection'
$trackSelection = Field $frames.track 'course_selection'
foreach ($selectionField in @('course','planet','laps','entrants')) {
    Need ((Count $courseSelection $selectionField) -eq (Count $trackSelection $selectionField)) "Course/track selection mismatch: $selectionField"
}
Need ((Count $frames.rider 'selected_rider') -eq (Count $frames.track 'selected_rider') -and (Count $trackSelection 'rider') -eq (Count $frames.track 'selected_rider')) 'Rider/track selection mismatch.'
Need ((Count $trackSelection 'entrants') -eq $racers) 'Course entrants differ from measured racer count.'
Need ((Count $frames.track 'session') -eq (Count $fps 'session') -and (Text $frames.track 'track') -ceq (Text $fps 'track')) 'Track/FPS session mismatch.'
Need ((Count $frames.track 'world_ticks') -lt $beginTicks -and (Count $frames.track 'window_swaps') -lt $beginSwaps) 'FPS must follow the track screenshot.'
Need ((Count $frames.track 'original_ai_racers') -eq $ai -and (Count $frames.track 'human_racers') -eq $human) 'Track/FPS AI counters differ.'
$raceSeconds = Field $frames.track 'race_seconds'
Need (($raceSeconds -is [double] -or $raceSeconds -is [long] -or $raceSeconds -is [int]) -and [double]::IsFinite([double]$raceSeconds) -and $raceSeconds -ge 0) 'Invalid screenshot race time.'
Need ((Count $fps 'followed_racer_slot') -lt $racers) 'Followed racer slot is outside the racer count.'
Need ((Count $frames.track 'selected_rider') -eq (Count $fps 'followed_racer60')) 'Track/FPS followed rider differs.'
$rows = @(
    (Row 'Source commit (external receipts)' $source), (Row 'Executable' $command[0]), (Row 'Executable SHA256' $exeHash),
    (Row 'Run started — Eastern' $startEastern), (Row 'Run ended — Eastern' $endEastern),
    (Row 'FPS (recorded)' $rate), (Row 'Measured interval, process-relative ns' "$begin to $finish"),
    (Row 'Measured elapsed ns' $elapsed), (Row 'Completed swap submissions' $swaps),
    (Row 'Window swap endpoints' "$beginSwaps to $endSwaps"), (Row 'Native tick endpoints' "$beginTicks to $endTicks"),
    (Row 'Native tick delta' ($endTicks - $beginTicks)), (Row 'Original AI / human / total racers' "$ai / $human / $racers")
)
foreach ($selectionField in @('course','planet','laps','entrants','rider')) {
    $rows += Row ('Correlated selection: ' + $selectionField) (Count $trackSelection $selectionField)
}
foreach ($key in @('host_driver_calls','keyboard_packets','controller_samples','followed_racer_slot','followed_racer60','warmup_excluded_ns','discarded_intervals','clock','measurement_scope','source_identity_scope')) {
    $rows += Row $key (Field $fps $key)
}
$html = @'
<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Completed Windows smoke evidence</title><style>
body{font:16px system-ui,sans-serif;max-width:1100px;margin:2rem auto;padding:0 1rem;background:#f5f6f8;color:#172132}h1,h2{line-height:1.2}table{border-collapse:collapse;width:100%;background:white}th,td{padding:.55rem;text-align:left;border-bottom:1px solid #ddd;overflow-wrap:anywhere}th{width:36%}.frames{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:1rem}figure{margin:0;padding:1rem;background:white}img{width:100%;height:auto}figcaption{font-size:.85rem;overflow-wrap:anywhere}.note{padding:1rem;background:#e8edf7}code{overflow-wrap:anywhere}
</style><body><h1>Completed Windows smoke evidence</h1>
<p class="note">The track image is the starting-grid / early-race capture, not an image of the later driving FPS interval. Capture overhead is excluded by the FPS receipt. This page indexes existing evidence; it does not establish new runtime acceptance.</p>
'@
$html += '<table>' + ($rows -join '') + '</table><p>Eastern timestamps above are the recorded run window (DST-adjusted offset shown). Frame receipts supply no individual wall-clock capture time. Do not treat index creation time or PNG file times as capture times.</p><div class="frames">' + ($cards -join '') + '</div>'
$html += '<p>Selection checks establish matching recorded fields and a later track session. The receipts have no shared UI transaction ID or Course-screen rider field; UI subtype slots are not mapped to race subtypes here.</p><p>PNG links reference existing derivatives without copying or decoding them. Their same-stem receipt linkage is checked; the receipts do not contain PNG conversion hashes. Source and executable identity are correlated from external build/run receipts, not inferred from pixels. Keep this index with its original run directory and compare index-inputs.json hashes if evidence is moved or changed.</p></body></html>'
# Detect changed inputs before publishing. No assets or executable are opened.
foreach ($entry in $inputs) { Need ((Get-FileHash -LiteralPath $entry.path -Algorithm SHA256).Hash -ceq $entry.sha256) "Input changed while indexing: $($entry.path)" }
$manifest = [ordered]@{source=$source; executableSha256=$exeHash; runStartedUtc=$start.ToString('o'); runEndedUtc=$end.ToString('o'); inputs=$inputs.ToArray()}
$staging = Join-Path $runRoot ('.smoke-index-' + [Guid]::NewGuid().ToString('N') + '.staging')
$null = New-Item -ItemType Directory -Path $staging
try {
    [IO.File]::WriteAllText((Join-Path $staging 'index-inputs.json'), ($manifest | ConvertTo-Json -Depth 5), (New-Object Text.UTF8Encoding $false))
    [IO.File]::WriteAllText((Join-Path $staging 'index.html'), $html, (New-Object Text.UTF8Encoding $false))
    # Same-parent rename publishes both complete files; an existing destination
    # is an error, never an overwrite. Keep failed staging for diagnosis.
    [IO.Directory]::Move($staging, $out)
} catch {
    throw "Index publication failed; diagnostic staging retained at ${staging}: $($_.Exception.Message)"
}
Write-Output (Join-Path $out 'index.html')
