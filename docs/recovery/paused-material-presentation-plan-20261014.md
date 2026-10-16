# Round eight — bounded paused material presentation analysis

Status: analysis complete; no scheduler implementation, production edits or patch. Local source is79fc223 plus preserved prior rounds including ROUND7, not the parent's newer089ec54 integration. Root's reported integration of ROUND7 and preservation of opponent batching are external status, not independently inspected here. Source references below name this isolated clone.

Session01a13c13-761f-7091-ace5-d597c948303d; gpt-6-astra / medium per existing task provenance. No model switch, helpers or duplicate writers. checks_run: [].

## Answer

The smallest defensible path is an additional, explicitly bounded MATERIAL PRESENTATION transaction for an already submitted, settled page. It must use the retained non-material draw basis and the actual canonical material owners, then submit/present the resulting real frame. It must NOT invoke a zero-dt update, ordinary page render, transition tick, deferred route commit, blocking Rider motion, or gameplay. Existing ordinary update/render/submit sequencing stays intact.

This path is not presently implemented. The new root-owned scheduler entry needs a cloud-owned pure material-service/projection seam for Tournament. If that seam cannot retain/reconstruct the actual frame without replaying page effects, the route must remain unavailable. Merely relaxing a render guard or copying the old framebuffer does not solve the problem: the former repeats side effects; the latter never displays changed material output.

## Original evidence versus native policy

Root supplied the full rr-application-animation-clock-proof.md (Library libfile_8b852b6dbe8881919ac1eb79e314150b). No original files, ELF instructions, parser or recovery pipeline were executed by this worker.

Proven source boundaries from that supplied text:

- Application global382B80 owns current230 and material-pause11C. Constructor13FC44/4C/50 initializes230/11C/120. Time publication140BE0 occurs before manager/root processing independently of11C. Flags228 bit3 at140C98 controls subsequent root update; it is not11C.
-29AC9C..ACB0 compares controller08 with application230 and returns on equality.29ACB8..ACC8 applies11C unless authored animatesWhilePaused18 permits service.
- Callback29ACDC..ACF0 precedes texture/vertex/pixel work. Callback=false still goes to29AD48, re-reads LIVE230 and stamps controller08 at29AD58. Texture leaf independently reads/re-reads230 at29A118/29A16C.
-17CCC0 sets120 and17CCC4 clears11C for the original one-update/re-pause command.140D2C..48 ends the real frame: copies LIVE230 to234, promotes pending120 to11C=1, then clears120.

These facts establish dispatch semantics and distinct application lanes. They do NOT establish that today's native inactive-window loop should run a source update, nor recover an original presentation cadence while paused. The proposed settled-page presentation admission/cadence is explicitly a bounded native scheduling policy. It does not claim full140AC0 scheduling or input-matcher parity.

Local corroborating sources: src/assets/original_standard_material_animation50la.h:73-79 has the duplicate/pause gate; recovery/20261010/ui-exact/assets-author/files/docs/original-ui-atm50kg.md:94-107 explains timestamp/sentinel behavior. ROUND7's src/game/original_application_animation_clock50kg.h:30-41 supplies the live read capability; do not substitute the older immutable-entry-stamp idea.

## Concrete blockers in the current native path

1. **Host eligibility and early skip.** tests/native/original_frontend_session50la.h:242-243 handles explicit pause/resume. :263 combines drawable, physical-input-or-unattended eligibility, minimize/event-budget and explicit hostPaused(). :287 invokes Frontend::sampleClock; :288 pauses audio when inactive. :289-292 resets lastUpdateNanoseconds, handles capture and continues without source update or render. This happens for source pause AND several unrelated host conditions. ROUND7 already publishes raw time here even when the legacy active timeline is held.
2. **Real update work.** platform/linux/native/original_ui_frontend50kg.cpp:853-890 requires no queued route and updates==submittedUpdate (:861), prepares audio, calls transition->tick (:877), performs page update and refreshes banks, then increments updates. dt=0 or input.paused=true does not remove these calls. A dummy update would authorize lifecycle progression and sequence/audio effects simply to obtain a render permit.
3. **Single ordinary render.** Coordinator :897 rejects zero updates or renderedUpdate==updates. :901-906 may finish Rider movement and resample material time. :908-913 calls the actual page render and records renderedUpdate. It is not a pure packet replay API.
4. **Single ordinary submission.** :915-939 requires renderer/bank identity, frameRendered, !frameSubmitted and matching renderedUpdate; actual renderer acceptance precedes submittedUpdate=updates. That equality protects the next source update from discarding an accepted source frame, including unique ExitText+440 endpoints. :940 onward commitDeferred requires the corresponding accepted frame before switching/queuing ownership.
5. **Mutating page render.** Main session :393-419 invalidates proposals, projects page/particles and calls aperture.render48. Options session :581-602 may apply Controls effects or aperture hooks. Rider session :898-905 applies page appearance/stat projection and aperture. Course session :442-458 applies palette and can begin entry at render48. None is established as a reusable paused-only projection.
6. **Repeated saved-dt danger.** src/game/original_ui_aperture_owner50kg.cpp:261-311 uses savedDelta298 to move geometry (:277-290), emits closed/threshold/opened hooks (:292-306), and writes lifecycle/input state. Repeating render with the old nonzero saved delta can advance an aperture without another genuine update. Setting a fake zero saved delta merely to bypass this would also be an unauthorized state mutation.
7. **Host wrapper coupling.** tests/native/original_ui_main_entry50kg.h:784-819 pairs coordinator update sequence with PCM advancement and ordinary render with submission/diagnostics. Standalone :333-337 runs update, render, swap; :377 commits deferred work. A paused-only branch cannot reuse that whole sequence and claim it performed no update.

Coordinator private counters are at :173; sourceCurrent() :280-282 checks application/manager/renderer/bank ownership. Existing checks should be reused as prerequisites, not bypassed by manufacturing numeric counter values.

## Proposed minimal transaction, not existing API

Suggested conceptual entry name: `OriginalUiFrontend50KG::presentPausedMaterials(...)`, orchestrated by a dedicated Frontend wrapper rather than Frontend::update/render. This name is a proposal only; no callable implementation is claimed. Parent may choose its actual naming. Keep the narrow initial implementation on the joined canonical Tournament route, not all screens.

### 1. Separate physical service eligibility from update eligibility

At the existing standalone :263/:287/:289 boundary, derive scheduling eligibility from current valid drawable bounds, focus/activity policy (including existing explicit unattended policy), non-minimized state, drained event budget and actual current host/context. Keep this distinct from the live11C read. The existing sampleClock(applicationClock,active) remains the sole publication there; do not take a second timestamp or turn its legacy advanceAnimations=false into pause.

After reading the supplied live view, consider paused presentation only when service eligibility holds AND pause11C!=0 AND pending120==0. Unknown/retired view is unavailable. If focus is lost or the drawable is unusable, skip entirely without changing material stamps. The unattended exception remains an explicit host policy, not evidence that focus inactivity equals pause.

Keep lastUpdateNanoseconds reset on this no-update branch as today; no acceptedElapsed/update call occurs. Continue explicit pause/resume and quit handling but do not dispatch page-navigation actions on this branch. On resume, use the existing ordinary active path, not a fabricated catch-up update or synthetic timestamp.

### 2. Admit a real stable source basis

Before any material mutation require, under coordinator serialization:

- live application/view, concrete current renderer/Video/context, exact active route and bank/publication;
- prior ordinary update AND its accepted ordinary render/submission, with updates==renderedUpdate==submittedUpdate, and no pending ordinary frame;
- settled route/phase and open stable page, no queued application request, pending route proposal, destination switch, retirement, exit endpoint, bank refresh, deferred callback or blocking Rider effect;
- a retained authoritative non-material projection basis from that exact accepted frame/route, including draw ordering, geometry/layout, clipping, camera/light state and resource identities;
- unchanged drawable/target contract for the smallest version. Resize or invalidated publication rejects paused presentation until a real supported path reestablishes the basis; do not rerun page layout to make it work.

No initial frame can be synthesized if pause arrived before the first ordinary frame. A pause in transition remains unsupported for this narrow path. Keep displaying/capturing only the last genuinely valid image under existing policy; do not call aperture to finish it.

### 3. Service only canonical materials and project their actual result

Cloud Tournament must expose a seam that services the actual canonical pass/controller/aggregate and rebuilds the affected packet/material state over the frozen non-material basis in original draw order. It must not call ordinary page/controller render, reapply SetText, advance model/particle/scene animation, consume page RNG, write profiles, or run transitions. Retained state is not permission to invent a visual overlay or replace the original resource identity.

For the smallest admission, retain the bounded joined ATM callback policy; do not admit arbitrary external callbacks merely because a view exists. If a currently bound callback may mutate page/application routing outside the supported contract, reject that route. The canonical owner still owns the exact source callback ordering and live re-read behavior for whatever callbacks are admitted. This worker cannot assert which callbacks root's newer Tournament code admits.

At each controller use live230 and live11C at their source boundaries. Duplicate stamps and paused non-permitted materials skip without stamping. Authored animatesWhilePaused permits a real service despite11C. Once dispatch proceeds, even callback=false commits the freshly re-read final230. The texture leaf uses its own live re-reads and real frame count/ordinal. Never replace these with an outer `advanceAnimations` gate or commit an entry snapshot after reentry.

A new timestamp may intentionally advance an eligible material on a later paused presentation; that is not accidental replay. Repeated calls for one transaction must not redo canonical service. Do not globally suppress a never-serviced controller just because another material saw the same application timestamp; per-controller source stamps remain authoritative.

### 4. Separate presentation accounting from source update accounting

Use a dedicated one-use paused-presentation receipt/serial tied to route/bank/base-frame identity and the prepared result. It is a native stale/replay guard, NOT current230, a frame-clock generation or a source update. Keep normal updates/renderedUpdate/submittedUpdate, lastAudioSequence and deferred-route eligibility unchanged. Do not set frameRendered/frameSubmitted to manufacture an ordinary submission.

Prepare all fallible admission/resource checks before canonical mutation where possible. Submit the actual prepared complete frame through the existing renderer's appropriate normal/source-frame submission API, then perform the real host swap/presentation. Render/API selection depends on the parent's current Tournament frame representation; it is not available in this older local tree. A cached framebuffer copy alone is only frozen presentation and does not satisfy animated material service.

If canonical service has mutated stamps and a later projection/submission/swap fails, report that failure and invalidate/fault this pending transaction. Do not roll clocks/stamps back or resample/re-run callbacks as though nothing happened. The smallest policy is no automatic retry after such a failure. Any later retry support must retain the already prepared packet and avoid material service; it cannot claim that renderer replay is safe without its own acceptance evidence.

Normal source input/update remains serialized against a pending paused transaction. A resume cancels only an unserviced proposal; it cannot erase already committed material effects. Retire completed/failed presentation bookkeeping before the next ordinary update, while keeping material stamps intact. No deferred commit is run solely to finish a paused display.

### 5. Preserve independent audio and actual host presentation

Keep existing inactive audio-output pause behavior. Do not call Frontend::update or advanceAcceptedNanoseconds to produce an audio receipt for a paused frame. Do not reuse the blocking Rider `present` helper as a shortcut: it performs checkpointAudio and belongs to another source operation. Only real renderer submission and host swap can advance paused-presentation/window counters. Captures must distinguish accepted source update identity from paused presentation serial; new display is not a new gameplay tick.

## Why this preserves sequencing

Ordinary update U still has exactly its existing render/submission prerequisite before update U+1, and +440/deferred routing still depend on that ordinary chain. A paused transaction borrows the completed stable basis for U without re-entering its page callbacks or altering its manager state. Only the authorized canonical material state changes under genuine live source gates; geometry is projected from the retained source basis and submitted once. Separate bookkeeping prevents a second display from pretending to satisfy an unsubmitted source update or triggering deferred work.

This is intentionally smaller than a general update/draw scheduler refactor. It does require a new pure projection seam and retained basis where none is proven locally; there is no safe one-line guard relaxation. If parent-current Tournament already provides that seam, reuse it. Otherwise keep paused presentation unavailable rather than infer purity from dt=0 or a settled-looking framebuffer.

## pending120 may remain unsupported

Yes, explicitly and without corrupting this bounded pause/material route. ROUND7's backing constructs120=0, grants only read access to consumers, and exposes no single-step writer. Its native explicit pause/resume API changes11C only. Ordinary live timestamp publication does not need to execute49I root phases or invent previous234 updates to service canonical materials.

The proposed paused presentation admits only pending120==0. A future nonzero120 must reject this bounded path and be handled by the actual single-step update owner; do not clear it during presentation or silently ignore it. Do not advertise/accept the original17CCA0 single-step command until one genuine update and the ordered267BE0/end-frame140D2C..48 continuation are connected. No `finishFrameTiming49I` call is appropriate in a material-only display: it would copy previous234 and potentially re-pause without the represented update boundary. The existing local physics49I zeros are not that owner.

This remains a disclosed subset: explicit native pause/resume plus material service, not full original pause/single-step matcher behavior. Root must audit any newly added120 writer before expanding admission. A future implementation must preserve callback-false stamping and live post-callback time even when an unrelated update path supports single-step.

## Exact parent/cloud work and limitations

Root: split the host inactive branch; add serialized material-presentation admission and receipt; retain/freeze the actual frame basis; wire real renderer submission/swap without ordinary update counters; preserve update/submit/deferred guards and independent audio policy. Cloud Tournament: establish the pure canonical material service/projection over its actual pass/controller/frame owners, including callback boundary semantics. Both must reject unsupported route/transition/bank/drawable/callback states rather than adding readiness booleans disconnected from real owners.

No proposed scheduler API or paused projection exists merely because this report names it. Parent-current Tournament/resource/render code is newer than the local clone and was not inspected. Original evidence proves the leaf gate and word meanings, not the proposed native scheduling policy. All runtime behavior, rendering, timing, failure/retry and paused audio effects remain unvalidated.

No repository-local AGENTS.md was found in this round's source inventory. Only source reads and ROUND8 report writes occurred. No production changes, patch, build/test/syntax/parser/import/sanitizer/benchmark/decoder/project-script/game launch, runtime lifetime action, guest execution, helpers, network, private logs, Git metadata write or central/other-clone access occurred. Prior deliverables and snapshots are preserved. Central execution and integration remain exclusively root-owned.
