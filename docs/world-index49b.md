# Compact immutable authored-world metadata: 49B-WorldIndex

2026-10-06. Cloud/Linux continuation of preserved 49A.

## Real-data production closure

`src/assets/world_index49b.h/.cpp` provides a self-contained resident projection
of the existing validated RRWORLD1 world data. It keeps exact authored placement,
portal, zone and resource-reference metadata for direct native queries. There is
no callback shell and no new serialized cache format.

The existing `RuntimeWorld48F` now owns this index. `assembleCachedWorld48F`
builds it immediately after loading the authored world, before visual-resource
filtering. Therefore all authored instances remain represented, including names
whose drawable availability is unresolved. Existing visual assembly operations,
resource classification and placement calculations are unchanged.

This is source-level integration into the Windows assembly route. The shared
index is compiled and tested on Linux, including its RuntimeWorld48F header
binding. The Windows-specific implementation was not built/run, and missing
scene assets prevent a renderer exercise here. No GPU geometry, visibility or
frame-rate improvement is claimed.

## Authentic corpus and limits of its meaning

Twelve unique retained RRWORLD1 fixtures are available under the historical
48K x64 and x86 world-fixture directories. Each architecture pair is byte-identical.
The corpus contains:
- 186 zones
- 206 portals and 412 endpoint incidences
- 15,340 ordered placements
- 7,180 per-world distinct resource-reference pairs
- 357,608 resource-text bytes

Every file is checked through the unchanged production RRWORLD1 parser, native
canonical re-encoding, decoding of its embedded original SCB bytes and identical
re-encoding. Tests compare the index against every authored object and value.
No new game assets are downloaded, reconstructed or transferred.

The worlds contain static placement/zone/portal data, not the separate TunnelTrack
map/AI spline graphs needed by action 2255B0. That exact blocker and unresolved
post-load index/reset effects are recorded in `reposition49b-blocker.md`. Portal
connectivity and supplemental default-start fields are not relabelled as vehicle
paths, race progression, spawn points or visibility decisions.

## Preserved resident information

Placement records retain all 25 existing binary64 values: position, look, up and
the complete 16-element placement matrix. No float32 reduction, recomputation,
normalization or coordinate conversion is introduced. Signed-zero bits survive.
Zone, ordinal, original ordering, lighting bit and stable resource-pair identity
are retained.

Portal records retain the original portal ordinal, endpoint order and all twelve
binary64 vertex coordinates. The explicit reserved word is zero. Both endpoints
receive an incident-portal index in original portal order. This is an indexing
convenience, not a recovered traversal policy or a claim that portal quadrilaterals
bound closed spatial volumes.

Resource pairs preserve the exact geometry_member and collision_member text.
Pairs are interned per world in first-appearance order. Returned string views
borrow exact counted bytes and are not NUL-terminated. The names are references,
not proof that the corresponding geometry or collision object is present, valid,
loaded, drawable or wired into another subsystem.

Native resident element sizes are:
- Placement: 216 bytes
- Zone: 20 bytes
- Portal: 112 bytes
- Resource pair: 16 bytes
- Incident portal ID: 4 bytes; resource text: 1 byte per character

Static layout assertions include an explicit portal reserved word so no implicit
host padding is used as data. No foreign pointers or original address-space
objects are embedded in resident records.

## Load-time provenance stays separate

Optional `WorldIndexProvenance49B` retains scene-info identity and original
parameter-block/source-offset mappings parallel to resident objects. It can be
inspected at load time and discarded without invalidating the index. Unknown
supplemental fields and complete original SCB bytes remain in the unchanged
RRWORLD1 cache and historical snapshot; they are not silently assigned gameplay
or renderer meaning.

The owning index retains neither `World48F` nor its parameter dictionaries,
original byte buffer, duplicate section strings or pointers into the input.
It remains valid after both source document and input bytes are destroyed.
Old views are invalidated when their owner is replaced/destroyed; copied owners
are independent and should obtain fresh views.

## APIs and ownership

`loadWorldIndex49B` accepts existing RRWORLD1 bytes, uses `parseWorldAsset48F` once
and projects the validated result. `buildWorldIndex49B` accepts a World48F and
reuses `encodeWorldAsset48F`'s complete invariant/provenance validator. That direct
builder creates and discards temporary existing-format bytes; their memory/time
is an explicit load-time cost, not an allocation-free claim.

Both paths are serial and transactional: failure preserves the previous owning
index and optional provenance. Load/build may allocate for validation, resource
interning, temporary staging and retained vectors. Native limits only reduce
existing world ceilings, with additional explicit bounds for reference count,
text and projected payload. They are storage contracts, not original game limits.
Reduced acceptance limits do not cap preceding parser allocations. The byte-load
front door checks asset byte size first, then the unchanged original parser uses
its own hard limits before the lower projection limits are applied.

`WorldIndexView49B` exposes only typed const spans. Queries include:
- Zone by its authored one-based number
- Ordered placements within that zone
- Placement by zone/ordinal or zero-based global ID
- Ordered incident zero-based portal IDs for a zone
- Portal by zero-based ID, preserving its authored one-based ordinal
- Exact resource text by zero-based resource-pair ID

The lookups are direct and allocate nothing. Invalid IDs return null/empty results;
failed resource lookup leaves its output unchanged. They do not call the importer,
load files, inspect geometry or mutate the world.

`copyWorldIndex49B` supplies the explicit caller-owned-memory path. The caller
provides six typed spans for zones, placements, portals, resource descriptors,
text and incident IDs. Requirements report exact element counts and payload.
Copy validates the complete source/index structure and storage contracts before
writing, then returns a borrowed immutable view. Failed copies preserve destination
bytes and the prior output view; the error enum is the diagnostic output.

All nonempty capacity spans must refer to live correctly typed objects. Complete
destination capacity ranges must be disjoint from one another, source arrays and
control objects, with valid alignment and address/size arithmetic. In-place copies
are rejected. The header specifies the error/control-reference nonalias conditions.
After success, do not mutate/reallocate/free that caller storage while its view
or text spans are in use. This is ordinary native typed memory, not an arbitrary
address or guest-memory API.

## Memory and timing interpretation

Exact resident element payload across twelve separate worlds is 3,814,368 bytes:
TA 86,770 bytes; largest SC 938,410. Optional provenance is excluded from that total.
Final requested-heap and peak-load measurements, vector capacities, input-buffer
scope, owner overhead exclusions and timing methodology are preserved in the
benchmark evidence and checkpoint.

The meaningful comparison is retaining a full parsed World48F document versus
retaining this new compact queryable metadata. The previous visual RuntimeWorld48F
kept visual buffers and summary counts, and discarded the parsed metadata. Thus
this integration adds a retained metadata capability; it is not a measured reduction
of the existing renderer's total resident memory.

Measured fresh-load requested-heap peaks exclude the caller's already-owned
RRWORLD1 input vector and any previous output. They do not measure the Windows
World48F-to-index assembly path or transactional replacement of an existing
RuntimeWorld48F, which can overlap additional retained objects.

Load peak may increase because parsed source, construction scratch and new resident
arrays overlap. Resident payload reductions must not be reported as peak-load,
process RSS, GPU-memory or whole-frame reductions. Source-read buffers, allocator
bookkeeping, stacks, executable pages and sanitizer overhead are separately stated
in the recorded measurement method. Host serial query/load timing is not gameplay
FPS, original-Xbox timing or a hardware benchmark.

## Verification and remaining boundaries

Final normal/ASan/UBSan counts, corruption/limit cases, caller-storage failures,
independent review, semantic controls, benchmarks and exact source/fixture hashes
are in `evidence/worldIndex49B/validation.json`. The preceding input suites remain
included without revisiting collision diagnostic work. The historical full 48F
import test needs standalone references absent here; its assertions remain
unchanged. The new real-cache/embedded-SCB checks cover the available authored data.

Use fresh output directories outside the tree:
```
python3 tools/test_native_world_index49b.py --output-dir ../world-index49b-release --stack-usage
python3 tools/test_native_world_index49b.py --output-dir ../world-index49b-sanitized --sanitizers --no-leak-check --stack-usage
python3 tools/test_world_index_sensitivity49b.py --output-dir ../world-index49b-sensitivity
```

CMake options are `RR_NATIVE_WORLD_INDEX_ONLY` and
`RR_NATIVE_WORLD_INDEX_SANITIZERS`. The current native-PC source configuration
adds the component via `NativePc49B.cmake`. CMake is absent and no configuration,
CTest or Windows/Xbox build is claimed. LeakSanitizer is excluded due to host
ptrace; ASan and UBSan are enabled for supported sanitizer runs.

There is no point-to-zone inference, visibility/streaming policy, respawn/path
logic, world-collision integration, shader change or geometry-availability claim.
No PC/RAM, asset transfer, installs or pushes were used. 49A and earlier evidence
remain preserved. The separate collision capacity contract is unresolved and its
source, diagnostic artifacts and guards were not changed.
