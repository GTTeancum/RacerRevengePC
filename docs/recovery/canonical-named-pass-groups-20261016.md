# Canonical named ShaderPassGroup ownership

Static source recovery, 2026-10-16. No execution or tests.

Authenticated OVERLAYS font/denmarkbold16pause.psm and both arrow Pause PSMs
specify `ShaderPassGroup = pause`. This is an actual named group, distinct from
literal `none` (-1) and `unique` (-2). The prior canonical material constructor
rejected it at 2475AC.

Recovered source ownership: 382FC0 points to process-static storage 435690.
3CA6A0 calls 299130; 3CA6B8 registers 299190 cleanup. Final vtable 3D3670 slot1C
is 299330. 2475AC calls it with the authored name and value zero. 299330 searches
owned names in insertion order using 308F00 case-insensitive comparison; a match
returns its existing index without reference mutation. A miss calls 298900,
appends a row with value/ref zero, and owns a copied name. Capacity grows by16.
299190 releases every owned name and registry backing at process retirement.
No pass retain/release accompanies this lookup.

The native process-static owner mirrors the consumed name/index semantics.
Only the canonical material constructor can append; collector admission checks
nonnegative indices against the same owner. Values/references stay implicit zero
because this bounded path has no producer or consumer for changing them. Actual
material construction retains its source ordering: parameters, named lookup,
then later admission/publication. A failure after name insertion preserves the
registry entry. All actual native strings are owned; no archive or pass pointer
is retained by the process registry.

Growth beyond signed-halfword representability rejects before index aliasing.
This is an explicit native bound, not guest integer-wrap emulation. Existing
none/unique branches, unique nonmerge behavior, named group equality and all
remaining collector state comparisons remain unchanged. This change neither
alters authored material passes nor supplies animation clock service.
