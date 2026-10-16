# Integrated menu wiring source audit — 2026-10-14

## Narrow correction

The confirmed native Results `MAIN MENU` action now returns through
`SessionExit50DU::ReturnOriginalMain` for an original-application-owned,
source-camera race. This uses exactly the admission already passed to
`HostSessionMenu50IF` for Pause's `RETURN TO ORIGINAL MAIN` destination:
`originalUiStarted && originalCameraEnabled`.

Before this correction, `terrain_interactive_drive50du.cpp` handled Results
`MainMenu` directly only when `humanCourseEnabled`. An ALL8 Course race instead
set `hostInput.menuRequested`. `HostSessionMenu50IF::poll` explicitly maps that
pulse to `Page::Pause` for its `nativeAllAiRestart_` path, so the already-confirmed
Results action reopened Pause instead of returning to the available original
Main. The frontend application can own an ALL8 Course race just as it owns the
bounded TA human Course race; `originalUiStarted` is assigned from
`originalMainApplication.enabled` at the beginning of the actual session.

The same input-release, `composePacket(false,input)` and `sampleController()`
boundary is retained. Only the existing destination-admission predicate changes.
The direct diagnostic route without the original application retains its host
menu fallback. No original retail Results, progression, save, menu event, or
profile write is invented by this native Results adapter.

## Ownership proof

- `HostRaceResults50IL::poll` requires fresh released input, opens a confirmation
  with Cancel selected, and returns `Action::MainMenu` only on an explicit
  accepted confirmation. It is a native input adapter, not an original scene.
- The actual `HostSessionMenu50IF` constructor receives `originalCameraEnabled`
  as native restart admission and `originalUiStarted && originalCameraEnabled`
  as original-Main admission. Its confirmed `Page::Leave` already returns
  `Result::ReturnOriginalMain` under the same predicate.
- `runSession50DU` returns `ReturnOriginalMain` only after its existing complete
  owned session closure. The application loop ends the native gameplay light
  lease after the local world, renderer and race resource owners are destroyed,
  then selects `runOriginalFrontend50LA` for the next session. The same Video
  object survives, with its host closed before frontend creation.
- The outer session loop accepts this return for the existing original fleet
  routes and requires `originalMainApplication.enabled` before entering Main.
  The correction neither rebuilds resources in a live race nor changes GPU,
  material, audio, application RNG, or profile lifetimes.

## Existing original Options and Controls flow

No second fix was needed in these already-integrated owners:

- `original_frontend_session50la.h` delivers original input 4 for Confirm and
  input 6 for Back to the actual coordinator. On Game Options and Controls it
  delivers four directional inputs, input 5 for defaults, and Controls input 7
  for configuration. Input remains under the existing physical-focus gate.
- `OriginalUiOptionsSession50KG::input` proposes Options Back at source
  `0x1F99A4` with the actual remembered slot store `0x1F9994`. The coordinator
  prepares and authenticates Main before accepting this proposal.
- Game Options Accept uses `describeOriginalUiGameOptionsAccept50KG` and
  `commitGameOptionsAccept`: actual process-owned profile flags, four volumes
  and sound mode are stored at their recorded source PCs; the concrete shared
  sound-mode owner applies `188740`; remembered group is committed afterward.
  Cancel returns at `0x1F51D8`, retaining the settings and only remembering the
  navigation group at `0x1F51C8`.
- Controls return is validated by `originalUiControlsReturnShape50KG` and the
  shared preset owner. Only the source Accept at `0x1F39AC` calls the canonical
  `selectControlPreset49D`; Cancel performs no preset store. The same prepared
  Options manager owns the page transition, its aperture and material resources.
- `OriginalUiFrontend50KG` validates the exact pending proposal, discharges only
  its concrete destination-owner blocker, commits through the existing transition
  owner, and then sends the source Select/Cancel sound. It does not accept
  caller-supplied destination readiness as proof.

Second-controller menu/hotplug popup, Gallery, memory-card and Credits ownership
remain the documented incomplete paths. This correction does not admit them.

## Verification boundary

Source inspection only. No build, syntax check, test, sanitizer, gameplay run,
project executable, helper execution, or commit was performed for this audit.
Runtime and visual behavior are not claimed.
