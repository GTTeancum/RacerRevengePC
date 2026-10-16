# Local completed-smoke evidence index

`New-SmokeEvidenceIndex.ps1` is a source-only candidate for PowerShell 7 on
Windows. It has not been run or syntax-checked. Root owns integration and runtime
acceptance. It reads an already-completed smoke; it never builds, launches,
captures, converts, decodes or copies images, modifies evidence, or opens a browser.

The output is static `index.html` with embedded CSS and relative links to the
four existing PNGs, plus `index-inputs.json` recording input paths and SHA256s.
No server, JavaScript, network, package or additional software is required.
Keep the index inside its original run tree; it is not a detached image bundle.

## Central runner integration

After the smoke has ended successfully and owned-process cleanup is confirmed,
after the existing PNG conversions have completed, supply these explicit inputs:

- The matching build's `build-receipt.json`.
- This run's `process-receipt.json`.
- This run's `frames/window-evidence-summary.json` and
  `frames/track-fps-baseline.json`.
- The four already-existing PNGs for Main, Rider, Course and track.
- A nonexistent output directory that is a direct child of this run directory.

Example invocation for a future, separately authorized runner integration:

```powershell
$run = 'C:\evidence\windows-capture-<commit>-attempt1'
& '<repo>\platform\windows\native\New-SmokeEvidenceIndex.ps1' `
  -BuildReceipt 'C:\evidence\windows-build-<commit>\build-receipt.json' `
  -ProcessReceipt "$run\process-receipt.json" `
  -EvidenceSummary "$run\frames\window-evidence-summary.json" `
  -FpsReceipt "$run\frames\track-fps-baseline.json" `
  -MainPng "$run\frames\frontend-session-1\main-1.png" `
  -RiderPng "$run\frames\frontend-session-1\rider-2.png" `
  -CoursePng "$run\frames\frontend-session-1\course-3.png" `
  -TrackPng "$run\frames\race-session-2\track-1.png" `
  -OutputDirectory "$run\viewer-<commit>-attempt1"
```

Use the actual run's paths and session numbers, not these placeholders. The
helper prints the resulting index path and does not open it. A failure creating
the index does not change the original build/smoke result. Both outputs are
written and closed in a unique sibling staging directory, then published by a
same-parent directory rename that refuses an existing destination. Failed staging
is retained with its path in the error; the final directory is never partially
published by these writes. There is no retry, overwrite or cleanup deletion.
No current wrapper is modified by this proposal.

## Receipt contract and freshness

The current unattended schema is required: completed/cleaned summary, successful
process exit, no forced termination/error or remaining owned processes, visible
window mode, matching full source commit and executable placement/build SHA256.
The four frame receipts are read only from the explicit paths named by the
supplied summary. They must match stages, image stems, settled/swapped flags and
session/order relationships. Each PNG must be beside and share the original
receipt's image stem. No fallback, latest-file discovery or directory scan is
performed. Evidence links must stay within the supplied process receipt's run.
Reparse files/directories within that run are rejected.

Track session must be later than Course session: the producer records its
monotonically increasing application session index. Rider selection must be
attached/source-accepted and match the track's selected rider, accepted race
selection and FPS followed rider. Course and track must agree on the recorded
course, planet, laps and entrant values, with entrants matching the FPS racer
count. These comparisons use fields emitted by the current capture producer.
The receipts do not expose a shared transaction ID or the Course screen's rider;
this is field/session consistency, not a proof of an unbroken UI transaction.
Course/subtype UI slots and racer camera slots are different domains and are not
equated or mapped by this helper. In particular, no subtype-slot-to-race-subtype
identity is inferred. The index shows the selection values it can correlate.

FPS must use `native-windows-track-fps-v1`, match the track session and followed
rider, and report a consistent elapsed interval, completed-swap delta, FPS value
and advancing native ticks. This helper targets the completed eight-original-AI
smoke: eight AI, zero humans, zero host-driver calls, keyboard packets and
controller samples. Missing, malformed or inconsistent required values fail
before index creation. Required null `source_revision` in current FPS receipts
is correlated through external build/run identities, not replaced with a guess.

Run start/end are taken from `startedUtc`/`endedUtc`, converted using Windows
`Eastern Standard Time` with its date-specific DST rules, and displayed with an
explicit offset. Frame receipts contain no individual wall-clock timestamps:
the page says so and displays the actual run window. Generation time, file
mtime, process-relative nanoseconds and race seconds are not presented as frame
wall-clock capture timestamps.

PowerShell compatibility: default `ConvertFrom-Json` can turn ISO timestamp
strings into `DateTime` values. `-DateKind String` was added only in 7.5. To retain
the 7.0 minimum, the helper reads the original `startedUtc` and `endedUtc` JSON
string tokens with `System.Text.Json` (included in .NET Core 3.1), then applies
its explicit-offset check and `DateTimeOffset` parsing. It never reconstructs
an offset from a converted `DateTime` or assumes the machine's local time zone.
The existing date-specific Eastern conversion is unchanged.

The starting-grid / initial-track PNG is labeled separately from the later
driving FPS interval. Its recorded race seconds, ticks and swaps are shown; the
FPS measurement must begin after that capture's tick/swap counters. The helper
does not infer vehicle position from pixels or claim physical monitor scanout.

Strings are HTML-encoded, relative URL segments are percent-encoded, and no
receipt text becomes script, style, markup or an absolute/network image URL.
Only the named receipts and PNGs are read; receipt sizes are bounded (1 MiB each,
16 MiB for the build receipt). Inputs are hashed again before writing output.

The native frame receipts identify PPM captures, not PNG conversion hashes.
Same-stem linkage and per-input hashes document the supplied PNGs but cannot
prove that an earlier conversion used the right pixels. The runner remains
responsible for that provenance. The executable and PPMs are not opened, and
PNG bytes are hashed without decoding. Moving or changing evidence after index
creation can invalidate links or recorded hashes; preserve the entire run tree.
