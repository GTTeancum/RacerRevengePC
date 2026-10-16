# Round 7 application clock source review

Reviewed 2026-10-14 against ROUND7.patch/ROUND7-REPORT.md and the applied, uncommitted source in `/workspace/shared/rr-recovery-git`, HEAD06ed4b1. Root confirmed application during this review. No production edits, patch applications, tests, builds, syntax checks, imports, project execution or guest execution performed. This is manual source review, not validation.

## Verdict

**No blocking defect found in this clock-owner/adapters delta.** The implementation follows the recovered230/11C ownership distinction, preserves the existing legacy presentation clock and connects a real read-only application capability across the named seams. Suitable to integrate as a bounded native publication change. This does not certify canonical Tournament dispatch, original scheduling or runtime behavior.

## Ownership and lifetime

- `original_ui_main_entry50kg.h:68..71` owns one private publication backing and retains the exact shared HostMonotonicClock50ID supplied at process startup. Constructor:205..207 retains/checks it before disabled-Main early return. Main state still aliases the same incoming pointer; no second epoch is constructed.
- Backing constructor is private and only actual Application is its writer/factory. Application copying was already disabled. View owns only shared const backing, not Application or Video; no ownership cycle is introduced.
- Application destructor:202 sets applicationAlive=false before member destruction. A retained Video/context/view keeps backing memory safe while read fails. No extra destructor requirement is imposed on consumers to clear dangling raw clock pointers.
- Default view is empty. Constructed-but-unpublished backing cannot be read, so constructor-zero230 is not a sampled receipt. read leaves output unchanged on failure; published0 andFFFFFFFF are not rejected or altered.
- Video gets the Application's own view only through its private constructor in videoForSession:347. The existing context factory already verifies actual Video identity and authoritative control block; the new argument is this Video's retained view. Context accessor uses existing live() admission. A previously returned clock capability remains a CPU clock view across host closure, intentionally not a GL-readiness token. After Application retirement the read itself fails.
- Constructor failure cannot currently leak a usable view: the constructor does not publish a view to an external consumer before it completes. Its ordinary destructor invalidation is therefore sufficient for the observed construction path.

## Sampling and cross-session consumption

- Frontend::sampleClock:486 samples exactly once using its existing host backend. Publication checks backend identity before writing the already-obtained rawSample. Legacy activeSourceNanoseconds/cold exclusions and sourceMilliseconds stay separate and unchanged.
- `terrain_interactive_drive50du.cpp:4424` replaces the local canonical scalar/+Published authority with a view of Application. advanceWorldAnimations:4434..36 samples once through that same owner and passes the returned value to existing bounded consumers. No parallel canonical clock or second publication remains at the old site.
- Applied-source Binder render region:7019..7036 reads the owner; only an unpublished owner triggers the preexisting first-render fallback. The subsequent render consumers use the value just read. Across a new race/session, a previously published UI/race230 remains an actual last publication. It is not a freshly sampled value, and the implementation/report correctly describe that distinction. No fabricated reset/generation is added.
- Startup RNG and other source101A50 reads do not all overwrite230; that distinction is correct. Original independent101A50 calls need not publish the application frame timestamp.
- Local physics FrameTimingState49I remains untouched and does not claim its zero230/11C are source publication. No140AC0/140D70/finish scheduling was simulated simply to obtain these lanes.
- Periodic diagnostics read the same view without sampling. Unpublished diagnostics may display default0 but separately report publication validity where the prior flag was reported; material consumers still check read success.

## Pause mapping

- Session hostPaused is now a read of the Application lane, not a second bool. Its setter forwards to the single explicit writer. Standalone and retained frontend pause/resume command sites both use it.
- Race code captures menuWasOpen before polling, compares it with the accepted resulting menu state, and writes only on an open/closed transition. HostSessionMenu50IF::poll admits the close only with canResume; a rejected resume does not clear the lane. Controls/Audio/confirmation subpages do not toggle it spuriously because open() remains true.
- Accepted Restart/ReturnOriginalMain/Quit results explicitly clear the lane before departure. This is the documented native policy, not an invented original source transition. Starting a fresh session does not blindly reset11C.
- Focus/minimize/event-budget/controller-loss/recovery states retain their separate suppression behavior. Composite gameplay `paused` is no longer passed as the MA material pause value. MA receives the actual application read.pause11C instead.
- Initially visible host startup menus do not synthesize an original pause action; this limitation is disclosed. They suppress gameplay through their existing menu scheduler. pending120 remains constructor-zero because no single-step/re-pause input has been joined. No unproven pending promotion or reset is fabricated.

## Adapter seams checked

- MainFrameClock appends view after the unchanged legacy three lanes. Old three-value aggregates therefore retain an unavailable capability rather than silently asserting known pause=false.
- Options/Rider/Course clock structs append the same default-unavailable view.
- Frontend render:909..911 explicitly forwards all four lanes. Rider uses refreshed renderClock after blocking presentation; other routes use the original input clock. Main already passes the whole aggregate.
- Session::sampleClock:513 is the producer of the four-field Main clock; materialClock refresh calls the same producer. Session::render continues forwarding the complete clock.
- Main/options/rider/course draw storage copies these aggregates rather than reconstructing only old fields. Rider stat projection:641 intentionally remains a separate legacy stat-clock conversion, not a canonical material consumer.
- Search of relevant native host/source files found no remaining mutable hostPaused field use. The sole StandardMaterialContext constructor call was extended alongside its private constructor declaration.
- Existing per-screen pause/advanceAnimations guards and coordinator one-render-per-update admission remain unchanged. There is no paused-only service scheduling claim.

## Non-blocking limits and hardening observations

1. `Application::publishAnimationTimestamp(backend,sampled)` is a trusted public producer API, not an unforgeable sampling receipt: identity validation checks the backend address, not whether sampled matches its latest result. The inspected callers supply the exact immediately returned sample, so no incorrect call is present. If this API is broadened beyond these root-owned producers, make the publication private/friend-scoped or combine sampling/publication in a trusted operation; do not claim the signature authenticates arbitrary user-supplied timestamps.
2. The read view is deliberately live. It does not itself enforce29AC90 gate/order/commit behavior. Canonical consumers must re-read at entry, texture read points and final29AD48 even after callback=false. Existing bounded value consumers are not general reentrant callback implementations.
3. Tournament career_resources::sampleMarker still uses legacy advanceAnimations/milliseconds in the inspected root source. This packet explicitly leaves that separate canonical integration to root. Do not advertise the clock change alone as canonical medstar dispatch completion.
4. A context-derived view is not a lasting host-current grant. Existing context/render admission must still be checked for each material operation. The new header/comment/accessor make that distinction explicit.
5. No public identity comparison between two clock views was added. In the actual one-Application production route the context/frame producer is connected to that same owner. If future multi-Application plumbing permits arbitrary view injection, pair the consumer with its actual owner's capability rather than accepting a field-equivalent or foreign clock.

## Review scope

Source proof reference: `/workspace/shared/rr-application-animation-clock-proof.md`. Patch reference: `/workspace/scratch/74cd1d3d7a45/clock-2213/packet/ROUND7.patch`. No issue in the inspected delta requires changing the recovered source proof. Execution/build checks remain root-owned and separately authorized. Opponent preparation/batching code was not altered by this review.
