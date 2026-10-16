# Original Pause115A60 viewport scope and retained native raster snapshot

Base1786b90. Source-only; no build, syntax check, test, decoder or application
execution. No driver invocation or scheduler activation is included.

## Recovered leaf and concrete join

Retained rr-clock-full-static-fields.txt shows115A60..115AC8 copies exactly six
viewport fields. If outputA2 is nonnull it first saves rectangle1E0..1EC and
near/far1F0/1F4. It then copies the supplied six fields to the SAME viewport.
It does not update matrices, renderer publication, scene selection or globals.

Pause1B9DCC..1B9DFC copies those six current fields;1B9E00..1B9E10 replaces
only rectangle with0,0,640,448.1B9E0C calls115A60 with saved-output storage;
1B9E90 restores the saved six fields after the ordered outer draw tail.

New OriginalPauseViewportScope50LA is privately constructible only by the actual
Pause observation owner. It borrows the camera viewport while the SAME concrete
camera is synchronously locked and restores all six fields on every return.
No arbitrary callback, host viewport guess, changed camera matrices or fabricated
camera publication is accepted.

`captureOuterNativeFrame` locks actual Pause/control/camera/RA/profile owners,
checks existing source identity and session generation, and reads the actual
camera publication count. The shared profile1125C selects count0..3, independently
of ALL8 racer control classes. Count>=2 enters the actual viewport scope and
orders bottom polygon BEFORE icon/backdrops/derived/conditional top polygon.
Count1 retains its original bottom-after-derived order. Count0 emits no objects.

The old unscoped captureOuterLayout remains restricted to count0/1. Commands
carry a private viewportOverride tag; public native prepareOuter rejects that
tag. Only the concrete observation join can lower the multiview-tagged sequence
while the actual scope is live. Mode1F0!=0 remains unavailable.

## Deferred rendering lifetime

The existing GL renderer runs after CPU preparation, whereas115A60 restoration
belongs after the source draw-call sequence. Native lowering therefore copies
the ACTUAL scoped viewport into stable owned rasterViewport_ before restoration.
Frame.viewport points to that retained snapshot, not to the now-restored split
viewport. Source camera identity is tracked separately as an opaque pointer and
scene/session pair. The pointer is compared only, never dereferenced after the
synchronous call. GPU retention remains the existing strong native owner.

The native shader currently admits source depth range0..1 only; other actual
near/far values reject rather than silently rendering with the wrong GL range.
The scope still restores exact incoming values on this failure. All commands
share the one private color/depth pass from the integrated renderer.

This is draw-tail preparation only. Last-camera admission, geometry timer tick,
full parent/Entity callback, original phase ordering and reversible race lifecycle
remain separate activation prerequisites. No number of profile views creates
extra native cameras or licenses a fake scheduler receipt.

## Disconnection/base-text prerequisite recovered separately

Static table read at3D1DE0 proves PSX device3D1D80+60=239C10.239C10/14 returns
the SAME device6C4 word.1B9FE0 clears Pause1F0/1F4 only when race1B0 bit3 is
clear, queries each actual profile-count device in order, and stops at the first
zero result. It sets1F4 only when that failed index is1;1F0 becomes1. This is not
the rumble capability query or the manager human-count loop.

6C4 starts zero at237C30 and has reset writers238AE8,238BEC,238C60. At2391A4
it calls3133D0 using actual device port/slot6BC/6C0;2391AC..1BC stores query!=0.
That lower query reads state byte70, remaps state6/request byte71==2 to5 and
returns99 for an absent lower entry. Its meaning cannot be equated to SDL
connected or an initialization-success flag without recovering the surrounding
original6E4 state-machine phase. Existing native feedback/control objects do not
own6C4 or that state machine. No default1F4, simulated device state, replacement
base text or guest driver protocol was added. This remains the exact blocker
for the original localization87/88 base-text branch.
