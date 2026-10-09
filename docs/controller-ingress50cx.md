# Persistent controller ingress50CX

Source-only integration of existing native48Y held sampling and48X player input
into the staged49K/49W reader. No hardware calls or frame scheduler are added.
Uncompiled and untested under the current team-wide test cap.

## Integration

Add `src/game/controller_ingress50cx.cpp` to the relevant production/driving
target. It needs the existing `device_held48y.cpp` implementation; do not call
`stepReadyHeldPlayerControls48Y` or `stepReadyPresetPlayerControls49D` here because
those perform the shaping/filter/binding which the staged vehicle already owns.

Retain one `ControllerIngress50CX` at a stable address per selected device,
constructed with the caller's fully initialized `DeviceButtonBank48Y` history.
The adapter supplies no default descriptors, button labels, device IDs, preset
selection, initial auxiliary values or host-to-packet translation.

1. Poll hardware through the caller's existing transport. Supply the actual
   readiness, read result,32-byte packet, descriptors, active count and device ID
   to `samplePacket`. False means an invalid48Y argument and preserves the old
   sample. Do not use the old accepted sample as if the failed call succeeded.
2. Read `deviceSample().status` and consume its ordered transition batch.
   `appendDeviceTransitions48Z(ingress.deviceSample(), queue)` uses it directly.
   Append once per successful sample. If appending fails, retain the batch until
   the queue owner resolves capacity; sampling again would replace it. No queue
   count reset, overflow policy or event dispatch is hidden in the adapter.
3. When using the existing48Z dispatcher, bind the selected
   `EventRecordSources48Z.devices[deviceId]` span to
   `ingress.buttons().records` with32 records. Check that the supplied device ID
   fits the dispatcher's eight-device domain. Do not bind a temporary/copy: live
   callback record changes must survive into the next poll. Supply real callback
   owners and live event context. The sampler's initial3F event mask is left for
   that dispatcher's existing live-context rewrite.
4. At the actual Ready49I frame entry, call `prepareFrameReader(queryContext,
   playerContext, selectedActionCodes, preparedFactor, reader)`. It succeeds only
   for an Accepted sample and a finite factor in[0,0.06], preserving signed zero.
   The factor is supplied after the real manager/root gates, never derived from
   a host clock or fixed interval here. The adapter cannot certify that callers
   actually performed those gates merely from the float.
5. Replace the driving client's `{nullptr,noAction}` reader with this returned
   reader and pass it directly to `readVehicleFrame49W`. Continue the existing
   post-player and `bindVehicleFrame49W` stages exactly once.49W still owns its
   canonical damage fields, gate byte and prior player flags; its existing reader
   overrides the corresponding copied `PlayerReadContext48X` members.

The selected action codes may come from the caller's existing
`resolveControlPreset49D` result. Device selection/action indexFFC and vehicle
configuration index5C remain distinct. Keep `configurationEntryNonzero` for the
later binding stage; this adapter neither chooses nor repeats that binding.

## Status, transitions and lifetime

- NotReady preserves the entire bank and returns no transitions.
- ReadFailed, including a nonzero packet byte0, clears only active held/value
  fields, preserves auxiliaries and returns no synthetic release events.
- Accepted uses the established packet4/5/6/7 axis fields and48Y button decoder.
  Axis conditioning and action queries occur once in the existing frame reader.
- A non-Accepted sample does not prepare a frame. Whether the game should run,
  skip or use some separately proven failure path remains a scheduler decision.
- Hardware samples commit independently of the later vehicle read. A rejected
  reader does not undo observed button history or already-dispatched callbacks.
  Retry the vehicle read using the same accepted sample if appropriate, rather
  than polling the hardware twice to recreate an edge batch.

The instance is non-copyable and non-movable so its member provider always points
at its own live bank. Returned `FrameReader49K.actions.context` points at that
member provider. It is valid only while the ingress instance remains alive at the
same address and before another sample or reader preparation. Consume one reader
synchronously, then continue; it is a borrowed view, not a queued snapshot. The
packet, descriptors, codes and input contexts may be temporary because none of
their addresses is retained. The complete prior button record is copied bitwise,
including auxiliary words and nonarithmetic float payloads.

Storage and work are bounded: one32-record sample, its32-transition batch, four
axis bytes and one provider. No new heap allocation, callbacks, filter history,
event queue, menu/boost/reset behavior, polling or timing policy is introduced.
The inherited48Y implementation uses bounded stack staging. No runtime footprint,
allocation or correctness claim has been verified for50CX yet.

## Delivery boundary

Only the new50CX header, implementation and this integration note were changed.
No existing sources, build files, tests, sealed49W, held48U, excluded49C or
unapproved `ee.bin` inputs were modified or executed. No PC access was used.
