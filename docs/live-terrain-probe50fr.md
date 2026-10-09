# Enabled live terrain probe 50FR

`tests/native/live_terrain_probe50fr_fixture.h` connects the enabled `vehicle+348`
frame preparation to its own live membership and the existing bounded 48M terrain
query. Metadata and actual single-pair endpoints come from the separate 50FQ cold
authoring owner. No saved terrain state, guest execution, full-world substitute,
or invented filtering/misses are used.

## Source ownership and order

Static inspection of retained ELF `SLUS_202.68` (SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`,
file offset `VA - 0x100000 + 0x80`) resolves the actual initialized owner:

- `212084` loads `vehicle+348`; `212088` forms `vehicle+488`, and `21208C`
  forms `vehicle+48C`. Stores `AC8301E0` at `212090` and `AC8201E4` at `212094`
  bind the probe's **count pointer** and **array pointer**, respectively.
- `219AD4..219AF4` produces `vehicle58C/590` with method2, skip sphere and ignore
  groups. `219AF8..219B18` produces `vehicle488/48C` with method1, sphere enabled,
  groups enabled and nominal capacity64. Their later merged `384/388` serves
  spring detection. The probe consumes only the second `488/48C` list.
- `1B197C` calls `11FF90`; the reached `11FFA4` prefix updates the probe's own
  `120230/1202E0/280D40` membership after 48M matrix/sphere preparation.
- `1B1984` loads the array pointer at `+1E4`. When nonnull, `1B1990` loads the
  count pointer at `+1E0`, `1B1994` dereferences it, and `1B1998` calls `122660`.
  A zero count still uses the limited-list route. The full `121920` alternative
  at `1B19AC` is not the initialized Pod's route and is not implemented here.
- The list is retained from the first-substep collection. Updating probe live
  membership must not recollect or replace it. Existing 48M preserves the exact
  `122660` filtering, sphere, geometry, callback and `1B19B4` surface behavior.

This distinction was found while inspecting the apparent constructor-null list
seam. A fresh full-world collector would have changed the real source behavior.
The final helper has no such substitute.

## Client wiring

1. Keep terrain refs1..37 and spring refs38/39. Add native terrain ref40 to all
   collector object/state arrays and the lifecycle registration array, using a
   distinct opaque identity supplied to 50FQ. Give it a separate 50EN membership,
   links and entries. Its reference is not a guest address.
2. Cold `initialize(owner, authored.initial.segment)` creates the actual one-pair
   rootless type2 descriptor. Keep Owner at its final address. Attach membership
   to the explicit initial collision zone with `bounds(owner, initialPrepared)`,
   collision overlap radius zero, and the actual second/dynamic method.
3. Resolve `collectionWork.second.identities` and `result.second.count` into a
   dedicated probe index/count buffer during existing `collectRegistered`.
   Do not resolve mergedIds for this owner, even though current query-only
   dynamic entries make the two lists happen to match.
4. Keep pointers to the persistent 37 TerrainCandidate48J owners. Allocate
   selected candidates, effects and traversal scratch once, then call
   `reserveSelected` once at cold setup. Capacity37 covers this client's current
   unique static-owner list; if source collection emits repeated entries beyond
   that capacity, reject explicitly instead of truncating. The source nominal
   list bound is64, so capacity64 is a straightforward conservative choice.
5. After the spring queries, call enabled frame preparation, then
   `query(owner, prepared, authored.metadata, context, probeIndices, probeCount,
   result)`. Context borrows the live lifecycle/world/probe membership/workspace,
   target pointers, selected scratch and bounded48M effect/tree-stack buffers.
   On Complete, pass result to the frame terrain-commit seam.
6. The helper commits target F8 and copied40/50 effects and probe metadata F8;
   mirror those target effects to separate movement views before their next
   query. Refresh the probe's collection projection from metadata plus its
   accepted membership cache before the next outer vehicle collection.

`prefixCommitted` records the successful live membership update if the later
query rejects. Preserve it and report the exact prefix/query status. Narrowphase
result/effects remain unchanged on its error; scratch may change.

The live path allocates nothing. Cold reserve sizes every selected candidate's
filter lists; live resize is preceded by explicit capacity checks. Inputs are
borrowed caller-owned storage and must be backed/disjoint under the underlying
bounded APIs. It does not redo frame preparation or collect candidates itself.

Groups, arbitrary response callbacks and other unsupported geometry retain
48M's explicit rejection. Detached/wrong owners and changed single-pair endpoints
reject. No builds, tests, guest execution or runtime measurements were performed
for this source-only change.
