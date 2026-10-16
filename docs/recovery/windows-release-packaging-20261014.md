# Windows release packaging contract — 2026-10-14

Source baseline: `79fc223eda2da3d98929c9f3b1253745d17e7c2a`.
Implementation only; no packaging, copying, probing, build or runtime acceptance
was performed by this worker. Parent owns integration and all later validation.

## Roles and layout

`Build-Interactive50DU.ps1` builds only. Optional SDL staging does not complete a
distribution. `Package-Interactive50DU.ps1` stages only approved existing files,
without executing native binaries, extracting archives or generating caches.
`Launch-Interactive50DU.ps1` is a separate manual one-process entry.

The package has `original/rr_interactive_vehicle50du.exe`, `original/SDL2.dll`,
original SLUS/UI/AUDIO/pod/track files under their original relative names,
`generated/evidence/`, `generated/work/`, and optional `rider/<cache>/`.
SLUS remains authenticated data, never executable guest code. Historical generated
`work/native/project/disc` copies are rejected. Native host CPU/desktop GPU only.
Every admitted driving controller, including player/camera slot 0, remains original
in-game AI; both audio profiles fix ALL8 and disable host autodrive. No simulated
input, temporary course driver or historical autodrive flags are included.

## Manifest schema 1

The runtime owner supplies JSON with every field below. `riderCache` must be null
or a relative directory within the rider domain. Paths use forward slashes and
conservative letters/digits/underscore/hyphen/dot components; no traversal, device
names, streams, links or wildcard expansion. Sources are absolute nonempty files.
Source paths, including `-Manifest` and `buildReceipt.source`, must be fully
qualified ordinary Windows drive paths (`C:\approved\file` or
`C:/approved/file`) or UNC file paths (`\\server\share\directory\file`).
Drive-relative `C:foo`, current-drive-rooted `\foo` or `/foo`, relative paths,
provider-qualified paths and device namespaces such as `\\?\` and `\\.\` are
rejected before lookup. UNC server/share components must be nonempty ordinary
names, not `.` or `..`. Windows PowerShell 5.1 on .NET Framework and PowerShell 7
on Windows use the same explicit root-shape check: `Path.IsPathFullyQualified`
is not available on the former, and `Path.IsPathRooted` alone is insufficient.
Forward slashes are normalized for this check; existing filesystem/provider,
nonempty-file and reparse-ancestor checks still apply. No existence or identity
claim follows from the root-shape check alone.

The following is a schematic fragment, **not a usable or complete manifest**:

```json
{
  "schema": 1,
  "sourceRevision": "79fc223eda2da3d98929c9f3b1253745d17e7c2a",
  "approvalReference": "parent approval artifact identifying this inventory",
  "assetAuthenticationReference": "original archive and prepared-cache authentication evidence",
  "runtimeClosureReference": "owner-reviewed dependency and supported-destination inventory",
  "crtPrerequisite": "approved installed x64 Visual C++ /MD runtime, version and provenance",
  "sdlProvenance": "approved official SDL2 distribution identity and approval reference",
  "sdlVersion": "approved SDL2 version >= 2.0.22; not SDL3",
  "audioProfile": "muted",
  "corpus": "evidence/staticType2Producer49X/original-word/native-fixtures.json",
  "riderCache": null,
  "buildReceipt": { "source": "C:/approved/build/build-receipt.json", "sha256": "64 hexadecimal digits" },
  "files": [
    { "domain": "runtime", "path": "rr_interactive_vehicle50du.exe", "source": "C:/approved/build/rr_interactive_vehicle50du.exe", "sha256": "64 hexadecimal digits", "provenance": "approved native build" },
    { "domain": "runtime", "path": "SDL2.dll", "source": "C:/approved/runtime/SDL2.dll", "sha256": "64 hexadecimal digits", "provenance": "approved x64 SDL2 runtime" },
    { "domain": "original", "path": "SLUS_202.68", "source": "C:/approved/disc/SLUS_202.68", "sha256": "64 hexadecimal digits", "provenance": "authentic original; authentication evidence reference" }
  ]
}
```

Enumerate **all** original UI/AUDIO/pod/track archives, generated corpus/roster/rig/
model/camera/track inputs and Rider image files required by the declared supported
menu/rider/course scope. Do not infer a full-game closure from the minimum gates.
The packager requires Main archives, evidence/work files and the named corpus;
Rider selection adds Rider/Course archives and AUDIO. Enabled audio also requires
AUDIO without Rider. These are minimum presence gates, not an exhaustive asset
closure algorithm. The owner must authenticate originals and caches using existing
owners before approving the manifest. No hashes are invented or learned as trust
anchors by the packager. File hashes preserve approved bytes; supplied authentication
references are attestations, not independently verified by this script.

`runtime` entries allow the fixed native EXE and explicit adjacent DLLs. Include
any approved redistributable app-local dependencies with provenance, or identify
installed prerequisites in `crtPrerequisite` and `runtimeClosureReference`.
No runtime is downloaded or installed; the script does not inspect imports,
validate SDL version/ABI, certify CRT closure or discover dependencies via PATH.
SDL may come from a separately approved runtime even if omitted at build time.
The executable hash and requested source revision must match the hashed successful
`built-not-run` build receipt. Revision strings still do not attest Git cleanliness.

## Staging and manual launch

After separate authorization, invoke:

```powershell
& .\platform\windows\native\Package-Interactive50DU.ps1 `
    -Manifest 'C:\approved\release-manifest.json' `
    -Destination 'C:\releases\new-release'
```

Destination must be new with an existing parent, separate from every copied
source directory. After assembling the complete copy list and before creating
output, preflight rejects a destination equal to or beneath the directory of any
inventory file, the manifest, the build receipt, the copied contract, launcher or tester
guide. Comparison is case-insensitive with a directory-separator boundary (so a
sibling such as `inputs-other` is not treated as a child of `inputs`); drive/share
roots retain that boundary. Existing no-overwrite and reparse checks remain in
force. This is lexical directory separation, not a new filesystem-identity proof.
Preflight validates declared hashes and path collisions before creating output.
Staging rechecks source hashes and hashes copied files. No overwrites, deletion,
archive compression, network operations, process launches or automatic retries.
Failures after destination creation retain the partial tree with a failed receipt
when receipt I/O is possible. If cancellation reaches finally with status still
`staging`, the receipt becomes `interrupted`. Hard termination or disk failure may prevent receipt
completion; only `staged-not-run` is a finished staging status.

Output includes the exact input `package-manifest.json`, `build-receipt.json`,
launcher, `launch-parameters.json`, and `package-receipt.json`. The unchanged tester
guide is staged at `platform/windows/native/README-tester50du.md`, and this contract
at `docs/recovery/windows-release-packaging-20261014.md`. That layout preserves the
guide's `../../../docs/recovery/windows-release-packaging-20261014.md` link within
the package. The launcher remains at package root; use the package launch example
below. Other historical/repository links in the unchanged guide are source-context
references, not a promise to include the full repository. Both packaged documents
are hashed inventory entries and participate in destination separation.
On `staged-not-run`, `inventoryComplete=true` describes the declared staged file
inventory only, excluding the receipt; runtime closure remains unverified.
Failed or interrupted receipts explicitly set `inventoryComplete=false` and an
`inventoryScope` warning: copy/hash failure or cancellation can leave partial or
unlisted retained files. The recorded entries are not an exhaustive directory
inventory in those states and must never be treated as closure.
For completed staging, the receipt inventories files with relative
path, domain, SHA256, size and provenance. It records manifest identity, supplied
approval/authentication/closure references, SDL/CRT prerequisites, audio selection,
ALL8 and disabled host driver. It explicitly leaves archive authentication,
runtime closure verification and runtime acceptance unperformed. It does not hash
itself; external signing/approval, if needed, belongs to parent. Local source paths
in copied manifests/build receipts may need a separately reviewed distribution
policy before sharing; this entry does not silently rewrite provenance.

To launch only after parent approval, resolve package-relative parameters without
changing shell cwd (this example is documentation, not an instruction to run now):

```powershell
$packageRoot = 'C:\releases\new-release'
$config = Get-Content -LiteralPath (Join-Path $packageRoot 'launch-parameters.json') -Raw | ConvertFrom-Json
$arguments = @{}
foreach ($property in $config.PSObject.Properties) {
    if ($property.Name -eq 'AudioProfile') { $arguments[$property.Name] = $property.Value }
    else { $arguments[$property.Name] = Join-Path $packageRoot $property.Value }
}
& (Join-Path $packageRoot 'Launch-Interactive50DU.ps1') @arguments
```

Manifest audio selection is explicit; muted is the launcher default. Enabled
requests frontend and race/device output together from original/AUDIO. Neither
profile changes controller ownership or authenticates a package receipt. Native
archive/cache authentication and Course admission remain authoritative. PE header
checks reject obvious EXE/DLL architecture/type mismatches, not ABI/provenance errors.
No exit code, inventory, hash or receipt constitutes original_main_ready, correct
rendering, audio readiness, menu acceptance, Course completion or race acceptance.

## Manual release profile versus the 602643e Gasgano/SA smoke

The historical source baseline above describes the packaging implementation.
The completed smoke discussed here identifies native source
`602643e80fa69f57272e6b45bf88b69e98588a90`; it does not replace that baseline or
certify this packager. This section documents dependencies and evidence scope,
without changing the manifest schema, launcher, packager or native owners.

The manual launcher starts Original Main with initial `RR_TRACK=TA`, explicit
mute by default, ALL8 original AI and `RR_AUTODRIVE=0`. It removes inherited
`RR_` settings before applying its fixed profile
(`platform/windows/native/Launch-Interactive50DU.ps1:114-143`). TA is the initial
setting, not a claim that native Course can admit only TA. The launcher provides
no Gasgano diagnostic profile or automated UI route.

The completed smoke used `run-window-capture-602643e-attempt1.py` in the central
Windows owner directory. Lines 14-15 select `codex-local/sa-runtime` and its
`evidence/staticType2Producer49X/original-word/native-fixtures.json`; line 36
sets `RR_TRACK=SA`, `RR_ORIGINAL_TEST_PROFILE=gasgano`, the prepared Rider cache,
muted output, ALL8 and disabled autodrive, together with the Windows capture/FPS
route. Line 69 starts the executable directly, bypassing the manual launcher and
packager. Native startup requires the complete guarded Windows evidence route
for this test profile (`tests/native/original_ui_main_entry50kg.h:212-230`).
Supplying its environment to the manual launcher cannot reproduce the smoke.
Do not remove those guards, turn this profile into a release unlock, or infer
normal fresh-profile Gasgano availability from diagnostic success.

### Rider cache tied to the matching binary

For this smoke, the supplied cache is
`codex-local/redrocket-20261013/gasgano-preparation-20261015-01/converted/evidence/ui-rider-ipum50kg`.
Its sibling preparation evidence `assembly-receipt.json` records five retained
base mips plus forty additional mips, with per-file size/SHA256/provenance. The
cache's `unlocked-manifest.json` records conversion provenance. The assembly
receipt identifies preparation source `d38322ed63fdf1fe2c3d66201183f4b5e8e7b0fb`,
not the later smoke's executable revision. Keep both identities distinct.

The exact 45 image filenames are each of these nine stems followed by
`-mip0.rrtex`, `-mip1.rrtex`, `-mip2.rrtex`, `-mip3.rrtex`, and `-mip4.rrtex`:

- Retained base: `19c_1`.
- Additional catalog: `04c_1`, `04c_2`, `10c_1`, `10c_2`, `12c_1`, `15c_1`,
  `15c_2`, `07c_2`.

The current source's base filenames appear in
`src/assets/original_ui_ipum_texture_cache50kg.cpp:23-29`; the eight additional
records appear in `original_ui_ipum_texture_cache50kg_additional_generated.inc:3-108`
in the same directory. `12c_4` has an original source pin but is not one of these
eight compiled additional records; do not derive a required output list merely
from the larger source-pin table. Native Rider loading reads/authenticates the
base and every compiled additional catalog, not just the selected rider
(`tests/native/original_ui_main_entry50kg.h:584-627`). A five-file Gasgano subset
or merely nonempty cache directory is insufficient for this binary.

A release manifest claiming this same cache scope must enumerate those exact
files under its chosen Rider-cache path, with owner-approved hashes matching the
binary's retained catalogs and original UIRIDER authentication. Carry the assembly
and conversion evidence as provenance references; the JSON provenance documents
are not substitutes for the image bytes and are not read by the cited runtime
mip-loading loop. This documentation does not independently rehash the cache,
approve redistribution or approve an unknown inventory. Hardware pixel parity
remains unverified in the assembly receipt. A different binary/catalog requires
a fresh owner review rather than assuming the number 45 is a permanent gate.

### SA and original field dependencies

The concrete source anchors below identify requirements to include in the
owner-reviewed manifest; they are not an exhaustive approved runtime closure:

- Original `TRACKS/SA.RES`; generated
  `evidence/visualSA/export-placed-normal/sa-world.rrmesh`, its
  `export-report.json`, referenced visual assets and `evidence/skySA` inputs.
  The SA descriptor also selects authored `tracks\\msa.spl`, `tracks\\asa.spl`
  and `tracks\\sa.tun` members; these are archive-member identities, not an
  instruction to extract or fabricate loose originals
  (`tests/native/track_session_assets50ig_fixture.h:130-138`).
- Generated `evidence/originalRoster50HW/raw-imports/roster.json`, each selected
  pod's `pod.json`, `import.json` and referenced imported resources, together
  with authentic `PODS/<POD>/<POD>.RES`. The original field identities are POD01,
  POD03, POD04, POD07, POD08, POD09, POD10 and POD11; Gasgano remains POD07 even
  when followed in slot 0 (`src/game/original_racer_selection50ij.h:18-27`;
  `tests/native/original_course_race_targets50kx.h:100-118`;
  `tests/native/native_racer_rig50hw_fixture.h:103-123`).
- The shared `native-fixtures.json` query/bootstrap corpus (not SA terrain), UI/Rider/Course originals, full Rider cache, prepared
  rig/model/camera inputs and authentic AUDIO required by native logical audio
  ownership even when output is muted. Package minimum presence gates do not
  enumerate all transitive resources or authenticate their compatibility. Real SA geometry comes from the SA archive and visual assets.

Retain original/generated path separation and existing native authentication,
Course admission, AI/physics/collision/handling/fixed-step/lifecycle semantics.
Do not substitute TA data for missing SA inputs or modify those owners to make
an incomplete package appear ready. Additional dependency filenames and hashes
must come from the runtime owner's approved inventory, not this partial list.

### Evidence boundary

The existing `windows-capture-602643e-attempt1/frames/window-evidence-summary.json`
reports completed/cleaned Windows evidence. Its `track-fps-baseline.json` reports
SA, followed rider 7, eight original AI, zero humans/host-driver calls/controller
samples/keyboard packets, logical audio enabled and audible output unrequested,
with 8.970563454845463 FPS over the recorded GPU-completed swap interval. That FPS
receipt has `source_revision=null` and requires external executable/build identity
correlation; the supervisor identifies the full source commit above. The smoke
uses a process-local test profile, not earned unlocks or saved-profile import
(`stderr.log:1-3`).

This is evidence of that bounded diagnostic invocation, not execution of a
staged release package or this manual launcher. It establishes no full-race,
earned-unlock, audible-audio, hardware pixel-parity or performance acceptance.
Existing `inventoryComplete` describes declared staging only; runtime closure and
acceptance remain separate. Parent retains integration and all later test slots.

## Historical boundaries and parent follow-up

`tools/package_windows49w.py` remains a historical base-dependent overlay.
`tools/package_playable50ff.py` remains a historical POD01/TA closure that excludes
runtime binaries and uses a bin layout/autodrive contract incompatible with this
entry. Neither is modified or invoked. Do not reuse its launch profile.

Parent must review and integrate this source, reconcile the stale central source
count prose with the existing manifest/generator gates (344 Windows C++ plus six C
here), obtain a complete authenticated approved inventory, review runtime/CRT/SDL
provenance and supported destinations, and perform all separately authorized
PowerShell/build/package/runtime validation. Audio owner should review enabled
frontend-to-Course transitions and Game Options Accept behavior. Source-only work
cannot prove those paths. No count/hash gate or native gameplay owner was changed.
