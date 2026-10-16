# Native Windows window framebuffer evidence

Source-only candidate. No build, test, execution, screenshot or FPS outcome is
claimed by these changes.

The existing application still creates its real SDL OpenGL window. These options
are Windows-only and reject `--headless-live`. All defaults are unchanged.

## Explicit options

- `RR_WINDOW_CAPTURE_DIR`: existing absolute output directory, independent of
  the original-disc working directory. Use a fresh directory for each run.
- `RR_WINDOW_CAPTURE_AUTO=1`: automatically capture each settled supported UI
  screen and the first actual Racing track image. F12 also requests a screenshot
  when genuinely focused. At most 64 images per cold session are written.
- `RR_UNATTENDED_CAPTURE=1`: allow background-window frontend and original-AI
  execution while honestly retaining SDL's observed focus and input releases.
- `RR_UNATTENDED_SECONDS=180`: shared total application wall budget; optional,
  defaults to 180, accepts only 1..300. Frontend, transitions and race cold setup
  all share its original epoch. Deadline checks run at owned control boundaries,
  followed by normal cleanup, never a process kill or a reset on race entry.
- `RR_UNATTENDED_UI_ROUTE=main-rider-course-ta`: exactly three original Confirm
  events on fresh Main group1/slot1, Rider19, and Course TA/3 laps/8 entrants.
  Requires unattended automatic capture, Original Main and the FPS option below.
- `RR_UNATTENDED_UI_ROUTE=main-rider-course-sa`: retains the same Main/Rider19
  confirmations, then sends one original Course Next event (input1) from fresh
  TA. It requires the actual committed SA slot2/course11/planet4 selection,
  three laps and eight entrants, waits for the real deferred refresh and settled
  submitted frame, captures that SA Course image, then sends original Confirm4.
  Initial TA and intermediate Course frames cannot satisfy its Course capture.
  This route requires the same options as TA, explicit `RR_RACE_MODE=2`, and
  genuine SA archive, prepared resources and existing Course/runtime admission.
  Missing resources or a rejected selection fail closed; there is no TA fallback,
  profile unlock, direct race selection or synthetic driving. Source support
  does not establish prepared assets or a successful SA smoke/FPS result.
- `RR_UNATTENDED_UI_ROUTE=main-occo20-course-sa`: selects Occo Ninebar
  (Rider20/POD20, fresh slot2) using one original Rider Next1 from Rider19/slot1,
  then follows the SA route above. The fresh source profile already unlocks20;
  this route does not unlock or directly select a pilot. It requires the immediate
  attached20/slot2 receipt with deferred4EC=3 and revoked frame publication,
  then waits for deferred4EC=0, the settled attached selection, actual submitted
  model triangles, a newer accepted source-frame generation and the same live
  immutable GPU bank publication. Initial Anakin and intermediate Rider frames
  cannot satisfy Rider capture, including F12. Occo Confirm follows that capture.
  This is five one-shot original inputs: three Confirm4 events, Rider Next1,
  and Course Next1. Genuine Occo preview/race assets and handling, SA resources,
  and original AI for all eight racers (including the followed slot0) remain
  subject to existing admission. Missing resources fail closed without fallback.
  Existing `main-rider-course-ta` and `main-rider-course-sa` retain Anakin.
- `RR_TRACK_FPS_BASELINE=1`: separate original-all-eight-AI track baseline with
  five seconds of warmup and at least five seconds of actual completed swaps.

All existing genuine Course readiness, original asset/cache, original camera,
race mode, and ALL8 admission remain required. No roster or profile values are
written to make navigation succeed. The route uses the normal source input
dispatch and waits for real settled captures before advancing. Changed physical
menu input, missing assets or refused source transitions produce an explicit
failure rather than selecting a substitute. Course selection is also captured
and recorded, though the required images are Main, Rider and track.

Unattended mode omits only focus-derived execution pause. Minimize, invalid or
oversize drawable, event-budget exhaustion, explicit pause/menu, quit, source
failure and cleanup safeguards remain. Physical menu and camera controls still
require genuine focus; racer control remains the real original AI for all eight
owners including camera slot 0. No focus, desktop, activation or hidden-window
state is fabricated or changed. Evidence is labelled `unattended_windows`, not
interactive foreground or physical desktop scanout proof.

## Actual captured pixels

After the genuine renderer publication and successful SDL window swap, the
window-only `readbackWindowFrontRgba` API binds read framebuffer 0 and reads
`GL_FRONT`, using actual drawable dimensions. It restores the read framebuffer,
default read-buffer selection, pack settings and PBO binding. Its request must
match the idle renderer's committed dimensions and validated storage capacity.
No private target or diagnostic rendering is substituted if window readback
fails. Window-system occlusion/composition may affect the front buffer; the
result is application framebuffer evidence, not proof of physical scanout.

Images are genuine top-down P6 PPM files, accompanied by JSON with stage, actual
focus/minimize state, source/publication counters, dimensions, swap count and
selection data. A failed readback/write is fatal and never marks a capture ready.
No existing output is overwritten. Capture I/O discards an unfinished FPS
interval; the next actual swap starts fresh warmup.

The shared owner retains successful captures through the real frontend-to-race
cold transition. It writes `track-fps-baseline.json` only after a completed metric.
Once Main, Rider and track files plus that metric exist, the race exits normally
without consuming the remaining wall budget. After owned session cleanup,
`window-evidence-summary.json` and `windows_evidence_complete` identify the files.
Timeout writes an incomplete receipt and returns exit code 3; incomplete routed
evidence also returns 3. Source/render/I/O failures retain the normal error exit.
