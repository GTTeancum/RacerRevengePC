# Construct private RA lighting snapshots in place

UNTESTED source change. No execution checks, allocation measurement or FPS gain.

In tests/native/ra_static_lighting50la.h, Binding::prepare now emplaces a Snapshot
in the already-reserved snapshots_ vector and fills that row directly, rather
than filling a stack row and copying it with push_back at the end. The recovered
normal metadata append remains at its original location.

All Snapshot members have explicit default member initializers; both forms
start with the same declared member values. The selected surface/placement,
camera identity, visual matrix, authored/native anchor, light selection and
normal-input operations retain their order. All validation and currentness
checks remain. The row reference is used only within its iteration; pushes into
separate normal vectors cannot invalidate it, and no row reference survives the
next snapshots_ insertion.

The aggregate creates a fresh private Binding in a local unique_ptr. It publishes
that pointer only after prepare succeeds; rejection or exception destroys the
unfinished object. Thus the earlier insertion of a private incomplete row on a
failed path cannot expose a partial lighting publication. The exact lit-row
reserve and final count checks remain. Public read still copies a validated
snapshot to the caller, preserving ownership.

This eliminates one source-level aggregate copy per prepared lit surface.
It does not eliminate heap allocation or reuse snapshots across publications.
Compiler optimization and actual runtime benefit are unmeasured. A successor may
validate success, stale input, malformed source and exception cleanup paths.
