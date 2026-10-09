# Raw Linux joystick transport 50DK

Status: **source-only, uncompiled, untested and unexecuted**. Only source/header
inspection and creation of these three new files were performed. No device was
opened; no runtime probe, test, build, compiler or validation script was run.
Existing sources, tests and build registries are unchanged. This consumed no
test slot and establishes no controller hardware or gameplay evidence.

## Scope

`joystick_transport50dk.{h,cpp}` owns exactly one explicitly selected Linux
joydev descriptor. It uses the public `<linux/joystick.h>` declarations inspected
in `/usr/include/linux/joystick.h`: `js_event`, the event-type bits and read-only
version/axis-count/button-count ioctls. It neither discovers devices nor changes
their mapping, calibration, permissions or configuration. "Raw" means unchanged
joydev event values; any kernel/driver-side mapping or correction is outside
this adapter, so these are not asserted to be raw hardware measurements.

This complements `src/game/controller_ingress50cx.{h,cpp}` only at the transport
boundary. It does not call 50CX or synthesize a `DevicePacket48Y`. A later explicit
device-specific adapter must choose physical axis/button assignments, calibration,
PS2 packet conversion, descriptors, action configuration and failure policy. No
timestamp conversion, game-clock origin, frame interval or neutral input is chosen.

## Caller contract

1. Keep one `JoystickTransport50DK` at a stable address. Construct, use and
   destroy it serially on one thread, with no concurrent/reentrant access. It is
   noncopyable and nonmovable and retains no caller path or buffer pointer.
2. Pass the explicitly selected absolute device path to `open(path, diagnostic)`.
   The path is used exactly as supplied, including normal OS symlink resolution.
   There is no default, directory enumeration, fallback device or automatic retry.
   Open is read-only, nonblocking, close-on-exec and without terminal acquisition.
   Successful character-device and joystick metadata checks leave every field
   unobserved. No event read occurs in `open`.
3. Give `poll` a separate writable `JoystickEvent50DK` array and its capacity,
   which must be 1 through `MaxPollEvents` (64). Capacity bounds both read calls
   and returned events. Each call requests exactly one UAPI event. There is no
   heap allocation, wait, retry loop or callback on the polling path.
4. Inspect the stop reason, diagnostic and progress counts before consuming
   data. Exactly `[0, receivedEvents)` was written; the accepted prefix of length
   `appliedEvents` has already updated the owner snapshot. Consume/copy those
   events before reusing the buffer. Snapshot references remain owned by this
   object and change with subsequent operations.
5. Inspect connection state, metadata, device counts and each field's `observed`
   flag before using its value/time. Zeros in unobserved or out-of-count slots are
   storage placeholders, never sampled neutral values. `initializationSeen`
   remembers whether that index has ever carried `JS_EVENT_INIT` in this open
   session; every returned event retains the exact original type byte. A regular
   event also makes a field observed. `allFieldsObserved()` only counts received
   fields; it does not certify a coherent full-device poll, fresh readings,
   completed game initialization or absence of event loss.
6. Call `close(diagnostic)` for a teardown result. The destructor also releases
   any owned descriptor. Linux close is attempted once, even on EINTR; the fd is
   no longer owned afterward and must not be retried. Close retains the snapshot
   for diagnosis. A later valid open attempt resets it, including on open failure.

The version value is reported verbatim. Admission requires successful metadata
queries and each read's exact public event size/type/index; no guessed controller
family, physical layout or unverified version-to-layout table is used. Axis and
button values remain signed UAPI values, including unusual button values. The
opaque `kernelTime` retains `js_event.time` with no arithmetic or unit conversion.
Fixed owner storage covers all byte-valued indices; counts still bound access.

## Stops, errors and partial progress

- `QueueEmpty`: `read` reported EAGAIN/EWOULDBLOCK. Earlier events in this call
  remain applied. It says nothing about whether all fields have been observed.
- `BudgetReached`: the allowed reads/events were consumed. More events may remain;
  the function deliberately performs no extra read to find out.
- `Interrupted`: EINTR stops immediately and keeps the fd open. Earlier events
  remain applied. The caller chooses whether/when to poll again.
- `Failed`: invalid arguments or no open descriptor consume nothing and preserve
  state. A terminal read error, EOF, short read or invalid event closes the fd.
  EOF and ENODEV/ENXIO select `Disconnected`; other terminal failures select
  `Faulted`. An open descriptor cannot promise hardware remains connected between
  reads. Poll never auto-reopens or supplies stale/neutral replacement input.

Every successful read is irreversible transport progress. `readCalls` includes
the final unsuccessful read, and `bytesRead` includes any short terminal fragment.
A complete invalid event is present as the last returned event but is not applied
to the snapshot. A short fragment is counted as consumed bytes and is not exposed
as a fabricated complete event; the stream is closed instead of guessing framing.
Previously accepted state is retained after either failure, but is historical once
the connection is no longer `Open`. These events/state must not be reported as an
atomic successful device sample or as having been rolled back.

Diagnostics distinguish invalid/duplicate opens, transport unavailable at compile
time, OS open failure, failed character-device check, unsupported ioctl (`ENOTTY`),
other query failure, read/protocol failure and close failure. They retain captured
errno and a separate cleanup errno. No permission workaround or device discovery
is attempted. Without Linux and its public joystick header, the same API reports
`TransportUnavailable` for a valid open attempt and performs no device operations.

Joydev is an event stream, not a transactional game-frame input source. This
adapter cannot prove that the driver retained every intervening transition, and
initialization events must not be mistaken for new physical presses by the later
mapping layer. Mapping, startup readiness and disconnected-device behavior remain
explicit unresolved caller responsibilities.

## Integration limit

A future authorized Linux client must compile the new `.cpp` with its own build
and provide a selected device and storage. No existing entry point or CMake target
was changed. Root-owned verification remains pending. No protected milestone,
game asset, executable or user-computer file was changed or executed.
