# Same-manager Options page navigation: source-only candidate

Frozen October 11, 2026 against active repository HEAD `ddd4a3e`.
The active tree was read only. No build, test, syntax check, project parser,
decoder, sanitizer, game, or guest code was run. Review consisted of reading
source and the existing static instruction listings, then staging source edits.
Root owns integration and every executable check.

## Delivered

Four exact replacement files are in `files/`; their unmodified `ddd4a3e`
versions are in `preimages/`. `changes.patch` is the corresponding unified
source diff. No new translation unit or build-list change is required.

- `platform/linux/native/original_ui_frontend50kg.{h,cpp}`
- `tests/native/original_ui_main_entry50kg.h`
- `tests/native/terrain_interactive_drive50du.cpp`

The concrete coordinator admits only the existing Options menu1 slot3 ->
Game Options4 and Game Options4 Cancel -> menu1 page routes. It retains the
original proposal, revalidates the session and exact source/write shape,
admits the real prepared page in the existing session, promotes only its
local retained decision, and invokes the actual transition owner. The
commit callback writes only the exact remembered slot6E4 or group6E8.
`notifyRouteAccepted` receives the original unmodified proposal immediately
after manager acceptance and owns the existing SELECT1/BACK5 dispatch.
Game Options Accept remains unavailable.

Manager cold identity is now explicitly `initialRoute`. All bound current
routes, accepted update/input receipts and snapshots read the transition
owner. Notices verify the owner's actual lifecycle/phase and use requested
route only for requestedPage90; all other notices use current route. Thus
requestedPage90 can initialize page4 before the original manager publishes
page4 as current. Polling is against actual current page/phase.

No same-page change invokes cold preparation, creates a manager/session,
switches banks, changes GPU publication, resets scene/fire/camera, queues an
application screen request, or resets the source/audio clock. Existing
audio preparation -> manager tick -> child update -> accepted PCM advance
ordering is preserved. Cross-screen deferred ownership is unchanged.

The native Application now creates and retains Options menu and Game Options
profile fields alongside Main. It performs explicit fresh startup resets
once, then marks the new process-owned profile known. Page entry, Defaults,
Cancel and screen re-entry do not reset that application owner. Defaults
only edits the existing controller's local buffer. Cancel only remembers
the group, so the next entry reloads unchanged committed settings.

Host keyboard mapping is page-specific:

- Main/menu1: existing Up/Left/W -> 2, Down/Right/S -> 3 remain unchanged.
- Game Options4: Left -> 0, Right -> 1, Up/W -> 2, Down/S -> 3, R -> 5.
- Enter/Space -> 4 and Escape/menu -> 6 keep their existing source meanings.

The host already recognizes R as native physical scancode21 in both window
and stdin input, so no parser change is added. This is explicit native key
mapping, not a claim that retail used PC keys.

Reports/capture receipts now publish authoritative current/requested routes,
actual attached group and that group's selected slot, active-page aperture,
GPU publication token and accepted update sequence. Reports include page4's
local settings separately from the committed application profile.

## Merge dependency and remaining blockers

Integrate `options-sound-mode-reconstruction` before these files. The entry
header uses its concrete `platform::OriginalSoundMode50KG` and final
`AudioOutput::prepare(..., application.soundMode)` parameter. The application
initializes that owner once from its real fresh soundMode1101C, and the entry
verifies the actual output retained the exact same pointer. Muted operation
has no mixer and gains no Apply capability.

This patch does not claim Accept is implemented merely because that consumer
can be installed. A complete source1F66F0 profile/audio/category transaction,
its exact shared-owner checks and source Accept route still need integration.
No saved-file persistence, memory-card readiness, gameplay handoff, positional
surround phase playback, split-screen or vibration runtime consumer is claimed.
Other Options pages, second-controller popup and HOF remain unavailable.

Root's current unrelated `options_resource=5` widget-load issue must be
resolved before any page4 runtime result is possible; the widget-loading
worker owns its staged correction. This package does not alter widget binding.

Compile and runtime behavior remain unverified. Root should cover initial
menu1 activation, menu1 slot3/page4 entry, all eight groups and 0/1 values,
Defaults, unavailable Accept without profile mutation, Cancel, re-entry with
remembered group and unchanged committed values, and subsequent menu1/Main
Back. GPU token and screen-switch count must not change across page routes;
accepted sequence and audio nanoseconds must stay monotonic.

## Exact source proof

All listings in `evidence/` were copied from existing authenticated recovery
evidence, not generated by executing project decoders or guest code here.

- `options-menu-input.txt`: menu slot3 requests page4 at1F9898 (delay slot
  argument4 at1F989C), lifecycle7 at1F9890. The shared successful route tail
  remembers selected slot in profile6E4 at1F9940, SELECT1 at1F9950, flush
  at1F9968. Existing Back stores6E4 at1F9994 and requests screen4 at1F99A4.
- `game-options-entry-input-update.txt`: controller0 events0..6 are dispatched
  by1F50F0. Cancel obtains attached group through2A6F80 at1F51B0, stores6E8
  at1F51C8, writes lifecycle7 at1F51D0, requests page1 at1F51D8, then BACK5
  at1F51F8 and flush at1F5210. Defaults1F5220 calls1F6840, resynchronizes
  at1F5228, then requests ADVANCE2 at1F5248. Accept's distinct1F5134 call
  to1F66F0,6E8 store1F5158 and request1F5168 are deliberately not admitted.
- `manager-lifecycle-2A3C80-2A41B0.txt`: source manager2A4020 calls old
  page+94 at2A4050, requested page+90 at2A4070, only then stores current
  page index at2A4078 and current object at2A4094. Page exit uses
  beginTransition7C at2A3DD0; completion selects lifecycle1 at2A3F18.
  The native transition owner already preserves these original switch cases.
- `profile-reset-settings.txt`: fresh resets set auto-save bit0 at1865B4,
  stereo1 at1865BC, volumes50/100/80/80 at1865CC/DC/EC/F8, clear vertical
  split bit1 at186628 and set vibration bit2 at186654, preserving other bits.
- `profile-reset-words.txt`: source reset writes Main6E0=1 at186664,
  Options6E4=1 at186668, and Game Options6E8=1 at18666C. Startup invokes
  the existing field-reset leaves, not a fabricated saved profile import.

The sound consumer's separate README supplies its188740/26CBA0 evidence and
bounded centered-routing limitations. This candidate does not change that
consumer or create a second sound-mode implementation.
