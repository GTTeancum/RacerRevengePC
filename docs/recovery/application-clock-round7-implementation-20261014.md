# Round seven — connected Application animation publication

Status: source implementation complete; not compiled or runtime-validated. Canonical Tournament dispatch remains cloud-owned. ROUND7-API.md was published before implementation and updated with exact implemented qualifiers for early handoff. ROUND7.patch is incremental against the newly captured immutable ROUND7-BASELINE, not against Git HEAD and not a repetition of earlier rounds.

Session: 01a13c13-761f-7091-ace5-d597c948303d
Model/effort: gpt-6-astra / medium, per explicit launch/task provenance; no switch or private metadata inspection.
Git base: 79fc223eda2da3d98929c9f3b1253745d17e7c2a plus all retained local round-one/two/three work. Parent's a67bcaf and later a1a3ab4 opponent batching are not present or synchronized here. Parent must merge these clock-only hunks into its newer tree, never replace the host file wholesale.
checks_run: []

## Exact changed source paths

1. src/game/original_application_animation_clock50kg.h (new, header-only)
2. src/assets/original_standard_material_context50la.h
3. platform/linux/native/original_ui_main_draw50kg.h
4. platform/linux/native/original_ui_options_draw_contribution50kg.h
5. platform/linux/native/original_ui_rider_draw_contribution50kg.h
6. platform/linux/native/original_ui_course_draw_contribution50kg.h
7. platform/linux/native/original_ui_frontend50kg.cpp
8. tests/native/original_ui_main_entry50kg.h
9. tests/native/original_frontend_session50la.h
10. tests/native/terrain_interactive_drive50du.cpp

Metadata: ROUND7-API.md, ROUND7-REPORT.md, ROUND7-RECEIPT.json, ROUND7.patch, and fresh immutable baseline snapshots plus absent-new-file record. No prior WORKER*/ROUND2*/ROUND3*/ROUND6* deliverable, earlier evidence, build manifest, asset, renderer implementation, cloud career_resources/controller dispatch or opponent-preparation/render batching was edited. No new translation unit/build entry is needed.

## Source proof and correction

Root supplied the complete rr-application-animation-clock-proof.md text (Library libfile_8b852b6dbe8881919ac1eb79e314150b), recovered against its a67bcaf tree. This worker used that supplied proof as evidence; did not retrieve remote assets, re-run recovery, inspect SLUS or claim independent ELF verification.

The supplied proof identifies actual global382B80 Application, current230, material-pause11C and pending120; constructor13FC44/4C/50 clears those words.140BE0 publishes49J-quantized uint32 milliseconds independently of pause.140E38/3C and140D2C..48 establish separate previous/reset/end-frame boundaries.17CC74..84,17CCC0..C4 and1BE8C8/1BE848 are independently proven pause writers. No focus/audio/minimize/dt inference is authorized.

Critical correction to ROUND6:29ACF4 false callback branches to29AD48, then freshly re-read LIVE Application230 is stored at29AD58. The texture leaf also reads/re-reads live230 at29A118/29A16C. The new API is a live read capability, not an immutable entry stamp. The cloud dispatch owner must implement these exact read/commit points. Earlier ROUND6 deliverables are preserved as requested; this API/report supersede their incorrect success-only stamping statement and missing pause provenance.

## Connected ownership/dataflow

- New header: sample fields at :9, private backing at :13, live view at :30/read :38. Only the concrete Application can construct backing/grant a view. Default view reads fail, and constructor-zero230 is unavailable until actual publication. read leaves output unchanged on failure. Published zero is valid; FFFFFFFF saturation/sentinel is preserved without clamping. Single-threaded ownership is explicit.
- Application in tests/native/original_ui_main_entry50kg.h:68 retains the same shared HostMonotonicClock50ID passed to its constructor. Construction :205-207 now retains/requires it BEFORE disabled-Main early return; direct-race entry therefore owns the same clock. One private lane backing is allocated at Application construction. No second timer or epoch exists.
- Methods :116-136 grant a view, publish already sampled time after backend identity agreement, sample via the retained backend at existing race boundaries, and read/write explicit pause. Destruction :202 invalidates outstanding views while their shared backing remains memory-safe. No Application/Video backreference is retained by the view.
- Frontend::sampleClock :486-513 still samples once and computes the original legacy active/cold-filtered timeline. Its existing rawSample is published at :491 before returning the four-field clock. Legacy known/advanceAnimations/milliseconds remain unchanged. Post-blocking materialClock calls this same producer intentionally; adapters do not take another sample.
- Frame clock headers append applicationAnimation. Existing three-value aggregate callers leave this view unavailable rather than manufacturing provenance. Frontend route adapters :909-911 forward the view to Options/Course and refreshed renderClock.applicationAnimation to Rider. Main already forwards the full object. Rider stat projection remains explicitly legacy, without a fabricated canonical pause lane.
- Video construction :297 and application videoForSession :347 pin the same view. StandardMaterialContext constructor receives it through the existing private Video factory; its applicationAnimationClock() (:72) grants it only through existing live() host-current/context checks. A returned clock view is NOT GPU readiness; callers retain their existing context/render admission checks.
- Terrain :4424 borrows one live view instead of owning sourceApplicationTimestamp/+Published. advanceWorldAnimations :4434-4436 samples/publishes through the actual Application, preserving the existing one-sample boundary and sharing that result with current world/ATM consumers. The Binder fallback :6998-7014 reads that same owner and samples ONLY if no application publication exists. Publication now survives serial UI/race sessions, so a prior actual UI sample is legitimately retained before the first world step. No per-race reset or synthetic zero is certified.
- Existing bounded MA shield service :7025 now receives pause11C from the application read, not the composite scheduling variable paused. The existing readiness guard still requires publication. Current race services remain bounded value consumers; general canonical callbacks use live re-reads per the cloud API. Periodic diagnostics :7200 read a separate observation of the same owner, never another authority or sample.

## Pause mapping and explicit limits

Frontend::hostPaused() (:479) is now a read of the actual Application lane, not a second mutable bool. setHostPaused (:482) delegates to the owner. Standalone frontend :242-243 and retained terrain frontend :5759-5760 map the existing explicit native pause/resume requests. Their focus, drawable, event-budget, minimize and unattended scheduling gates remain distinct and unchanged.

Accepted native race-menu open/close transitions at terrain :5969-5973 set11C to match those explicit actions. Accepted terminal menu actions clear it at :5985 as a documented native departure/resume mapping, not a generic page/race initialization reset. No write occurs merely because the composite gameplay paused predicate changes. Initial host startup-menu visibility is not used to claim an original pause event. This is an explicit native input/menu policy onto the recovered11C lane, NOT original17CC64 physical matcher parity or implementation of every original pause writer.

pending120 is constructor-zero and read-only to consumers. No single-update/re-pause command is joined in this task, so no pending writer/140D44 promotion is fabricated. previous234, raw250, phase22C and root scheduling are not implemented or reset by this clock publication. The existing local physics FrameTimingState49I and .016f factor remain untouched and never certify known230/11C.

No paused-only presentation scheduling was added. Current inactive frontend continues to skip update/render; existing per-screen paused&&advanceAnimations guards and coordinator update/render sequencing remain. Thus animatesWhilePaused can be correctly represented/read at a real service but paused-only runtime coverage is NOT claimed. No callback, material dispatch, texture conversion, GPU publication, AI/driving controller or course-driver code was changed.

## Cloud/root integration contract

Use `rr::platform::OriginalUiMainFrameClock50KG::applicationAnimation`, type `rr::game::OriginalApplicationAnimationClockView50KG`. Read `rr::game::OriginalApplicationAnimationClockSample50KG` via `bool read(Sample&) const noexcept`. Fields are current230, pause11C, pending120 (uint32). The source gate uses pause11C!=0. Missing/unpublished/retired owner fails instead of assuming pause=false.

Cloud Tournament owns duplicate/pause gates, callback execution and canonical aggregate/frame operations. Re-read at original entry/leaf/final boundaries, including final stamping after callback=false. Do not put canonical dispatch behind an inferred !advanceAnimations pause gate. Independent suppression before service must remain a no-stamp operation. Do not copy a traversal entry stamp across reentrant callback publication. Root's newer Tournament sampleMarker receives full MainFrameClock; its actual current code must be reconciled without importing this older frontend's full source.

The sealed Video context accessor supports consumers needing the same owner without a frame-clock parameter; it does not create a second authority. One main-thread producer contract still applies. Producer methods are trusted application operations; material consumers receive only read capability. Publication uses the permitted host backend/49J mapping, not console sample parity, wrap invention, or a wall-time readiness claim.

Parent added opponent preparation batching a1a3ab4 near the render region. This patch changes only the clock read immediately before the existing Binder service and the MA pause argument nearby; no opponent preparation loop/body, batching, ordering or rendering logic is altered. Reconcile those hunks by source anchors into the newer file. New header is complete for early cloud handoff.

## No checks / remaining work

No builds, tests, syntax/parser/import checks, sanitizer, decoder/converter, project script/preparation, gameplay, guest/SLUS execution or executable probing ran. Git no-index diffs were used only for static deliverable assembly. No .git writes, patch application, diff --check, commits, branch sync/reset/push, network, installs, security changes, private session/auth logs, helpers or central/other-clone access occurred. No repository-local AGENTS.md was found; only applicable source instructions were followed.

Parent alone integrates and performs any separately authorized central build/smoke. The previously reported a67bcaf pass does not validate this untested delta. Unknowns are parent-current API/merge differences and execution behavior, not the supplied source identification of230/11C. Cloud canonical dispatch and any future paused-service scheduler remain separate work. A new central failure notice requires pausing feature work for recovery; none arrived during this implementation.
