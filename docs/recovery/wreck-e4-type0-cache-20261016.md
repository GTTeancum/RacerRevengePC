# E4 type0 shared resource and reached constructor lifetime

Source-only incremental work against `795d360`. No build, test, syntax check,
project decoder, guest instruction execution, or gameplay was run.

## Actual join

`OriginalWreckRacerStorage50LA::constructFirstE4ToPreparation(application, diagnostic)`
continues the first actual retained piece after D8 append. It constructs the
existing planned E4 Entity, applies11B570(5) and the actual PHYSICSBODY category,
then calls the typed11B920 loader. Success stops at1662A0, before responseE8,
owner9C,bodyA0,11F570 and append. The existing canonical model identity is borrowed;
no second graph, Entity, model-domain row, or world query definition is created.

Inventory binding now admits each authentic `targetMember` / raw[3] f.COL into
its SAME application resolver. COL admission and every lookup additionally require
that resolver's sealed mounted provider to resolve the same case-folded name and
exact retained bytes at its current revision. Missing/conflicting mounts reject.
PSG/MCF semantics and texture ordinals are unchanged. The existing inventory
binding is serial and retains its failed prefix rather than replaying it.

The shared `OriginalStandardCollisionType0Resource50LA` owns actual native
vertices, optional normals, nodes/radii, and faces. Each query shape owns its
resource pin, while signed source ref60 is changed explicitly. A lookup pin is
not a source retain. Cold typed array admission copies the retained immutable
input in source order; warm construction aliases the same arrays and increments
ref60 once. This is host semantic storage, not an original memory-layout claim:
original nodes are36 bytes with radius/ushort children; native radii are separate
and children are32-bit. Original faces are20 bytes; native faces are16 bytes.

## Source anchors

Authenticated SLUS_202.68; VA-to-file offset is VA-0x100000+0x80. Evidence was
read as raw words and existing source, without invoking a decoder.

-1661D8..166248 constructs the target filename using authored stem and original
 3A7678 `f.col`; the inventory already retains that exact authored member.
-166250/166264 allocate/construct E4,166274 selects mode5,166288 writes category.
-166294 calls11B920;1662A0 begins the subsequent callback setup. It is the stop.
-124FF0 allocates the per-query shape;125004 borrows the actual model110.
-125890 reads COL+68 vertex count into shape11C and COL+80 packed-position flag
 into shape108.1263F8 stores root10C=0. COL+64 reverse mode is separate.
-125AC4/125BB0 allocate packed/float vertices100;125CD4/125DB0 optional normals104;
 126028 is the absent-normal NULL store;126038 nodes114;12625C faces118.
-125014 allocates shared resource;125024 installs shape124. Resource08/0C/10/14
 alias nodes/faces/vertices/normals;04/18/1C hold vertex count/packed flag/root;
 matrix20 copies local40.1250D4 sets ref60=1;1250E4 publishes1326A0 association.
-1254A0 dispatches warm type0 to1254E0.125594 installs the same resource124;
 12559C/1255A0 increment ref60 after per-query shape creation.
-11B650 invokes1327D0(resource) BEFORE11B658 invokes11AEE0, even when other
 references survive.1327D0 clears the first matching resource identity, so an
 older uncached resource must never clear a newer resource at the same file.
-11AEFC..11AF04 decrement ref60;11AF10 returns for positive counts. Final release
 frees nodes114, faces118, vertices100, normals104, resource124 in that order.
 The source124-NULL branch frees those same reached arrays. Common11B600 then
 frees shape8C and result90; this type0 loader leaves result90 genuinely NULL.

## Retirement and failure

`retireFirstE4Prefix()` performs the real unlinked leaf reference transition,
then11B4A0/11B600: cache association removal, source ref decrement, final-array
release if zero, shape disposal, Entity count decrement. Failed cold copies keep
reached backing until this same explicit source-null-resource cleanup. Native
resource bookkeeping allocation is not claimed as a retail0x70 allocation.
Cache-publication failure preserves its resource/ref for explicit retirement.

D8/DC retirement rejects a reached live E4. After genuine E4 retirement, existing
D8/DC -> E0 -> D4/D0 -> model -> piece/collection retirement remains ordered.
Only exact source-retired components are omitted from the live-domain view;
canonical rows are neither erased nor replaced. Query backing release checks
that any attempted type0 ownership actually retired.

## Remaining gates

-Continue1662A0 callback165920, piece9C and bodyA0 binding, then11F570 and append.
 The full canonical E4 geometry publication must point at resource-owned mesh
 and radii, never the inventory's independent immutable input copy. Add matching
 canonical published-geometry retirement before enabling that continuation.
-E8, mutual/group filters, later body/effect setup and runtime retirement remain
 separate. All activation and destructive/KO gates remain closed.
-This root join covers the existing first-occurrence constructor only. The owner
 supports genuine warm sharing for later ordered occurrences when their real
 constructor cursors are joined.
-Packed optional vertex normals, reverse-mode1 and empty/single-leaf trees remain
 unsupported by the existing target loader. No decoder format or accepted source
 branch was expanded here.
