# Concrete held-state input: 48Y-DeviceHeld

2026-10-06. Cloud/Linux continuation of preserved 48X-PlayerInput.

## Production closure

`src/game/device_held48y.h/.cpp` replaces 48X's opaque action-query boundary for
supported direct button codes with a concrete native current-held-state provider.
It also produces that state from supplied ready packet data, preserves original
pressure/digital behavior and edge resets, and returns the original ordered
transition records. The ready-frame entry then uses the unchanged 48X input path
through engine/aero binding. No physical button names are invented.

The initial type 7 DualShock descriptor profile is supplied by
`initialDualShockDescriptors48Y`, with the proven active count 16. Its values are
independently derived by executing 156 original straight-line setup instructions
from an initially cleared descriptor table. This is a numeric profile, not device
connection negotiation or an OS controller driver.

## What the query actually means

The PSX device vtable at 3D1D80 maps slot+10 to 143600. It is a current-held query,
not a queued-event consumer. A query succeeds only when the selected record's
first word is exactly 1; merely nonzero is insufficient. Repeated queries do not
consume or clear held state.

The one digital bank contains 32 records at device+324, each 24 bytes:
- held word, pressure/value float, four auxiliary words
- optional successful output copies all 24 bytes verbatim
- a miss leaves the optional output untouched
- a null output pointer is permitted

The float payload is copied as bits, including signed zeros and arbitrary NaN
payloads; the query performs no floating-point arithmetic on it. Auxiliary fields
have proven edge-reset behavior but no proven duration/timestamp interpretation.
The base constructor initializes only held/value in each slot. Native callers
supply the remaining state; this milestone does not claim those auxiliaries were
all zero in the original process.

Native direct codes are explicitly bounded to 0..31. PSX initialization activates
16 descriptors, which is distinct from the bank's 32-slot storage domain. Negative
direct codes do not use the separate virtual-axis query at 1439D0 (slot+14).
Code -1 instead aliases axis-history data preceding the bank. Other negative or
extreme values may reach still different fields or wrap original address math.
A permitted query outside the native domain returns unavailable/error, preserving
outputs; it never invents a virtual-axis fallback or a negative event bank.

The 48X reader still skips configured -1 only in its two proven action slots.
Negative codes in other queried slots therefore remain an explicit unsupported
connected configuration when the reader reaches them. Recovering a complete
history-backed policy or verified configurations is future work.

## Context-mask gate

Original 143600 first resolves manager+34. Without a context, only masks 0,
80000000 and 3F pass. With a context, helper 143E30 uses root byte+228 bit 2,
context word+518 and context word+1898 in this order:

1. If root bit 2 is clear and mask contains 80, reject
2. Mask 3F always passes
3. With root bit 2 set, mask 80 passes when context+518 equals 1
4. Mask 80000000 passes exactly when context+1898 is zero
5. A mask containing 40 must equal `(context1898 | 40)`
6. Otherwise any intersection with context+1898 passes

No symbolic modifier/menu names are inferred. Mask 0 differs between absent and
present contexts. Gate rejection precedes record-index access. The recovered
player reader always uses 3F, so its valid direct held queries need no clock or
context-dependent event lookup.

## Packet sampling and transitions

The native sampler accepts caller-supplied readiness, SDK read outcome, 32 packet
bytes, resolved descriptors, active count and prior records. Its count domain is
0..32; the verified PSX initializer uses 16. Transport calls are explicit inputs,
not executed by this library.

- Not ready: preserve the entire bank, produce no transitions
- Ready: save old held values, clear held/value for active records only
- Read failure or packet[0]!=0: keep those clears, preserve auxiliaries and emit
  no release transitions. The original separately supplies neutral axis values;
  this button-state API does not fabricate whole-device scheduling
- Success: decode active-low digital bits as
  `uint16(((packet[2] << 8) | packet[3]) ^ FFFF)`
- Descriptor flag bit 1 selects pressure instead of digital mask. Pressure below 26
  stays cleared; pressure 26..255 gives held 1 and value `(byte+1)/256`. The digital
  mask is ignored for a pressure descriptor, including below the threshold
- A digital descriptor sets held/value to 1 when its mask intersects decoded bits

A changed held word produces an event in ascending descriptor order:
- Release: clear record+10 and+8
- Press: clear record+14 and+C
- Event words are `[buttonCode,2,deviceId,newHeld,3F]`

Equal held states emit no event. Pressure changes while held remains 1 are not
new press events. All unmentioned record words and inactive records survive.
The sampler preserves the original numerical distinctions between not-ready,
read failure, held queries and transitions.

Events are returned in a fixed 32-entry batch. The original 127FF0 appender writes
these words and increments the manager queue count. Native code does not append
to an unproven shared queue or choose an overflow policy. Later original manager
code rewrites each event's final word from context+1898 (or zero without context)
and dispatches application callbacks. Neither rewriting nor dispatch occurs in
this milestone. The returned 3F is explicitly the appender's initial value.

## Connected ready-frame API and ownership

`stepReadyHeldPlayerControls48Y` requires a successful-format ready packet and
uses the freshly produced bank as 48X's native query provider. Supported queries
are now answered from concrete held state; tests no longer invent their boolean
outcomes for this connected path. The provider omits the original reader's unused
scratch-record copy; standalone query tests cover the exact copy separately.

The function connects supplied packet/state data through the already recovered
axis conditioning, overrides, shaping, rate limiting and model-field binding.
This is a declared dataflow composition, not proof of the original frame/substep
scheduler or event-callback ordering. Failure/readiness state is available through
the separate sampler; the ready wrapper does not invent when gameplay should run
after a failed hardware read.

Results commit only after successful validation/computation. Query misses preserve
optional payloads; invalid direct indices preserve both match and payload outputs.
Typed same-record/distinct-record copies, reusable state aliases, failed pressure
offsets and late player/binding failures are tested. No global queue is mutated.
Core production is serial and allocation-free; descriptors/state are reusable.

## Original evidence and proof boundary

ELF SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Frozen query/gate/producer/queue blocks total 333 words:
- 61 query words `[143600,1436F4)`
- 49 gate words `[143E30,143EF4)`
- 188 packet/transition words `[237EE0,2381D0)`
- 35 queue-appender words `[127FF0,12807C)`

All 323 reachable words are exercised. The 10 excluded words are documented in
the manifest: duplicate setup after unconditional transfers and the impossible
negative conversion path of a source LBU (its value is 0..255). They are retained
as exact original bytes, not silently removed.

The oracle executes original gate and queue instructions, complete query
prologue/epilogue, branches, delay slots and branch-likely annulment in bounded
synthetic memory. SDK read copies supplied packet bytes through an explicit hook.
Axis-list updates are trace hooks for the separately recovered 48X dependency;
they have no claimed button-bank effect. Not-ready is the separately verified
original pre-clear branch, outside the packet-body interval.

The reference is the project's strict host binary32 convention, not a complete
EE exceptional-value emulator. No timing frequency is inferred. The held query
needs no history clock. Current readiness polling, SDK transport, rumble, original
history rings, shared queue capacity/ownership, application callbacks and complete
gameplay scheduling remain outside the implemented boundary.

## Validation and footprint

Final counts and exact source hashes are recorded in validation.json and the
checkpoint. Coverage includes complete mask branches, all pressure byte values,
original-word record copies, descriptor flags, prior/untouched fields, transitions,
aliases, failure paths and all earlier input regressions. An independent harness
passes 718,243 checks over 74,352 packet and 4,000 query comparisons normally and
under ASan/UBSan, including arbitrary record payload bits. A separate profile
review passes 97 field/count checks against the 156-instruction setup extraction.

Native sampler/frame results are 1,416/1,532 bytes on this Linux x64 host. GNU size
reports 2,360 bytes in the new object's text/read-only category and zero data/BSS.
GCC estimates 1,368 bytes for the sampler frame and 1,664 bounded bytes for the
composed frame; nested calls and caller storage are additional. These are host
observations, not a whole-frame memory bound, Xbox memory guarantee or FPS claim.

```
python3 tools/gameplay/device_held_recover48y.py --check
python3 tools/gameplay/device_held_provenance48y.py --check
python3 tools/test_native_device_held48y.py --output-dir ../held48y-release --stack-usage
python3 tools/test_native_device_held48y.py --output-dir ../held48y-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_device_held_sensitivity48y.py --output-dir ../held48y-sensitivity
```

Use new/empty output directories outside the tree. CMake options are
`RR_NATIVE_DEVICE_HELD_ONLY` and `RR_NATIVE_DEVICE_HELD_SANITIZERS`. CMake is absent
and was not executed. LeakSanitizer is explicitly excluded due to host ptrace;
ASan and UBSan are required in the sanitizer run. Windows/Xbox, rendering,
physical hardware/game-loop input and full gameplay/FPS remain untested.
48X and all earlier source/evidence are preserved. Collision capacity remains
unresolved; its grouped-query/detector source and evidence are unchanged.
