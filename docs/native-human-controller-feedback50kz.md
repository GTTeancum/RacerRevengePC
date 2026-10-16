# Native human controller feedback 50KZ

This connection preserves the original `vehicle58!=0` feedback skip for every
original AI racer. Human `vehicle58=0, vehicle3C=0` with fresh profile11050 bit2
and positive D20+D24 resolves the actual source5C device object. The native human
slot owns source5C0; the camera-followed slot is not used to invent a human role.

## Recovered source behavior

The vtable at3D1D80 has +40=238870 and +44=2388B0. The second collision call at
222DEC is +44, not +3C. Clear238870 zeros selected duration timers. Request2388B0
checks the real capability subfield, clamps duration to[0,5], and replaces the
selected timers. Collision channel2 selects both timers and supplies
min(0.005*(D20+D24),0.5) seconds, not vibration strength. Both helpers always
return0; this value is not an SDK-success result.

The native application retains initialized device objects for source slots0/1.
A keyboard-only session genuinely does not initialize/open SDL game controllers.
Its fresh device feedback capability is false, corresponding to the original
constructor's capability mode2. Clear executes; request takes the source
incapable return. The profile vibration bit remains unchanged. There is no
virtual physical controller, fabricated callback result, or suppressed request.

`consumeVehicleImpacts49Z` commits only its previously recovered222B20 prefix
and enters `ImpactFeedbackPending50KZ`. The runtime enqueues any preceding
222CFC partner event, then `resumeVehicleImpactsWithControllers50KZ` resolves the
retained source5C and executes both actual native helper bodies. Separate
clear-returned and both-returned phases retain real effects on a later failure.
The suffix begins at222DF4 and never decrements prefix timers, enqueues event0,
or invokes a controller helper again. Later2228D0 partner events are returned
separately and appended before repair. Missing source5C ownership still stops.

## Original device service

Header-only `controller_feedback50kz.h` reconstructs initialized timers, signed
stop-write counters, and the actual six-byte actuator packet. After a successful
ready input sample, original237DF0 services large then small motor:

- Large source byte715 starts at200; small source byte714 starts at1
- An inactive positive duration sends its start packet and sets counter2
- A positive counter subtracts the actual supplied poll elapsed immediately
- Expiry sends zero and decrements the counter; the next eligible poll sends
  zero once more, preserving the two-write shutdown
- Durations may become negative; inactive/failed-read polls preserve all state
- Each command snapshots all six bytes in exact source order, at most four
- Source3139A0 return values do not roll back timers, bytes, or counters

The native numerical domain is finite normal/signed-zero binary32 and nearest
rounding for subtraction. Unsupported consumed arithmetic rejects explicitly.
No exceptional EE arithmetic or original instruction execution is claimed.

## Physical SDL transport and explicit platform policies

Only an actually opened, still-attached SDL instance can receive a submission.
SDL_GameControllerHasRumble supplies actual capability; keyboard input never
supplies that fact. Hot-unplug, instance mismatch, and unsupported hardware are
separate receipts and cannot be counted as successful transfers. A failed SDL
call reports its error and retains the already committed source service state.

Source actuator0 maps to SDL's high-frequency motor at0/65535. Actuator1 maps to
SDL's low-frequency motor atbyte*257, making source200 equal51400. These are
native platform bindings, not claims of identical PS2 motor feel. SDL calls
carry a100ms safety lease, renewed from the unchanged active source packet on
eligible native device ticks; lease refreshes are counted separately from
source commands. This preserves durations longer than100ms and sustained timer
replacement without inventing extra source helper calls. Pause/focus loss stops
physical output while source timers freeze. Teardown attempts a final stop but
a failure cannot prevent controller/window/GL/library release; it is reported
as a cleanup advisory, with the bounded lease as a fallback.

The existing native scheduler assigns a device-manager poll once per16ms
simulation tick, after packet acceptance and before event dispatch. Additional
host ingress and pause-release samples service the event bridge only. Original
1280E4..128140 proves device service precedes event dispatch, but the outer PS2
polling cadence is unrecovered. This fixed cadence is an explicit native host
policy, not original scheduler parity. No vehicle physics substep, opponent,
or all-AI frame can duplicate feedback service.

The optional physical-controller path now requires SDL2.0.18+ for its capability
query. Keyboard-only windows do not load those symbols. Windows x64 uses the
same SDL ABI/calling-convention owner as Linux, with no new translation unit or
platform-specific device library.

Official API contracts consulted:
- https://wiki.libsdl.org/SDL2/SDL_GameControllerHasRumble
- https://wiki.libsdl.org/SDL2/SDL_GameControllerRumble

## Verification boundary

This candidate received source/evidence review only. No build, compiler/syntax
check, executable test, sanitizer, gameplay, controller hardware check, or PC
interaction was run by its workers. The parent's authorized central build/smoke
and actual driving/controller acceptance remain required. Important cases are
keyboard collision continuation with unchanged profile4, a real rumble-capable
pad, an actual non-rumble pad, disconnect/reconnect, pause/resume and teardown,
source prefix/tail event ordering, and unchanged all-eight-original-AI skip.
This closes the represented feedback cut; it does not establish that unrelated
human race, camera, audio, or collision boundaries are complete.
