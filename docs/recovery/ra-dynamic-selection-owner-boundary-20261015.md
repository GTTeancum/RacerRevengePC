# RA dynamic selection: source ownership boundary

Source review only. No build, test, syntax check, gameplay, guest execution, or
runtime activation was performed. This continuation deliberately changes no C++.
The previous private static method44 candidate remains incomplete and uncalled.

## Authenticated dispatch

SLUS_202.68 SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Evidence uses file offset `VA - 0x100000 + 0x80`; each extracted range has its own
SHA256 in the companion `source-evidence/ranges.json`. Labels are static source
word annotations, not execution results. Unrecognized opcodes remain raw words.

Racer constructor20F390 calls SearchableGameObject constructor27E130 at20F3C0
and installs table3D1790. Derived constructors16E260 and1A02C0 install3CE1E0 and
3CED50 respectively. All three tables have slot40=229AE0. This establishes the
callback address, not a retained native runtime class-table owner. Do not assign
player/AI labels to these derived constructors without separate evidence.

Scanner27EAF0 uses the real searchable facet at racer+30, invokes containing
Entity slot40, then reads racer33.bit1. Static model191 is not that predicate.
No native substitute may set racer33 from static box clipping or put a racer in
the static-model ledger to get past method38.

229AE0 writes racer38=1, initializes shared434820 once, calls224720, optionally
resolves material244 for state40==0 or material26C for state40==4, calls29BFF0,
then invokes detached7F8.slot40 and141C30(racer). The neighboring229BB0 routine
is a separate function; it must not be folded into229AE0.

## Newly established dependencies

224720 is complete through the return delay slot2255A8. It has live source
dependencies beyond clipping: racer254 cable state, renderer442F70 current
scene, Application382B80->race28C->B4 and race1B0, attachment transforms, racer
state40 and speed fields. It updates cable fields and invokes174370 repeatedly;
the final224720 tail calls174360 with racer254, membership7F0 and computed data.
The early null254 branch returns without this work; a native implementation
cannot manufacture null254 to select that branch. Existing buildPodCables50FD
documents only connected state40==0 mesh generation and copies scene nodes into
workspace. It is not the complete original callback or an owner of its writes.

29BFF0 iterates material48 entries in material4C order and invokes each actual
entry's table slot20 (29C014..29C03C). It is not a no-op material demand marker.
The full dependency needs the actual texture owners and callback lifetimes.

141C30 requires parent24 mask1000, walks actual18/14 children in linked order,
checks child29 bit0 and source exclusion bit plus child20 mask1000, services the
actual renderer before EACH eligible child's slot40 callback, then continues.
The retained cold racer prefix does not supply a complete runtime callback
forest. Model aliases alone cannot replace that forest or detached7F8 dispatch.
Neither the recovered229AE0 body nor the cable update establishes a substitute
producer for the selected33 predicate; that producer remains to be joined.

## Neighbor dispatch ordering to preserve

27FC40 returns0 for an already-current A6. Otherwise it stores current A6
BEFORE comparing A4 to the current admission stamp; a nonadmitted zone returns1
without composing its matrix or invoking callbacks. An admitted zone first
calls280700, then actual method34.slot44, then method38.slot44 only if nonnull
and distinct from method34. Only after those callbacks does it recurse through
portal linked order. Starting-zone exclusions belong to admission, not this
walk. Do not prepare every admitted zone's matrix ahead of the callbacks.

Current prepareRaZonePrefix114D00 stops at27F888 after the static method. Moving
neighbor preparation before this stop would reorder source effects while the
dynamic callback is unresolved. Therefore no such implementation is included.
Queue reset remains per physical scanner block, renderer service remains per
block, and no copied, sorted, deduplicated, or dummy-success queues are added.

## Required owner join before implementation

1. Bind retained searchable identities to their actual containing racer and
   full source-class callback owners, including detached7F8 and eligible linked
   children; retain original AI ownership for every racer.
2. Join224720 mutations to actual cable, race, renderer scene, graph and shared
   state owners. Recover the selected33 producer and actual material texture
   callbacks, preserving their failure and renderer-service order.
3. Invoke the sequence through genuine Application root26C, race/scene ancestry
   and published SourceCamera114D00. A caller matrix, ready flag, direct camera
   hook, or borrowed racer DrawLease is insufficient. Any extended camera/scene
   lifetime must be explicit and typed.
4. Join remaining fallback environment443108, packet/state emission and actual
   scene render transaction before exposing any completion capability. RA
   trigger geometry and runtime/GPU activation remain blocked.

Review checklist: compare the three slot40 table words; inspect complete224720
and29BFF0; retain per-child141C30 and per-block renderer service; confirm A6-before-
A4 order; confirm no native source or cleanup predicate changed. Root owns all
integration and Windows validation. This evidence artifact is not a completed
dynamic-method implementation or a gameplay claim.
