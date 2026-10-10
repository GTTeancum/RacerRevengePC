# Concrete Main/Options frontend coordinator

Source-only staging. No native build, syntax check, test, project parser,
sanitizer, gameplay run, or guest execution was performed. The active repository
was read only. The package adds exactly two native source files and requires the
already-integrated exit-text, renderer-bank and complete Options owners. It also
expects the session stale-draw/reentry corrections integrated as 15ac82a.

## Deliverable

`OriginalUiFrontend50KG` owns concrete Main4/1 and Options12/1 manager lifetimes.
Each lifetime has fresh CPU resources, authenticated native material bank, GPU
publication, page session and transition manager. The coordinator retains the
actual process profile, explicit Options +6E4 profile subset, application/RNG,
video/context owner, original archives, decoded English strings and shared
frontend audio transport. Nothing resets the RNG or fabricates a save/profile.
The source application is never sent to gameplay. Main slots1..3/5 and Options
subpages/HOF stay unavailable.

Only two route transactions are admitted:

- Main slot4: real session proposal; complete cold Options destination; unchanged
  source proposal revalidation; actual manager `requestMain`; actual savedSlot6E0
  write; exact session `notifyRouteAccepted` SELECT1.
- Options Back: real session proposal; complete cold Main destination; unchanged
  proposal revalidation; clear only the original destinationOwner blocker after
  concrete admission; actual `requestRouteDecision`; exact callback commit of
  sole +6E4 word at1F9994; exact session `notifyRouteAccepted` BACK5.

The application queue callback checks source manager lifecycle9, actual closed
aperture, matching target, exact flags2/sourcePC2A3F70 and the still-live concrete
cold destination. It only records a deferred request. It never switches, destroys
or creates anything inside the callback. The source owner remains terminal after
queue acceptance. No readiness boolean or arbitrary callback certificate is a
public coordinator input.

## Parent entry wiring

1. Build `OriginalUiFrontendCold50KG` with the seven exact authenticated archives,
   same real 640x448/D24 video lifetime, process material runtime, existing Main
   profile/application, separately initialized/imported known Options +6E4 owner,
   explicit muted/enabled transport, and actual texture context/functions. The
   template texture request has no samplers; set an explicit per-bank byte budget.
   Existing startup's fresh native profile can call the proven Options reset
   helper explicitly; the coordinator does not do that for the caller.
2. Call `prepare(cold, diagnostic)` while the actual video context is current.
   English is authenticated/decoded once from retained LOC.RES. Main CPU/GPU/page
   are cold prepared, with an unbound external transition owner.
3. Call `configureRendererRequest(request, diagnostic)` before renderer.create.
   The exact context and isCurrent function pointer must match. This fills only
   the original UI opt-in and CPU/GPU pair. Caller retains its normal renderer
   create ownership and error handling. If renderer.create fails before activate,
   caller must close that partial renderer before coordinator.close, because the
   coordinator has not yet borrowed that renderer instance.
4. After successful renderer.create call `activate(renderer, diagnostic)`. The
   renderer independently validates the exact bank. Only then do original initial
   requestedPage90/pageEntry74 notifications run through concrete page callbacks.
5. For real input call `input`. Inspect status `routeAccepted`, `ok`, `noChange`,
   `unavailableDestination` or `secondControllerRequired`; other outcomes reject
   the operation/frame. Keep these separate from each underlying source page's
   original callback return. Routes carry no fabricated gameplay readiness.
6. Adjust host source elapsed/material-clock accumulation for any measured
   `receipt.coldInterval`: its steady-clock begin/end and nanoseconds bracket real
   synchronous destination preparation, including failed preparation cleanup.
   Exclude that interval rather than clamping a long delta, updating source time
   synthetically or reseeding RNG. Source logic receives no loading-time update.
   The caller continues to own the shared application clock and source conversion.
7. `update(update, receipt, diagnostic)` invokes actual page audio preparation,
   one actual manager tick, then actual source child update with identical args.
   A successful receipt has `accepted=true`, exact input, route and monotonic
   sequence. Advance root-owned PCM output exactly once for that receipt's actual
   accepted nanosecond interval. Do not advance PCM on failure or replay a receipt.
8. `render(clock, diagnostic)`, then `submit(renderer,width,height,diagnostic)`.
   Every accepted update must render and submit before another input/update, so
   the unique +440 loading-text endpoint cannot be discarded by a later tick.
   The coordinator reads the real manager lifecycle; no caller snapshot can stand
   in for entry or exit completion. Submit reports actual renderer acceptance,
   not a window swap or proof of hardware rendering.
9. Caller performs real window swap and capture/readback as appropriate. After
   every consumer returns, call `commitDeferred(renderer, diagnostic)`. With no
   queue this returns noChange. With the exact submitted queued-source frame it
   calls finishSubmittedWork (checked glFinish), replaces the bank while idle,
   binds the cold destination's fresh initial manager and retires the old session,
   GPU and CPU resources. The next real update is the destination's first restart
   case; no source update or entry notification is invented in between.
10. Quit goes through `requestQuit`, then `close` while the context remains current.
    The coordinator closes the borrowed renderer before sessions/GPU resources.
    Parent closes its shared audio output/device after sessions and before context
    shutdown. Retry only unresolved cleanup, never a partially consumed frame.

`mainSession`, `optionsSession`, `mainResources`, `optionsResources`, `textures`
and `gpu` expose const active owners for truthful state/capture receipts. Inactive
page getters are null. `snapshot` includes active route, actual manager and page
snapshots, prepared destination, deferred request and update/render/submission/
switch counts. Borrowed pointers expire at switch/close. `draw` is unavailable
while a public operation is active, after invalidation, on failure or after quit.
Caller must not separately drive sessions/managers, replace banks, close/recreate
GPU publications, or destroy the borrowed renderer/context during coordinator use.

## Failure/lifetime behavior

All manager callbacks are noexcept and use stable heap Manager contexts. There
are no shared_ptr cycles or runtime snapshot reconstruction. The cold destination
manager remains unbound until actual application activation. A rejected cold
preparation makes no route/profile/selection-sound write. Cleanly released failure
returns unavailableDestination with underlying preparation diagnostics. Failed GPU
cleanup is sticky and retains the incomplete manager and actual context owner.

Any source audio/callback/child/render/submit or post-admission switch failure is
sticky: no successful frame is published and no already-consumed operation is
retried. On bank replacement failure, the renderer preserves the previous borrow;
on later initial-notification failure, both source and destination lifetimes stay
retained for explicit close. A failed old-GPU retirement retains its owner too.
Close first ends the renderer borrow, then destroys sessions and closes texture
owners. A failed renderer close retains its borrowed pointer and all page/bank
owners until a successful idempotent retry; the noexcept destructor never
dereferences that borrowed renderer. No unresolved GPU names are silently discarded. The existing GPU-owner
contract still requires explicit close before destruction.

Initial source notifications and Main entry's real mode34 effect occur only on
activation. Source previousPage94 is not invented for a cross-screen retirement;
the old manager is destroyed through its native lifetime after deferred handoff.

## Source correspondence / validation remaining

The unchanged transition owner supplies original manager2A3C10 initial selection,
2A3C80 switch progression, and2A3F70 application queue. Main source input orders
manager request1E6160, savedSlot6E0 write1E61A4 and SELECT1 at1E61B4. Options Back
has +6E4 at1F9994, lifecycle6/request4/1 and BACK5 at1F99D0. The concrete sessions
and shared aperture own actual B4 exit effects and +440 RenderText1D75A4->2E0170.
The renderer bank swap/GPU barrier is explicitly native lifetime management,
not claimed recovered console timing.

Parent-owned central validation still needs compilation and actual enabled/muted
Main->Options->Main captures; source entry/exit endpoint frames; real selection
restore; no duplicate PCM; failed cold resource/GPU/cleanup paths; lost context;
foreign renderer; repeated input; pending frame guards; window/capture consumers;
and exact lifetime retirement. This package claims source inspection, not those
unrun outcomes. Add `platform/linux/native/original_ui_frontend50kg.cpp` to the
same native target that already owns both page sessions.
