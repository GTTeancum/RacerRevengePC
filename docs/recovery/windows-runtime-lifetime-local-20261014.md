# Windows runtime lifetime audit — 2026-10-14

Base: `79fc223eda2da3d98929c9f3b1253745d17e7c2a`.
Status: source inspection only; no builds, tests, syntax checks, executable probes, or runtime validation. No runtime source changes were justified by the inspected ownership paths. Line references below refer to this base, whose host sources are unchanged.

## Scope and ownership

Audited `platform/linux/native/window_host50dq.cpp` and `.h`. Read the adjacent dynamic-library helper and GameHost close wrapper for boundary context only. No renderer, audio, Main, build, gameplay, or driving-controller changes. SDL device transport is distinct from the original in-game AI driving controllers; this audit changes neither.

`State` owns the DLL reference, SDL window, GL context, and up to two opened SDL game controllers (CPP lines 103–113). SDL joystick pointers obtained from controllers are temporary borrowed values, never independently closed (181–208). The host is noncopyable/nonmovable (header 97–100). Construction records a thread identity; it does not independently establish that this is the process main thread (CPP 261–268). The caller must satisfy the header's main-thread and exclusive SDL ownership contract (header 85–90).

## Creation and failure paths

| Stage | Source evidence | Ownership outcome |
| --- | --- | --- |
| Invalid request, wrong thread, existing state, allocation failure | CPP 288–294 | Returns before acquiring a new SDL/library resource. |
| DLL load or required symbol/version failure | CPP 213–257, 296–309 | Cleanup unloads any acquired DLL; `initAttempted` remains false, so incomplete function tables are not used for SDL teardown. |
| Foreign SDL subsystem or current SDL GL context | CPP 310–318 | Rejects before SDL initialization; cleanup releases this host's library reference without calling SDL_Quit on the foreign owner. |
| Optional controller entry-point failure | CPP 159–179, 324 | Occurs before initialization and before any controller opens. |
| SDL initialization failure | CPP 325–328, 568–592 | `initAttempted` is set before SDL_Init, allowing SDL_Quit to clean a partial initialization. |
| Attribute, window, window-ID, context, bind, or actual GL requirement failure | CPP 344–406 | Acquired window/context handles are stored immediately and use the shared cleanup path. The primary error detail is copied before teardown (305–307). |
| Controller open failure or device changing during open | CPP 181–208 | Failed opens publish no connected handle; opened but rejected devices are closed. Accepted handles remain in stable slots until disconnect or host close. |

Create failure returns false even when cleanup succeeds. If cleanup cannot finish, its diagnostic is copied into the create diagnostic's cleanup fields (296–304). State remains owned, so a second create is rejected until close completes (291, 594–603).

## Shutdown and native handle lifetime

`closeOwned` first clears readiness (564–567). If an SDL current context conflicts with the stored context/window, or unbinding fails, it returns before deleting resources (568–573). The owner can retry close after resolving the failure. Since readiness is cleared, normal bind/render APIs are no longer a recovery mechanism: bind requires `created()` (413–416). Do not interpret `created()==false` as proof that ownership was released; use `ownsResources()` on the owner thread (269–270).

Once unbound, teardown stops active rumble best-effort, closes controllers, deletes the GL context, destroys the window, calls SDL_Quit, and releases the DLL reference, in that order (574–600). A failed motor-stop submission records `cleanupError` but does not prevent resource release. Window/context/controller teardown APIs are void here; the wrapper cannot establish driver-side success. A DLL-release failure retains `State` and its library handle, with `initAttempted` cleared, so retry does not repeat SDL destruction (589–603).

The Windows helper acquires an HMODULE with LoadLibraryExW and releases it with FreeLibrary (`native_dynamic_library50win.h` 48–96). The host retains this reference through its final SDL call. It does not own or expose raw HWND/HDC/HGLRC handles; SDL owns their implementation lifetime. The audit does not establish that FreeLibrary unloads the module when other references exist.

GL function pointers returned by `procAddress` are borrowed (CPP 275–277). Callers must stop using cached pointers and destroy renderer GL objects while the appropriate context remains current, before host close. DLL ownership alone is insufficient once the GL context has been deleted. No renderer lifetime proof is claimed by this bounded audit.

Successful teardown clears state and cached window information; repeated close then succeeds without invoking SDL (565, 601–604). Destruction on the wrong thread or with unresolved teardown terminates (261–264), as documented in header 94–96. Explicit close and diagnostic handling are therefore caller obligations.

## Parent integration follow-up and uncertainty

- Inspect nested cleanup diagnostics even on successful close. `GameHost50ER::closeOwned` forwards `d.window`, but its return status changes only when window close returns false (`game_host50er.cpp` 348–352, 375). Thus a rumble-stop warning can coexist with true at both layers. This is a reporting obligation, not a demonstrated handle leak; no wrapper edit was made.
- Confirm renderer teardown occurs before host close and cached GL pointers are not reused afterward. Integration and any needed changes outside the two owned source files belong to the parent.
- Preserve exclusive SDL ownership throughout lifetime. The creation-time SDL_WasInit guard is not a process-wide lock and does not prove absence of raw foreign WGL contexts; no speculative locking or WGL probing was added.
- Heap-payload events are disabled before window creation (CPP 330–335), and drained drop/extended-text payloads are freed (poll, 483–487). External producers or filters that violate the exclusive event-queue contract were not audited; no new unbounded shutdown drain was introduced.
- Parent-owned future validation may cover each creation failure boundary, controller disconnect/open rejection, unbind failure/retry, library-close failure/retry, repeated close/recreate, and successful close with a rumble warning. These are suggestions only; none was executed here.

No AGENTS.md or SKILL.md files were found within this isolated repository using file enumeration and tracked-file discovery. No private session/auth logs or external documentation were read. No network access occurred. Runtime SDL/driver behavior, physical-device response, GPU completion, and application-wide shutdown ordering remain unverified.

