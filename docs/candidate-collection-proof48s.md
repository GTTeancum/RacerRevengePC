# Candidate collection: initialized original execution 48S

Date: 2026-10-06. This proves a bounded, explicitly initialized candidate collector,
not a live world, registration system, playable race, or recovered object owner.

## Frozen source and independent results

`tools/gameplay/candidate_collection_fixtures48s.py` starts every gameplay case at
authentic `0x122350`. It executes original instructions and real delay slots until
`JR0x12264C` naturally returns, or stops at a named reached dependency. It never
patches an original word, intercepts a helper/virtual return, injects a candidate
list as an iterator result, calls native candidate code, reads guest RAM, or
changes the frozen 48R artifacts.

The 239 cases contain 224 natural returns and 15 explicit boundaries, 282,725
executed instructions, 1,849 covered PCs, 223 conditional branch PCs, and 121
branches observed in both directions. All cases have `data=b''`, an empty allowed
source-address set, strict byte initialization, and `source_reads=0`. Every
executed read must find an explicitly initialized byte or one genuinely written
by an earlier original instruction. The interpreter's observations cannot supply
bytes or alter guest execution.

- Authorized ELF SHA-256:
  `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`
- Independently mapped original words:
  `05ce6db129c12888b32c7892526a957c745e6108e5bf1d602d913c711f735863`
- Original dispatch/static data:
  `8ee2d9c392e9e54994ff8d6f0ca0060c361b85135bcfeb1ffbfc81fa27556d30`
- `evidence/candidateCollection48S/native-fixtures.json`:
  `a737d045269c03ec44bd71cefa90b895b3d63aacb169e6fb02fd913ab7c807e9`

Both the authenticated-ELF run and `--frozen` replay produce identical fixtures.
The first 215 case objects are preserved byte-for-byte as JSON values and retained
in `archive215`; the 24 additions extend pool, fallback, alias, recursive selection, diagnostic and stale-slot
coverage without rewriting those earlier expectations.
`proof-report.json` records full source coverage and branch outcomes;
`semantic-checks.json` records 32 named behavioral assertions. Native statuses in
the packet label the bounded interface's rejection policy; they are not fabricated
original return values. The original residual `v0` is recorded separately.

## Actual synthetic spatial structure

These are deliberately synthetic objects, not a reconstructed snapshot. The
query's `+E0` identifies a linked sequence of zones, each holding real method
pointers at `+34/+38`. Null `+E0` cases initialize and execute the original global
fallback source `0x443148`. The optional final zone `0x443108` is explicit too.
The runtime adapter may resolve these explicit identities into indices; that does
not establish the live world's default source or its ownership.

Every method uses the authenticated QuadTree vtable and genuine `265040` open,
`2655C0` next, `265C40` rewind and, on cache replacement, `265C60` close. The
`spatial_layout` records actual synthetic addresses for:

- source nodes and their four-child arrays
- original item-block chains, each with four object-pointer slots
- cached selected-node links, separate from source nodes
- pool descriptors, preallocated chunks and individual selected-link records

`initial_state` and `expected` retain query/candidate/group state, tree cursors,
caches and epochs, pool counters and links, and all declared pool-entry tails.
Available pool records contain explicitly initialized, valid stale links. An
ordered `items` field is only input for constructing authentic four-slot blocks;
it is never substituted for the original spatial algorithm.

Uncached cases execute `265330/2654C0` node selection and real `105720` pool
acquisition. Allocation-free preallocated storage suffices in the returning
cases. Dedicated cases execute free-entry pops, reserve-chunk adoption with and
without an active tail, multiple active chunks, and reset appending an existing
reserve chain. Pool byte counters are measured even when free reuse leaves the
current chunk used count unchanged. Cache replacement executes real pool reset. Pool exhaustion stops at
`105870`, before an allocator is called. No allocator stub, successful allocation
assumption, or forced failure return is used.

The original runtime type IDs are explicitly supplied values, including alternate
IDs and equal direct/group IDs. Initial ELF type bytes are not treated as final
runtime class IDs. Debug enable/init state is explicit; enabled assertions execute
real `133660` with a valid method and return naturally. Diagnostic output helpers
are not stubbed.

## Behavior that must not be normalized

The cases establish the following, including stale-field effects:

1. `*count` clears before the query-enabled gate. Disabled queries clear bit5 and
   return; enabled queries set bit4 before bounds and spatial work.
2. Source iteration occurs before the signed capacity test. Capacity0 still
   opens and advances the tree, stamping the first object. Capacity64 may stamp
   the65th object without changing its bit4 or the query's current identity.
3. Direct candidates set current identity and bit4 before sphere/filter rejection.
   The sphere skip argument does not bypass `120E70`. Both caller tuples
   `(2,1,1)` and `(1,0,0)` are covered, as are mode-bit combinations.
4. Null slots and same-epoch repeated objects are skipped by real QuadTree next.
   Two trees with equal local epochs can suppress a shared object; unequal epochs
   can return the same object again.
5. Switching to a new method returns its first next result directly. An empty
   middle zone can end enumeration despite populated later zones. Positive
   fallback reopens an earlier zone. Distinct source links can share one zone
   identity; that identity prevents an erroneous additional fallback traversal.
   With later-method diagnostics explicitly disabled, a null later method reaches
   fallback/end. A null middle zone with no fallback skips remaining zones; with
   fallback, the retained list cursor yields measured order `[1,1,4,1]`. Enabled
   diagnostics instead reach a named external-output boundary. Initial null
   methods remain an invalid dereference boundary.
6. Cached rewind resets the node cursor and slot but preserves `+B0`. An
   interrupted block therefore carries over. Stale slot5 or INT_MAX skips the
   already-overfull block without dereferencing its nonexistent slot. A disjoint open returns without
   resetting the old cursor, which can subsequently yield stale cached entries.
7. Genuine uncached whole-subtree selection prepends links. The covered root and
   four children enumerate `[root,child3,child2,child1,child0]`; this is measured
   from source execution, not a host sorting rule. Depth4 full and mixed
   partial/full trees also enter recursive265534 and retain measured multilevel
   prepend order `[1,3,7,2,4,6,5]`.
8. Groups do not run direct-member sphere or `120E70` filters. Each member updates
   current identity and bit4, but the output appends the *outer group identity*.
   Nominal capacity1 can therefore produce three group entries. Member shape4/5
   translations and scale words change through actual `135CF0` instructions.
   Other nonzero supported member shape tags are returned without those writes.
9. A member may be the same object as a direct candidate, another member, or the
   query. There is one shared epoch/flag state per original identity. Equal direct
   and group type IDs take the direct branch first.
10. Query shape3 bounds run `1202E0` and its own min/max/vector/matrix helpers,
    transform endpoint points using explicit cached scene matrices, and update
    the query's cached center/radius. This does not reuse48Q ray preparation.
    Shape1/6 bounds preserve the query's cached sphere. Signed zero, fractional
    quantization, rotations/shears and per-endpoint scene presence are covered.

All 239 cases verify that the output suffix beyond the actual appended count
remains unchanged. Physical output backing 128 is separate from nominal capacity;
this permits faithful observation of group expansion without a host overrun.

## Exact boundaries

The 15 partial executions cover: a null selected method (two mode masks),
nonpositive shape3 pair counts before extreme-sentinel arithmetic (two cases),
unsupported query bounds tags (six), pool growth, a null returned group member,
the unrecovered group shape0 matrix-setter integration, and two enabled
diagnostic failures that reach external output at3075A8. Their original partial
mutations are retained in the evidence. The native bounded API is transactional
and must reject without committing them; source partial state is not represented
as a successful collection.

Shape0's setter words are preserved by the source map, but its full typed shape
and descriptor contract is not admitted by this collector proof. Stopping before
that call does not claim the function is intrinsically unsupported. Live index
construction/registration, unknown virtual methods, allocator execution, dirty
scene ownership, and unrestricted malformed pointer graphs remain outside scope.

## Local interpreter additions and scope

The shared earlier interpreter is unchanged. The 48S subclass adds exact
instruction semantics needed by newly closed paths: signed64 BLTZ/BGEZ,
unsigned64 SLTU, SD64, MOVN64, signed32 MULT with sign-extended LO/HI and optional
Rd, finite in-range EE CVT.W.S truncation, and correct scalar-load64 sign/zero
extension. Upper128 lanes remain preserved where required.

`isa-checks.json` contains 47 isolated authentic-word checks. These explicitly
include positive64 values with bit31 set, true negative64 values, real BLTZ delay
slots, poisoned upper lanes before LHU/LW/LB/LBU, negative and fractional
conversion, multiplication high/low results, and MOVN upper-lane preservation.
These isolated instruction-model checks are not described as gameplay-entry
proof. Collector cases themselves always start at122350.

PCSX2's local pinned `third_party/pcsx2-scalar47m/R5900OpcodeImpl.cpp` provides
load, arithmetic and move reference semantics. Its SHA is recorded in the audit.
The [PCSX2 branch interpreter](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/Interpreter.cpp)
uses signed64 `SD[0]` for BLTZ. The
[PCSX2 FPU implementation](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp)
uses truncation toward zero for in-range CVT.W.S. The 48S extension rejects
nonfinite/out-of-range conversions rather than asserting physical EE exceptional
arithmetic fidelity. Original decoded opcodes and source provenance are also in
the frozen map.

The previously missing BLTZ instruction could not silently execute in older
proofs: the shared interpreter would reject REGIMM. Older scalar loads did leave
stale upper32 within the low64 register. Whether that affects a proof depends on
later consumption. The separate read-only audit
`evidence/candidateCollection48S/audit_prior_upper_loads.py` replays frozen48P/Q/R
cases both unchanged and with only scalar-load64 extension corrected, compares
complete captured results, and verifies that prior evidence files stay identical.
The audit covers 753 cases: 338 in 48R, 170 in 48Q and 245 in 48P. All numerical and
state outputs, residual low32 returns, events, calls, byte audits and boundaries
are unchanged; 395 prior evidence files remain byte-identical. There is one
qualified diagnostic: 48R `strict-threshold-3fffffff` changes
`last_v0_change_pc` from 219770 to 219750 because that observer compares the full 128
register and earlier scalar loads propagate different upper lanes. This does not
change a native expected value or the residual return. The archived diagnostic
must not be cited as proof of a physical EE full-register writer. The durable
result is `prior-upper-load-audit.json`; this audit is intentionally separate
from the collector fixture corpus and does not cover all earlier milestones or
arbitrary GPR behavior.

## Reproduction

Run from the source root, without installing anything or running a native build:

```
python3 -B tools/gameplay/candidate_collection_fixtures48s.py
python3 -B tools/gameplay/candidate_collection_fixtures48s.py --frozen
python3 -B evidence/candidateCollection48S/audit_prior_upper_loads.py
```

The first two write only48S collector evidence. The audit only reads prior
artifacts and writes its own48S report. Native runtime validation is a separate
step; none of its outputs feed original expected results.
