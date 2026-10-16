# Independent Windows packaging source review — 2026-10-14

## Scope and disposition

Reviewed the completed local packaging worker's `WORKER.patch` and
`WORKER-REPORT.md`, with the retained repository at
`8469309` and patch baseline `79fc223eda2da3d98929c9f3b1253745d17e7c2a`.
This is a source-only review. No patch application, project script execution,
build, test, syntax check, sanitizer, packaging, binary probe, or gameplay was
performed. Only this review document was written. Root owns integration and
all authorized validation.

No source-review blocker to the central native compilation was found. The
patch does not change C/C++, source manifests, generator hash gates, include
paths, compiler/linker arguments, or native dependencies. Its build-script
changes add receipt fields and hash staged runtime files after compilation.
They can affect final build receipt success if staged-runtime I/O fails, but
do not add another compile, build, native invocation, or packaging invocation.
The new packager and launcher remain separate manual entries. The retained
Windows source files were unchanged between the worker baseline and the
reviewed repository tip in the inspected Git comparison.

The round-one findings below are retained as review history. The round-two
delta addresses them at source level; see the disposition immediately below.
Neither review establishes an observed runtime result.

## Round-two correction review — 20:54 UTC

Reviewed `ROUND2.patch` and `ROUND2-REPORT.md` from the completed round-two
packaging handoff as a delta against the already reviewed round-one working
source. Repository HEAD was observed at `806a798` during this second review;
no source integration or check was performed by this reviewer.

**Disposition: ready for root source integration.** No new concrete blocking
defect was found in the correction delta. This is not Windows script/runtime
validation or approval to execute packaging or launch the client.

- The source-file guard now recognizes explicit drive-absolute and ordinary
  UNC file roots without newer .NET APIs. It rejects drive-relative and
  current-drive-rooted forms, and the manifest is checked directly before
  lookup rather than being promoted through `Resolve-Path`. UNC server/share
  checks reject dot, dot-dot, and the stated invalid namespace characters;
  device namespaces are rejected by those root checks. Existing provider,
  nonempty-file, reparse and hash gates remain.
- The destination equality/descendant predicate now covers the complete copy
  list before output creation: inventory, manifest, build receipt, launcher,
  tester guide, and the newly copied contract. Trimming and restoring one
  separator handles lexical drive/share-root boundaries and avoids confusing
  sibling names with children. This remains lexical separation, with the
  documented filesystem-alias/concurrent-change limitations.
- A finally block changes unfinished `staging` to `interrupted`. Successful
  receipts mark only their declared staged inventory complete; failed or
  interrupted receipts explicitly warn that retained partial/unlisted files
  may exist. No automatic retry or partial-tree deletion is introduced.
- The unchanged tester guide is copied to its original relative
  `platform/windows/native/` location and the contract to `docs/recovery/`,
  repairing the guide-to-contract relative link. Both documents enter hashing
  and complete-list source separation. The launcher remains at package root;
  the packaged contract gives the package-specific launch example. Other
  historical guide links remain explicitly documented source-context links.

Round two changes only `Package-Interactive50DU.ps1` and the packaging contract.
It does not alter central native compilation, the original-source owner
workflow, authentication trust bounds, default mute, ALL8 ownership, host
autodrive disablement, or native-only launch. All packaging/path/cancellation
behavior remains unexecuted. Root still owns application of round one followed
by this round-two delta, reconciliation with its current tree, and all
separately authorized script/build/package/runtime validation.

## Round-one findings and smallest corrections (resolved in round two)

### 1. Rooted is weaker than absolute for Windows source paths

`Package-Interactive50DU.ps1`, `Require-File` (patch lines 16–18), accepts
`[IO.Path]::IsPathRooted($Path)`. Windows drive-relative `C:approved/file` and
current-drive-rooted `\approved\file` satisfy rooted-path semantics without
identifying an absolute location independent of the process drive state.
Consequently the manifest can select a source through current-directory state,
contrary to its documented absolute-source contract. `Get-Item` subsequently
returns a full path and the approved hash still gates copied bytes, so this
does not bypass hash checks or constitute a demonstrated traversal exploit.

Smallest correction: reject drive-relative/current-drive-relative forms before
resolution, accepting explicit drive-rooted paths and intended UNC paths only;
retain provider/file/reparse and hash checks. Use a PowerShell 5.1-compatible
implementation rather than assuming newer .NET fully-qualified-path APIs.
Priority: low contract/path hardening, not a native-build blocker.

### 2. Input-directory separation omits packaging metadata inputs

The destination-inside-source-directory guard in the manifest-file loop
(patch line 81) covers `plan.files` only. The manifest itself, build receipt,
launcher and tester guide are appended later and never receive that guard.
Thus a new output below one of those source directories can pass even though
the staging contract says the destination is separate from input directories.
Output remains new and copies use no-overwrite semantics, so this is a
contract inconsistency rather than evidence that an input will be overwritten.

Smallest correction: apply the same destination/source-directory separation
predicate to every final entry, after appending metadata and launcher/guide
entries and before creating the destination. Preserve case-insensitive path
boundary comparison. Priority: low contract hardening.

### 3. Interrupted staging can retain an unfinished status

The packager initializes `status = 'staging'` and changes it only in the
successful body or catch. Its finally always writes the receipt, without the
build script's explicit unfinished-status handling. A PowerShell cancellation
that unwinds through finally without catch can therefore leave a receipt with
`staging`, rather than identifying interruption. It remains correctly
ineligible for completion because the contract accepts only `staged-not-run`.

Smallest correction: in finally, if status remains `staging`, set a documented
`interrupted` status and explanatory error before writing the receipt. Keep
partial trees, do not retry or delete them. This is cancellation bookkeeping;
its behavior needs separately authorized Windows validation.

## Safety and authentication assessment

- Relative output paths exclude traversal components, separators outside the
  conservative slash schema, ADS colons, wildcard characters, trailing dots,
  and ordinary DOS device names. Case-insensitive duplicate and file/directory
  prefix checks operate across runtime files remapped into `original/`.
- Existing sources and destination ancestors are checked for reparse points.
  These are preflight checks, not an atomic defense against concurrent changes
  to the filesystem. Keep the reviewed inputs and destination parent under the
  approved owner's control while staging.
- Sources are hashed in preflight and again before copying; copied targets are
  hashed against the declared approved hash. No executable, extractor, archive
  generator, downloader, or native authentication routine is invoked.
- Manifest authentication/approval/closure references are nonempty supplied
  attestations. Their contents are not verified. The build receipt must have
  `built-not-run`, matching requested full source revision and executable hash;
  it does not prove Git cleanliness, source authenticity, original-asset
  provenance, SDL ABI/version, or dependency closure. These limitations are
  disclosed in the contract and receipt. Minimum inventory gates are not a
  complete menu/rider/course asset closure algorithm.
- The two executable launch checks admit only AMD64 PE32+ with appropriate EXE
  versus DLL characteristics. That excludes SLUS execution through this entry
  but is not binary provenance validation. SLUS is copied as data and never
  assigned to ProcessStartInfo.FileName. No guest interpreter/emulator path is
  added by the patch.

## Controller and audio assessment against retained native source

The launcher clears inherited `RR_`, `SDL_`, `MESA_` and `LIBGL_` settings in
the child's environment, sets `RR_ORIGINAL_AI_ALL_RACERS=1`, and fixes
`RR_AUTODRIVE=0`. Both audio profiles preserve these values. In retained
`tests/native/terrain_interactive_drive50du.cpp`, the ALL8 branch owns every
racer slot and disables the human keyboard/controller path; native-fleet
ownership also rejects enabled host autodrive. The patch does not add simulated
inputs, temporary course drivers, or a SLUS executable target. All-original-AI
policy, including slot 0, remains intact at source level.

`AudioProfile` defaults to `muted`. Fixed settings are
`RR_ORIGINAL_MAIN_AUDIO=muted`, `RR_AUDIO=0`, `RR_AUDIO_DEVICE=0` and the original
`AUDIO` root for logical race ownership. Rider/Course still requires AUDIO when
muted. Enabled changes frontend mode/root and race/device output together,
with both roots pointing at the same original AUDIO directory. This agrees
with the explicit enabled-root requirement in
`tests/native/original_ui_main_entry50kg.h` and the original-fleet logical audio
ownership in `tests/native/terrain_interactive_drive50du.cpp`. There is no WAV
request or fallback retry. Presence and environment wiring do not prove
audibility, Options Accept behavior, frontend/race transitions, or acceptance.

## Failure and handoff boundaries

Preflight failures normally precede output creation. Copy-stage failures retain
the partial tree, rethrow, and attempt a failed receipt. Hard termination and
receipt/disk I/O failure can prevent a reliable receipt; the documentation
appropriately does not promise otherwise. A copied file that fails its target
hash check is retained but is not appended to `receipt.files`, so failed
receipt inventories must not be mistaken for complete inventories of a partial
tree. Only successful `staged-not-run` inventories have the completion claim.

The launcher does not authenticate package receipts. Human/root approval of
the exact manifest, receipt, binaries, dependencies, supported destinations
and native archive/cache authentication remains necessary before launching.
Source paths in copied manifests/receipts need review before distribution.
Existing historical package tools remain untouched and are not invoked.
In round one, the staged tester guide's repository-relative packaging link would not resolve
inside the package because the referenced contract is not copied; optionally
ship a reviewed local contract copy or make that package-facing reference
self-contained. Round two implements a copied contract and matching guide
layout, as reviewed above. This is documentation polish, not launch or build behavior.

No source changes were integrated by this reviewer, and no Windows packaging
or runtime validation claim is made.
