# Native original Main application entry

Status: STAGED SOURCE ONLY. No build, syntax check, native asset parser, importer,
gameplay, test, or guest execution was run by this worker. Root owns all later
validation. This stage does not modify the active repository.

## Files and prerequisites

- `files/tests/native/terrain_interactive_drive50du.cpp`: integrated opt-in entry
  inside the existing complete cold session and actual interactive event loop.
- `files/tests/native/original_ui_main_entry50kg.h`: explicit fresh process
  application/profile owners, genuine native video/context owner, authenticated
  catalog-path asset loading, actual UI texture bank/session preparation,
  pause-aware source presentation clock, native status receipts.
- `preimages/tests/native/terrain_interactive_drive50du.cpp`: exact active
  preimage captured at eefa92a; unchanged through the observed41492c2 root update.
- The helper is a new file, recorded absent in the preimage inventory.

Required joins: original Main draw resources/collector, original Main session
with `render(clock,diagnostic)` preparing its own source particle constants,
original particle geometry/material/fog native renderer, original Main camera,
lighting/fire/widget/aperture/transition owners. The compiler command must include
`platform/linux/native/original_ui_main_draw50kg.cpp`,
`platform/linux/native/original_ui_main_session50kg.cpp`, and the new source
particle constants implementation if the session join introduces one. The existing
build-command preparer does not yet list those two new platform files; root owns
that central integration and build, not this worker.

## Concrete scope

`RR_ORIGINAL_MAIN_MENU=1` activates only the original Main frontend. Actual
player/track/roster cold loading and the existing renderer creation still occur.
No dummy mesh, placeholder renderer, generated labels, replacement images,
guessed fog constants, or synthetic input is supplied. The renderer enables
original UI with the exact CPU/GPU texture banks produced from authenticated
originals, then creates using the existing genuine player draw assets.

All Main events are intercepted before the existing host-menu, results,
restart/checkpoint, driving input packet and gameplay-step branches. Unavailable
confirm remains in Main; original back6 is genuinely unhandled. No route can
launch gameplay or restart into the host menu. Actual gameplay outside this
opt-in is unchanged. No host course driver is called by the Main path.

The native video owner genuinely contains GameHost and owns640x448 source canvas,
32-bit RGBA metadata and24-bit depth packet representation(max0xFFFFFF,format0x31).
Both window and headless requests use that owner's dimensions and RGBA8/depth24;
the real renderer owns actual RGBA8/D24 private targets. Shared lifetime uses an
owning shared_ptr conversion, never a no-op deleter or alias to stack storage.
The screen can resize while retaining the original640x448 source canvas.

The profile/application subset is new native process startup, not an imported
retail save or reconstructed boot. Native backing starts zero except the narrow
source Main reset. One initial sample from the existing shared application clock
seeds the process-owned Main RNG once; fire serial starts0 and both evolve in the
same shared owner. There is no active gameplay RNG consumer in Main-only mode.
Any future gameplay admission must join this same process owner rather than
copying values to another active RNG. The normal material mode is also an explicit
fresh process-owned value. No savefile writes or original startup-seed claim.

Explicit `RR_ORIGINAL_MAIN_AUDIO=muted` is required. No UI audio owner is supplied;
the source session records suppressed effects. Existing cold race audio may be
prepared, but its device is not opened and its mixer does not advance. `RR_AUDIO_WAV`
is rejected to prevent representing mute as an audio proof. Enabled frontend
music is not admitted by this entry yet.

Real elapsed time drives each Main update/render, with no fixed synthetic ticks
or clamping. Focus/minimize/oversize/explicit pause does not update or render and
capture can still retrieve the last committed image. A pause-aware presentation
sample accumulates actual quantized active intervals from the existing application
clock; paused wall time cannot jump arrow animation after resume. An active stall
outside the session's admitted delta range fails explicitly. Main debug source
buttons are unbound in this keyboard adapter and therefore actually released.

Cleanup resets session borrowers, closes renderer, then closes the exact UI GPU
bank, all before the host context. Failed GPU ownership prevents host closure;
existing ordered destructors remain available for cleanup retry. No resource
owner is destroyed under a live GPU borrower.

## Root-only launch contract

Working directory: the existing fully prepared native asset root, currently
`/workspace/shared/recovery-20261010/native-assets-prepared-01`.

Argument vector:

1. Root-built actual native fullgame executable
2. `evidence/staticType2Producer49X/original-word/native-fixtures.json`
3. `--headless-live`
4. The output PPM path, e.g. `OUT/live.ppm`

Keep the existing root-reviewed full-cold environment. Add:

- `RR_ORIGINAL_MAIN_MENU=1`
- `RR_ORIGINAL_UI_ROOT=/workspace/shared/recovery-20261010/ui-inputs/game-files`
- `RR_ORIGINAL_MAIN_AUDIO=muted`
- Do not set `RR_AUDIO_WAV`

One known full-cold combination is GC, RR_GC_INCOMPLETE_WORLD=1, RR_RACE=1,
RR_RACE_MODE=2, RR_START_MENU=0, RR_ORIGINAL_AI_ALL_RACERS=1, RR_AUTODRIVE=0,
RR_DISABLE_RACER_CONTACTS=0, RR_AUDIO=1, RR_AUDIO_DEVICE=0,
RR_AUTHORED_WORLD_FOG=0. These select real cold owners only: Main advances no
racer/world steps, keyboard driving packets or audio mixes.

UI assets are found exclusively by RR_ORIGINAL_UI_ROOT plus the exact compiled
archive catalog disc paths APPSTART.RES, LOC.RES, UI/UIMAIN/UIMAIN.RES,
UI/UIFONTS.RES, UI/CONTPOP.RES, UI/TEXTURES/OVERLAYS.RES. The source contains no
private absolute root. Every archive is size-bounded and authenticated by the
existing native archive owner, then English is read from the actual LOC archive.

## Capture and input protocol

Keep stdin open. EOF is a real quit and must not be used as an idle wait. Window
close or `quit\n` is terminal. No fake window swap is reported in headless mode.

`original_main_ready ` followed by JSON is emitted after the first complete
native GPU submission, with native_frame1,width640,height448. At that point the
original opening can still be in progress; readiness does not mean idle.

Send `capture\n`, one outstanding request at a time. The same existing renderer
readback produces P6 640x448 RGB, rows converted from GL bottom-up. It writes a
same-directory `.pending` path, closes it, then atomically renames it over the
requested PPM path. Only then does stderr emit:

`original_main_capture {"sequence":1,"native_frame":N,"file":"exact argv path","width":640,"height":448}`

This is followed immediately by `original_main_state ` plus JSON with reason
capture and submitted==N. Other state reasons are bound, ready and quit. State
keys include updates,renders,submitted,selected_slot,saved_slot,input_events,
unavailable_destinations,source_rng,fire_serial,input_shown,lifecycle,
aperture_state,aperture_top,aperture_bottom,aperture_input_enabled,particles,
source_packets,quads,spans,source_milliseconds,audio_muted,audio_suppressed,
world_ticks,mixed_frames. Aperture states are open0,opening1,closed2,closing3.
All counts come from real owners. Original Main emits no generic gameplay
capture acknowledgment, and no gameplay tick is invented to satisfy a recorder.

`quit\n` emits the state reason quit, invalidating the CPU draw publication;
quads/spans then read0. After all GPU and host cleanup succeeds, stdout emits:

`original_main_closed {"native_frames":N,"captures":C,"world_ticks":0,"window_swaps":0,"cleanup_complete":true}`

Then the process exits0. Missing closed or a nonzero exit is not clean completion.

Optional direct keyboard control uses the existing host key commands/physical
edges: Up/Left/W -> original2previous; Down/Right/S ->3next; Enter/Space ->4confirm;
Escape or explicit menu ->6back. Headless `down KEY` then `up KEY` uses the existing
host protocol; no delayed pulse is synthesized. Restart/checkpoint cannot leave
Main. Pause/resume are explicit host presentation controls only.
