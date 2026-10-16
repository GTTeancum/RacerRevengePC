# Actual race display renderer reset owner

This inactive source-backed join depends on `resetOriginalRenderer2450A0`.
It adds no race destructor call or Pause activation.

`RaceDisplayLifetime50LA::resetRenderer2450A0` uses its actual retained Video,
private renderer and private dispatch aggregate. Current host, thread and both
back-pointers must still agree. A private renderer predicate additionally rejects
active submission, staged frame, diagnostic ownership and blocking fade. This
predicate performs no GL operation and creates no completion receipt.

The helper then performs exactly one real initialized-context drain and clears
only D8 bit 0 after success. A committed image is unnecessary: original2450A0
loads renderer40's table+24, whose actual243A80 table3D2050 resolves to245090,
which tailcalls242EE0(1,3). Using the two-drain camera-start operation or the
image-publication drain would add different semantics. Pending source callbacks
and nonempty source queues remain rejected rather than silently discarded.

Child/display pins are deliberately retained and permitted. Original1A68C4
invokes this reset before later child/world releases. The reset neither deletes
native textures nor retires HUD owners, closes GL, marks the display retiring,
or bypasses checked downstream resource retirement. Even an already-disabled
D8 still executes the drain. On native failure the D8 suffix has not occurred.

## Remaining admission

This is the renderer portion, not complete2543F0 or1A6840. Actual texture manager
253DF0 initializes13C null, but2543AC..CC allocates0x1824 bytes, invokes252C70 and
publishes the actual owner there.2543F0 drains first, calls252EA0(13C,1) when
present, then clears13C.252EA0 releases allocation-owned backing and arrays;
the current registration-list-only lifetime facet does not own those. A default
null added to that facet cannot certify an arbitrary reached reset. The genuine
native counterpart must bind the existing canonical residency/upload backing
and perform its checked retirement at the reached source operation, or prove
that the constructor route never created it. This patch supplies neither claim.

Video44/renderer2C/global3E1C10 cleanup and full source-order race lifetime remain
separate prerequisites. No build, test, syntax check, decoder or gameplay run was
performed for this patch.
