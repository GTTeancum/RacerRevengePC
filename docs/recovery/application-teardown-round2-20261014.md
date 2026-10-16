# Application teardown audit — round two, 2026-10-14

Source-only result: no concrete GameHost50ER lifetime defect was established, so its implementation and header are unchanged. This document is the only integration change. No software checks or runtime activity were performed.

## Provenance and boundary

Repository HEAD observed: `79fc223eda2da3d98929c9f3b1253745d17e7c2a`. Before-state for the two owned GameHost files is `ROUND2-BASELINE/platform/linux/native/game_host50er.{cpp,h}`. This new document has no baseline snapshot and is an addition. Round-one WORKER deliverables and its audit document are preserved. References below name the locally inspected source, not the parent's newer checkout.

No AGENTS.md or SKILL.md was found in repository file enumeration. Read application ownership/cleanup code and public renderer/texture lifetime declarations only; renderer implementation, audio internals, profile, Tournament, and Ryloth were outside this audit. No changes to driving controllers, original AI, course drivers, input, rendering, or host execution policy were made. No PS2/VU/guest or native gameplay execution occurred.

## Actual application owner

`tests/native/original_ui_main_entry50kg.h:89–99` retains a shared application Video identity. Video contains `GameHost50ER host` and has a default destructor (`254–264`). `Application::videoForSession` refuses reuse when that host is either created or still owns resources (`314–320`). Holding the Video shared pointer keeps the C++ host object alive; it does not keep a closed GL context alive.

The executable's `main` calls the separate frontend or race function, waits for it to return, and only then processes the next session (`tests/native/terrain_interactive_drive50du.cpp:7600–7618`). The local host reference comes from Video (`4324–4328`); the window/context is created inside the cleanup-protected try block (`4618–4640`). GPU texture owners are declared before the local renderer (`4447–4452`), so their ordinary local destruction follows renderer destruction. The frontend coordinator and fade adapters are declared after the renderer (`4476–4480`) and therefore destruct before it. These local owners are distinct from the application's longer-lived Video.

## Race cleanup and rollback

The race's `closeWindow` lambda (`terrain_interactive_drive50du.cpp:4483–4617`) establishes this ordering:

1. Cancel/retire camera display transactions and close camera spatial/particle dependencies when the camera path is active (`4485–4491`). Failure returns before host closure.
2. If UI startup occurred, close an unborrowed renderer before UI banks; otherwise let the coordinator retire source owners through its borrowed renderer (`4493–4515`). Failed retry terminates while context and owners still exist.
3. After the application's audio cleanup calls, close any remaining renderer and return without host close if it still owns resources (`4528–4533`). Audio internals are not audited here.
4. Close dynamic/material and opponent/player/track/cable/sky GPU owners. Each ownership-retaining failure returns before host closure (`4534–4590`). Close remaining native model slots while Video is still available (`4591–4608`).
5. Only then close the host (`4609–4611`). Inspect the nested nonfatal rumble-stop warning (`4612–4614`) and clear the feedback host pointer (`4615`). This actual application already handles the successful-close warning identified as a caller obligation in round one.

Renderer creation failure throws into the same cleanup path (`5206–5207`). The catch invokes closeWindow before rethrow; camera-enabled failures retry and terminate if unresolved (`7435–7441`). Normal exit also closes, retries on that camera path, and rejects a false result (`7443–7451`). Other paths rely on explicit ownership guards followed by local destructors when cleanup fails. The renderer and texture public contracts require unresolved destruction to terminate rather than abandon GL names (`platform/linux/native/vehicle_window_gl50ds.h:902–905`; `material_texture_gl50eb.h:91–106`). This is source-level ordering evidence, not proof of successful driver cleanup.

## Separate frontend cleanup and rollback

`tests/native/original_frontend_session50la.h:16–20` declares the Video reference, renderer, then Frontend. Its closeWindow (`30–42`) first closes a renderer not borrowed by the coordinator, then closes UI owners, then any remaining renderer, and only afterward the host. Failed renderer/UI cleanup returns before host closure. The coordinator's `close` implementation retires source owners, closes its borrowed renderer, clears that borrow only on success, then releases managers and the light-manager lease (`platform/linux/native/original_ui_frontend50kg.cpp:988–1011`). `borrowsRenderer` compares the exact stored object (`1017–1018`).

Frontend host creation, texture-function resolution, bank preparation, renderer creation, and activation all occur within the same try block (`original_frontend_session50la.h:47–112`). Both exception cleanup and normal cleanup retry once and terminate before local-owner unwind if still unresolved (`406–416`). Thus failed frontend renderer creation cannot intentionally proceed to host close before renderer cleanup succeeds. The pending Course request is returned only after cleanup (`424`). No Course/profile behavior was changed or audited beyond this boundary.

## Borrowed GL procedures

`GameHost50ER::procAddress` first requires currentness, then delegates to its actual context owner (`platform/linux/native/game_host50er.cpp:93–101`). It returns an address, not a reference-counted lease; it has no registry of external GPU owners. `closeOwned` knows only its window/headless/stdin resources (`348–375`). Adding renderer-aware behavior here without an explicit ownership design would be speculative.

The race uses glGetString immediately after lookup (`terrain_interactive_drive50du.cpp:4850–4856`). Renderer callbacks borrow `&host` and query that exact host's currentness (`4859–4866`). Texture requests use the same host/currentness callback and load their GL function table (`4875–4879`). The frontend does the equivalent (`original_frontend_session50la.h:79–108`). Public renderer and texture contracts require those context/callback lifetimes through close (`vehicle_window_gl50ds.h:288–295,890–905`; `material_texture_gl50eb.h:36–40,74–76`). GL tables and GPU names must be retired before host closure; a stable Video address across sessions does not make old procedures or GL names reusable.

No application-side host release/rebind was found in the inspected race entry; host creation establishes the current context and explicit teardown encloses its clients. Renderer internals and all possible external consumers were not audited, so no universal no-stale-pointer claim is made.

## Failure diagnostics and parent follow-up

GameHost create rejects existing ownership, delegates creation, and runs closeOwned on failure (`game_host50er.cpp:127–150`). Window creation diagnostics stay in `d.window`; a failed facade cleanup is separately copied into cleanup fields (`51–57,141–145`). Closure marks the facade not ready but keeps its mode while resources remain (`348–375`). The destructor terminates on unresolved teardown (`62–65`). A false created() is consequently not permission to discard the owner; ownsResources() is the relevant owner-thread query (`69–83`).

The application's `reportHost50DU` prints only primary enums and errno (`terrain_interactive_drive50du.cpp:191–196`), omitting the already-preserved operation/detail/cleanup fields. This limits visibility into failed-create rollback or failed-close diagnostics, but is not evidence of incorrect resource ownership. An optional exact reporting extension is supplied in ROUND2-REPORT.md for the audio worker/parent; that read-only file was not edited.

Parent should reconcile these line-based findings against its newer tree and retain renderer/GPU-before-host ordering on all added exits. No code or manifest integration is required for this documentation-only patch. Parent owns all future validation. Runtime GL/SDL cleanup success, cached procedures inside renderer implementation, allocation-failure behavior in excluded owners, and audio teardown are unverified.
