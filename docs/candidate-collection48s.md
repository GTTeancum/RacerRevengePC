# Bounded candidate collection (48S)

## Implemented boundary

`src/game/candidate_collection48s.{h,cpp}` implements the closed, serial projection
of original `122350`, spatial iterator `294590/294600/294740`, authenticated
QuadTree methods `265040/2655C0/265C40/265C60`, selection `265330/2654C0`,
and the preallocated routes of `105720/105990`. It consumes explicit synthetic
caller-owned spatial structures. It is not a live game-world binding, recovered
world registration, ownership system, general virtual dispatcher or allocator.

The independently frozen source map is `docs/candidate-collection-map48s.md`
and `evidence/candidateCollection48S/map/`. The authorized disc ELF hash is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`;
`original-words.json` SHA-256 is
`05ce6db129c12888b32c7892526a957c745e6108e5bf1d602d913c711f735863`.
The map contains 28 blocks, 2,955 words and independent virtual-dispatch data.
Freezing a routine is not itself native behavioral verification; the separate
native test report identifies executed comparisons and guard coverage.

Collection bounds call the additive `prepareCollectionBounds48S` implementation.
Its tags 1/6 preserve the query's cached sphere; tag 3 recomputes it from explicit
endpoint bindings and cached scene matrices. Unsupported bounds remain explicit.
No detector-stage endpoint rewrite, parent-scene refresh or ownership inference
is introduced.

## Typed API and storage

`collectCandidatesBounded48S(input, initial, workspace, destination, result)`
uses these separate domains:

- Immutable object/filter/group descriptors. Filters use borrowed arrays with
  signed counts and explicit physical capacities; no owning vectors are copied.
- Immutable spatial nodes (four child references and an item-block head), linked
  four-slot item blocks, tree definitions, zone objects and separate zone links.
- Compact mutable object state: shared 16-bit stamp, F8 flags, cached sphere and
  the supported group-member shape fields.
- Compact tree state: 16-bit epoch, flags, cached integer rectangle, selected-node
  heads, item-block/slot cursor, auxiliary B4 and blocked898.
- Preallocated pool definitions and compact mutable pool/chunk/entry state.
- Caller-owned mirror buffers for transactional work and a bounded traversal
  stack. These are workspace, never a pre-enumerated candidate list.

All references are checked index-plus-one values into their named tables. Zero
means null; they are never guest or host pointers. Object identities are opaque
nonzero 32-bit values. Zero is reserved for null/current/list sentinels: allowing
a real object to use zero would make an original null filter-list entry match
a nonnull object. Object identities must also be unique so that borrowed
identity-list values have an unambiguous object meaning. Repeated spatial/group
references must use the same object reference, including the query itself.
Thus stamps and flags are genuinely shared across all tree appearances.

`zoneHead` references a zone-link table entry. Each link separately references
its zone object, permitting repeated links to one zone and authentic fallback
identity comparison. A null original registration chain substitutes global
443148, whose ownership is not recovered here: callers supply the resolved
explicit chain. `fallbackZone=0` explicitly supplies a null optional443108;
a nonnull value supplies its actual zone identity. Unknown reached methods yield
`UnsupportedMethod`; missing methods/invalid links are not treated as empty.
The initial null method reaches an unsafe dereference and always reports
`MissingMethod`. Later null selections can reach diagnostics or fall through to
fallback/end when diagnostics are disabled. `laterMethodDiagnosticsEnabled`
defaults true and reports `MissingMethod` at those later boundaries. Setting it
false explicitly supplies the precondition that both later-zone/fallback
assertion paths are already initialized and effectively disabled; under that
condition the original natural null/fallback behavior is implemented. The API
does not initialize debug globals, call diagnostic output or guess their state.

The query is one entry in the shared object table. `input.bounds` supplies its
shape input and cached scene bindings, but the helper's cache input is taken
from this shared object's initial cached sphere. If the query also appears as a
group member, its already prepared bounds shape is authoritative; the duplicate
member-shape descriptor fields cannot invent a conflicting query shape. `initialCurrentIdentity` and
`initialActiveTree` supply the other preserved state. Result count always starts
at zero, as in the source. No live-memory addresses are retained.

## Source-order behavior

1. Query F8 bit 1 gates activity. The disabled path clears only bit 5, preserves
   query current identity and every spatial state, and writes count zero. It
   does not consume bounds, sphere or method values. The active path sets bit 4
   without clearing bit 5.
2. Bounds are prepared, then the first selected method is opened. The method
   mask prefers the zone's first method under bit 1 and otherwise its second
   under bit 2. Same method references are not reopened as a second method.
3. Every collector loop performs iterator next *before* comparing signed count
   to signed nominal capacity. Zero/negative capacities still open and advance.
   A full result can therefore stamp and advance one additional object, without
   setting that object's F8 bit 4 or changing query current identity.
4. The spatial wrapper first advances the existing method. After exhaustion it
   chooses one new method, opens it, and returns that method's first next result
   directly. A null first result ends this collector even if later zones exist.
5. A direct object becomes query current and gets flag bit 4 before optional XYZ
   sphere testing and the bilateral eligibility filter. Rejected objects retain
   those effects. The identity lists preserve signed-count/mode semantics;
   empty identity lists bypass their modes, while empty category allow lists
   reject. No sorting or deduplication is introduced.
6. A group performs its XZ coarse test unless skipped. It then resolves members
   in order, computes the source's common translation/scale arithmetic, applies
   supported shape effects, changes member flags and query current identity,
   and emits the *outer group's identity* for every member. No member sphere,
   filter or nominal capacity check is inserted. Repeated members and repeated
   group outputs remain observable.

The source's group member tags4 and5 mutate translation and the two scale ratios
at distinct physical offsets; the typed state projects these same fields under
common semantic names. Other nonzero member shape tags return the member after
common arithmetic without those writes. A null shape is an unsafe reached
source dereference; tag0 requires the additional matrix/cache setter. Both are
currently `UnsupportedMemberShape`, never successful no-ops. The frozen tag0
setter alone is not advertised as implemented.

## Authentic spatial/cache/pool behavior

QuadTree open computes float XZ intervals, swaps reversed ends, and tests the
original inclusive extent limits. A disjoint open leaves the old traversal
state untouched. Its return value is ignored by the wrapper, which still calls
next; stale initialized cursor contents can therefore produce an object.

Overlapping intervals are scaled and converted by truncation toward zero only
after finite signed32 range checks. The source quantum mask arithmetic is
retained: minima are `converted & -quantum`, maxima add `quantum-1`. Positive
quantum is required in this closed domain; no power-of-two normalization is
silently substituted. Out-of-range or nonfinite conversion is rejected.

A matching active cached rectangle restores A8 from A4, clears AC/B4 and wraps
the ushort epoch. It deliberately preserves B0 and blocked898. A changed active
rectangle resets the preallocated pool and closes the old search before
rebuilding. Fresh open clears B0/AC/B4/blocked898 and updates the active-tree
identity. Query completion never implicitly closes a method.

Selection walks the source four-child order, uses inclusive integer overlap,
and prepends each node after processing selected children. The resulting linked
list therefore visits the parent first and reverses selected child traversal.
A caller-owned explicit DFS stack replaces native recursion without changing
node or allocation order. Full-subtree selection uses the same postorder.
Reachable cycles, invalid references or insufficient traversal workspace return
an error rather than recursing without bounds.

Next walks selected-node links and linked four-slot blocks directly. It advances
the slot before testing/returning an object, skips nulls, and compares the shared
object ushort stamp with the selected tree's ushort epoch. Equal epochs across
trees suppress shared objects; differing epochs may re-emit them. No global
identity set is substituted for this behavior. Supplied cached selected-link and
item-block graphs must be well-formed. Next checks consumed references and uses
bounded hop guards when exhausting chains; it does not certify an unconsumed
cached tail after early nominal termination. Such tails are preserved, not
claimed repaired or fully validated.

Distinct chunk descriptors require disjoint full entry ranges inside the global
entry table, checked eagerly as part of their declared backing contract. Shared
pool/tree identity uses the same table reference, not duplicate overlapping
chunk descriptors. Pool allocation preserves the source priority: unused slots in the active
chunk, then the entry free list, then a spare chunk. It updates used count,
active/spare/free links and byte accounting with the original eight-byte stride.
Close/reset chains active chunks onto spare chunks and preserves entry storage.
Reached pools require nonnegative logical capacity/use, acyclic active/spare/free
chains, a consistent active tail and disjoint active/spare chunk identities. Free
entries cannot belong to spare chunks or the active chunk's unused suffix. Each
selection also rejects re-acquiring an entry already in its new selected list.
These are checked bounded topology preconditions, including before early nominal
termination can otherwise expose a malformed newly built list.
Pool exhaustion returns `AllocatorBoundary` at the mapped105870/305700 call
boundary. It never allocates, guesses success or supplies synthetic records.
The API covers fixed eight-byte selected-node entries; generic pool strides and
an external allocator are outside scope.

## Transaction and safety contract

Nominal capacity and physical backing are independent. The former is a signed
outer-loop limit; the latter bounds actual identity stores, including full group
expansion. A backing shortage returns `InsufficientCapacity` and does not expose
partial output, flags, stamps, cache changes, cursors or pool mutations. Count
arithmetic above INT32_MAX is outside the safe finite output domain.

The initial-state arrays are copied into caller scratch, then all source-order
work occurs there. On `Complete`, exactly the declared world counts of state
and identity prefix `[0,count)` are committed to destination; unused destination
tails remain untouched. Every error preserves *every byte* of all destination
buffers and the result object. Scratch may change on an error.

Before copying or executing, full declared scratch/destination capacities, the
traversal stack and result are checked for alignment, representable address
ranges and pairwise overlap. They must also be disjoint from every input table,
input/initial/workspace/destination descriptor, nested filter/group array, bounds
endpoint table, endpoint binding table and cached scene matrix table, including
inactive tails. Read-only arrays may alias each other. As usual for a C++ span
API, declared storage must genuinely exist and be readable/writable as declared.

These structural checks apply even when values are inactive. Numeric checks are
lazy: ignored W lanes, inactive spheres and unselected methods remain opaque.
All reached scalar arithmetic and intermediates must be finite; radii and half
extents are not additionally required to be positive. Binary32 operations are
separately rounded without contraction. This is the established finite native
model, not a claim to emulate every exceptional EE/VU mode.

Runtime is serial and allocation-free, with constant native stack. Identity
uniqueness and pool/selection topology checks may take quadratic time in caller
table sizes; no console frame-time parity or linear-time validation claim is
made. This favors compact bounded storage over an additional marking array. Immutable
world graphs are borrowed, not cloned; only compact mutable projections are
mirrored. There is no lock, thread, filesystem access, retained pointer, original
code execution, guest-memory read, heap growth or platform install in this API.

## Verified native result

The pinned final run is `evidence/candidateCollection48S/native-final2/summary.json`.
Its runtime/header, bounds dependencies, test sources and fixture hashes were
checked before and after the serial run. The final collector source SHA-256 is
`500c746ca26de2a876e0e36a8fa4db9675cb39c2976f618a205cd2f822c260b7`;
header SHA-256 is
`495d8d17c54bd40f9460db075117586318f408db8c9f675e57a7dccf6ea7c6e1`.

- 239 genuine original-instruction fixtures: 224 natural returns compared
  against native bitwise state, and 15 explicit source-boundary cases.
- Release and ASan/UBSan runs pass 391,437 checks with zero failures and zero
  observed allocation calls; 1,197 native safety cases include 868 exhaustive
  declared-span overlaps and 17 lazy-read cases.
- Ten semantic mutants were all detected, covering capacity-before-next,
  group identity/inner-limit/flags, shared stamp suppression, cached B0,
  child order, direct flags, epoch increments and inclusive sphere touching.
- The measured known-source native stack bound is 1,512 bytes for this compiler
  configuration. Traversal storage is separately caller-owned at 20 bytes per
  frame. This does not assert an Xbox ABI stack measurement.

The complete test methodology, sanitizer qualification, timings and reproduction
commands are in `docs/candidate-collection-native48s.md`. LeakSanitizer is disabled
because this executor does not support its ptrace requirements. Benchmarks are
serial Linux synthetic microbenchmarks, not game execution or console FPS.
