# Live game host 50ER

`GameHost50ER` selects one existing host explicitly:

- `Window`: `WindowHost50DQ`, unchanged SDL raw keyboard input and real window swap submission; the F5 press also requests a host-only full-session restart.
- `HeadlessLive`: `OffscreenGlContext50DC`, live newline-delimited commands on nonblocking stdin. Use the explicit `MesaSurfaceless` EGL platform where a window display is unavailable.

There is no automatic fallback, frame script, artificial vehicle motion, alternate physics loop, networking, or display-socket workaround. This is a host for the existing 50DU game client, not a standalone rendering fixture.

## Connecting the existing client

1. Add `game_host50er.cpp` and `offscreen_gl_context50dc.cpp` to `rr_linux_interactive50dv`. Include `game_host50er.h` in 50DU and replace the host/diagnostic types with `GameHost50ER` / `GameHostDiagnostic50ER`.
2. Create `GameHostRequest50ER`, choose its `mode`, and populate the selected `window` or `headless` request with the existing explicit dimensions, GL version/profile, and component requirements. Headless mode does not use a swap interval.
3. Point renderer and material-bank callbacks at this exact `GameHost50ER` instance, using its `current()` and `procAddress()`. Choose `VehicleWindowOutput50DS::headless` only when `host.mode() == GameHostMode50ER::HeadlessLive`; otherwise keep `defaultBackBuffer`.
4. Declare `GameHostInput50ER hostInput; auto& input = hostInput.keys;` and call `host.poll(hostInput, diagnostic)`. Keep the same 50DR packet composition, 50CX ingress, 48Y held-state sampling, 48Z event dispatch, fixed-step clock, physics, live membership and renderer loop. `host.info()` retains the logical/drawable dimension and focused/minimized fields used there.
5. Latch `hostInput.captureRequested` in the client. After a successful real-game renderer submission/commit, fulfill a pending request using 50DS `readbackRgba`. If ordinarily host-paused or no frame has committed yet, retain the request. At the specifically declared undertrack recovery pause, 50DU may capture the last already committed image without preparing or submitting another frame. Do not generate a substituted frame. Cold-allocate any readback buffer within the renderer's fixed dimension ceiling.
6. A successful renderer submission is a render commit. Separately, only when `host.hasWindow()` is true, call `host.swapWindow(diagnostic)` and increment an actual swap-submission counter. Calling it in headless mode returns `NotWindow`; there is no simulated swap success.
7. On `input.quitRequested` or Escape, feed the same final released packet through the existing native sampled-input path before shutdown. Quit has priority over a simultaneous restart. If `hostInput.inputError` is nonzero, report the stdin errno and retain the error exit. Close renderer, texture bank and then host, preserving existing ownership order.
8. On `hostInput.restartRequested` (F5 press or stdin `restart`), 50DU dispatches and acknowledges the same 50EC release-all sample, closes renderer, texture bank and host, then destroys its entire session. Its outer loop creates fresh native state and owners using the unchanged authored-grid cold initialization. The process remains alive; a window/context is recreated. The facade only reports this request and never performs a gameplay reset. Original source `2255B0` reposition remains separate and pending.

Diagnostics keep the selected underlying diagnostic in `window` or `headless`; top-level `error`, `operation` and `systemError` describe facade failures. Failed creation can retain ownership if cleanup fails. As with both original hosts, resolve explicit close failures before destruction.

## Live commands

Keep stdin open while playing. In a terminal, enter one command per line. The host does not change terminal settings, so press Enter to deliver a command.

| Command | Effect |
| --- | --- |
| `down W` / `up W` | Set/release the current raw W key state |
| `down S` / `up S` | Set/release S |
| `down Up` / `up Up` | Set/release the Up-arrow key |
| `down Down` / `up Down` | Set/release the Down-arrow key |
| `down R` / `up R` | Set/release R; its game action remains the native client's binding |
| `down Escape` / `up Escape` | Set/release Escape; the existing client handles quitting |
| `release` | Release all held keys |
| `capture` | Request a capture from a committed live game frame |
| `restart` | Release held keys and request a full cold session restart in the client |
| `quit` | Release all held keys and latch quit |

Commands and key names are case-insensitive; spaces, tabs and CRLF are accepted. Input is current raw key state, not an action/event injection API. Holding a key lasts until its release, `release`, restart, quit, EOF or read error. Down/up drained together do not manufacture an intervening native sample or a button pulse. Keep a key held across an actual game input sample when a press is intended.

Each poll makes at most one nonblocking read of 1024 bytes. A full read sets the same conservative `eventBudgetExhausted` flag used by the window client. A partial command persists in a fixed 64-byte buffer; at most 63 bytes excluding newline are accepted. Unknown, malformed, non-ASCII or overlong lines are rejected whole, reported through `commandsRejected`, and never partially applied. Blank lines do nothing.

EOF discards any unterminated line, releases every key, marks `inputEnded`, and latches quit. A permanent read failure does the same and reports errno through `inputError`; `EAGAIN`, `EWOULDBLOCK` and `EINTR` merely produce a current-state snapshot. Terminal snapshots are returned successfully so the client can sample the final release before teardown. Commands following `quit` in the same read are ignored. `restartRequested` is a
per-poll flag, coalesced within one poll. Commands later in that same read still
belong to the old session; quit takes priority, and other old-session input is
discarded when 50DU recreates the session. A partial line does not survive a
restart. F5 uses the non-repeated SDL press edge, not a native action-button slot.

The facade exclusively borrows stdin and its open-file-description flags, restoring the original flags on close. Do not concurrently read, change flags, close or replace stdin while it owns the host. No per-poll application allocation is performed; underlying context creation retains the original hosts' bounded cold allocations.

## Verification status

The restart addition is source implementation and manual source review only. No
build, tests, runtime probes, game runs or installations were performed for it.
The coordinating task owns focused checks. A preceding live-client run reached
6,821 completed ticks before `ExternalCut` at `0x228BBC`; that observation does
not validate these later restart changes.
