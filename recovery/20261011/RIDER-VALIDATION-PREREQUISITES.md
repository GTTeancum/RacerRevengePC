# Later root-owned Rider validation

Source baseline: activation integrated by root as d25e7ca, following renderer
retirement/clear-marker fixes through48a801f. This is a proposed root-only recipe
and observation plan. No conversion, build, parser, fixture or native run was
performed by this worker. The upcoming Options-focused slot remains separate.

## 1. Actual kind2 conversion, once authorized in the root validation window

Use the active pinned generator and existing host50FC decoder. An installed
FFmpeg exposing the native IPU decoder is required. The generator authenticates
the original PST/archive/ELF anchors, helper source and all five compressed mip
payloads; a failed identity check must be investigated, never bypassed by changing
pins or supplying fallback pixels. The output root must not already exist.

Proposed command (not executed):

```sh
python3 -B /workspace/shared/rr-recovery-git/tools/assets/recover_rider_ipum50kg.py \
  --repo /workspace/shared/rr-recovery-git \
  --source-pst /workspace/shared/recovery-20261010/ui-rider-controller-reconstruction/preview/members/ui/textures/19c_1.pst \
  --rider-archive /workspace/shared/recovery-20261010/ui-inputs/game-files/UI/UIRIDER/UIRIDER.RES \
  --elf /workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68 \
  --output-root /workspace/shared/recovery-20261010/rider-ipum-generated-01 \
  --execute-cold-conversion
```

If that output path is already occupied at execution time, select a new retained
output root; do not overwrite it. The five outputs and provenance manifest appear
under `<output-root>/evidence/ui-rider-ipum50kg`. The enabling catalog appears at
`<output-root>/src/assets/original_ui_ipum_texture_cache50kg_generated.inc` only
after all real outputs have been written and read back.

Root must preserve the complete conversion manifest and copy the generated
catalog into the matching active source path before the later build. The current
unavailable catalog cannot be enabled by a flag alone. Retain all five CPU mips;
GPU sampling deliberately uses the authenticated highest-resolution original base
image under the approved native residency policy. No historical VRAM allocation
or hardware IDCT/pixel parity is claimed.

## 2. Build and startup prerequisites

- Use the root's existing centralized native build path and time gate, on a
  frozen commit containing d25e7ca, its registered session CPP, current renderer
  fixes, and the real generated catalog. Record exact commit/catalog hashes.
- Keep the existing known native geometry/corpus/video launch inputs and actual
  context/currentness callbacks. No new substitute mesh or asset corpus.
- Original UI root must contain all currently required Main/Options archives,
  plus genuine UI/PODPHYS.RES, UI/UIRIDER/UIRIDER.RES, UI/UIBKG.RES and
  UI/UIPODTEX.RES at their compiled catalog paths. `readArchive` uses that
  exact catalog; it does not search or accept basenames.
- Set RR_ORIGINAL_MAIN_MENU=1.
- Set RR_ORIGINAL_UI_ROOT=/workspace/shared/recovery-20261010/ui-inputs/game-files.
- Set RR_ORIGINAL_MAIN_AUDIO=muted. Rider enabled audio is still unavailable.
- Set RR_ORIGINAL_RIDER_IPUM_CACHE=<output-root>/evidence/ui-rider-ipum50kg.
  That directory must contain exact19c_1-mip0.rrtex through19c_1-mip4.rrtex.
- Do not set RR_AUDIO_WAV in this muted observation. Retain the normal root
  window, bounded host-input/capture and exit configuration.
- Missing optional Rider environment input preserves the prior Main/Options
  startup contract; the root's enabled-audio Options observation must leave it
  absent. A present but invalid cache is an explicit failure, not silent fallback.

The existing GPU image budget remains32MiB. A genuine budget/admission failure
must be reported with its owning resource and measured need; the source overlay
does not manufacture readiness or truncate the bank to bypass it.

## 3. Proposed bounded observations

Use actual elapsed native frame time and the existing input/capture path. Do not
force manager lifecycle, model playback, RNG, profile fields or frame counters.
A 30-second bounded session is sufficient as an observation cap; wait for actual
reported milestones within it, and report failure if they do not occur.

1. Capture settled original Main. Record zero world ticks, initial profile/RNG
   and genuine bank publication. Select Main's first racing slot using actual
   keys and confirm. Cold admission must finish before source profile/route
   mutation; failures should remain on Main with a concrete diagnostic.
2. Observe the real Main exit/+440, screen5/page1 activation, Rider opening panel
   and aperture, actual Rider19 pod/character attachments, model animations,
   binder geometry and all window/body passes. Capture after the aperture is
   open and Rider input is genuinely enabled. Verify original text/portraits,
   stats/notches, background, prompts and Huttese depth, with no missing mesh or
   fallback art. Record source frame/target generations, quads/draws/triangles,
   GPU publication, attachment flag and zero gameplay ticks.
3. Hold idle long enough to see genuine source motion progression and possible
   idle transition; don't demand a particular random choice. Record the actual
   shared RNG and successive accepted source frame generations. Observe the actual pod
   transform and binder animation without resetting their owners.
4. Press Left and Right once each. They map to original Rider0/1. In this bounded
   initial19 implementation each requested other-rider path must return explicit
   unavailableDestination without changing selected rider/slot/profile, detaching
   models or publishing a different texture bank. Confirm/Enter likewise remains
   unavailable because page2/course ownership is incomplete.
5. Press normal Back/Escape. Observe the actual exit/+440, source owner retirement,
   return to Main4/1, correct saved Main slot and fresh Main bank publication.
   Capture Main after its aperture reopens. No stale Rider frame/source alpha
   should survive the bank switch.
6. If time permits, repeat entry and Back once in the same Application. This
   checks retained profile/light/RNG ownership and explicit old composer
   retirement without requiring a second process or synthetic reset. Quit from
   Rider or Main through the normal host path; reporting and cleanup must finish
   without requesting a Main particle/navigation owner on Rider.

Additional mode1/mode2 entries can be a later bounded observation after the first
complete flow succeeds. Their profile commits and prospective cold snapshots are
source-reviewed, but no visual/runtime result is implied here.

## Expected evidence and known scope

Capture files must carry the actual screen/page and renderer receipt associated
with their current GPU publication. Rider `selected_group` is null in capture
metadata because the adapter uses the actual selected-slot snapshot rather than
exposing a fictitious navigation group. `original_rider_state` contains genuine
Rider aperture/input/selection/audio fields and accepted source receipt counts.

Preserve native logs, screenshots/video, frozen build inputs and any concrete
failure diagnostics. A successful build alone does not verify visual fidelity;
a screenshot alone does not verify Back, repeated ownership or cleanup. Original
retail framebuffer pixel parity and hardware mip-residency parity remain unclaimed.
