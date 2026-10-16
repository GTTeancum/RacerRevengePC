# Rechecked type2 / sphere5 descriptor history

Static data inspection only; no guest execution, runtime observation, conversion or central edits.

## Object identity is verified, not inferred from variable names

-122660 saves a0 as s5 (actual querying collision object), a1 as s4 (candidate count), a2 as s3 (candidate pointer array).1226B0–BC loads candidate into s1;1226CC stores it in query+F4. All later a1 candidate reloads come from that same+F4.
-122724 reads query+88. Actual jump-table words388B90..388BA8 are `{122750,122760,122770,122780,122818,122794,1227B0}`. Index2 therefore calls121410 at122770 with a0=s5 in the delay slot and a1=the current candidate from122730. No argument reversal occurs.
-12142C loads query+8C into s1 before any use of candidate+8C. The candidate-shape table388AF0..388B08 is `{121468,121640,1218F0,1218F0,1218F0,121884,1218BC}`. Shape5 goes121884.
-121884 alone loads candidate+8C into a0. Query shape is still s1.121888/12188C load query+50/+4C and store them as the two-word temporary descriptor.1218A0 loads query root+40.121894 passes query shape pointer as matrix a3.1218A4/A8 load sphere radius50 and squared54.1218AC calls108E90, delay slot t0=0. The local descriptor is neither a result object nor the sphere's allocation.

## Resource construction and preparation preserve that identity

-124FAC–124FBC read the original resource tag into collision+88.124FC4–124FCC explicitly branch tag2 to1253DC with size0x60.
-1253DC allocates that shape;1253E4 stores it at collision+8C.1253F0 stores field44,1253F8 initializes normal-mode54 to0.1253FC reloads the SAME+8C as a2 to126640 at125400.
-126640 saves a2 as s2.126D38–126D44 allocates count*32 and stores its base at s2+50.126D50–126E10 reads exactly XYZ at offsets00/04/08 and10/14/18 for each32-byte pair.126E18 sets root+40 to0.
-11FA30–11FB14 reads query+8C/+4C, copies only the64-byte prepared matrix into shape00..3F, updates collision cached center60/radius80, and transforms that center. It does not replace shape+4C/+50 or form triangles.
-11DD00 clears globals3DFDC0/3DFDC8 at11DD64/6C. Its shape2 tail11E2A4 resets the SEPARATE collision+90 result object through124F70 and initializes result fields; it neither rebinds3DFE60 nor converts shape+50.

Thus the leaf-stride mismatch remains after checking register reuse, table indexes, pointer-pair order, allocation owner, shape preparation and result reset.

## Exact leaf-byte dependency

108EE4..108EF4 calculates20*(tag&7FFF) from descriptor[1], i.e. actual shape+50. Leaf indices read little-endian u16 at offsets0,2,4; normal shorts at12,14,16. These are not the typed32-byte endpoint layout.

For leaf0, indices overlap the low/high words of first.X and the low word of first.Y; normal12/14 overlap first.W, and normal16 overlaps second.X. Leaf1 starts at byte20, therefore crosses into the next endpoint pair. This arithmetic identifies the input bytes; it does NOT establish that the resulting vertex indices are in bounds or that opaque W values are known. Native Type2ResourceView49X preserves endpoints and explicit opaque-W inputs, not an authenticated triangle table.

## Shared external vertex context

106ED0 and109F10 use pointer3DFE60. Descriptor+0 is vertex backing and+8 selects the vertex format; nonzero selects signed-short triples scaled using exact0x38000100 and source383640. Zero selects the separate float branch. A type2 shape does not itself furnish this external descriptor here.

An aligned static scan of the bounded code region found these direct store sites, each confirmed with its LUI3E instruction:

| PC | Stored value | Context |
|---|---|---|
|1089FC|t3|1089F0 dual-tree query entry;121490/1214A8 supplies target shape+100|
|10A4E4|t0|sphere wrapper10A4E0; tail-jumps108E90, replacing t0 with t1 selection|
|10AF68|t2|box wrapper10AF60, jump delay slot|
|10B724|t0|another recursive query entry|
|111304|a3|ray-array wrapper111300|

No immediateFE60 address-materialization instruction was found in this bounded scan beyond those load/store sites and unrelated stack/branch immediates. This does not prove absence of every indirect, GP-relative or bulk write.

ELF PT_LOAD fields are vaddr100000, file size2D5A00, memory size34F600.3DFE60 lies in its zero-filled tail `[3D5A00,44F600)`, so startup null is established. Null must not be silently replaced by a guessed static mesh.

## Preceding query chronology and limits

The existing source-backed vehicle collector calls method2 first, then method1, and219B1C..219BD0 appends first-list then second-list, capped64, without sorting or deduplicating. Spring2959EC prepares the actual query;295A08 calls122660 with that retained list.122660 evaluates candidates in the supplied order, running sphere/filter checks before shape dispatch.

A preceding admitted shape0 call121468..1214AC invokes1089F0 with target shape+100 and therefore changes3DFE60 even if its final geometric result misses. A prior caller elsewhere can also leave it changed. A later shape5 call121884 reuses that retained context and does not select a replacement. The static source does not identify one unique mesh independent of the actual candidate history.

Existing native vehicle kernels represent their target inputs explicitly and do not currently preserve a canonical3DFE60 context across all five writer families. A new owner would need real source-order updates and retained immutable vertex resources across query boundaries; a per-trigger setter or guessed last terrain index would not close the join.

Reversing the pair is not equivalent:124270 rejects candidate shape2 at1242C0/C4. Only its shape0 branch1242E8..12430C binds target+100 through10A4E0. The native camera selection1 API consequently cannot replace the type2→sphere5 boolean path by swapping operands.

## Proven bounded contract versus unresolved input

Proven numerical boolean path: actual current sphere point; actual prepared query matrix; ordered36-byte type2 nodes and high-bit tags; radius50 and stored squared54;20-byte leaf byte interpretation; external vertex descriptor; two-sided `abs(planeDistance)<=radius`; source109F10 point-in-triangle; three ordered strict edge-distance tests; first hit return. No previous-pose input is provided to this branch. Contact history remains actual CC/D0 state.

Still required before a faithful native caller can be admitted:
1. Actual lifetime/history owner for3DFE60 and every reachable writer in the selected full query schedule.
2. Original endpoint W-word provenance and the exact bounded backing bytes read by20-byte leaf interpretation.
3. Valid source vertex descriptor, format, allocation extent and index bounds after that interpretation. No arbitrary native pointer dereference or out-of-bounds replay is acceptable.

The camera implementation's private arithmetic/inside/edge helpers can be factored AFTER these inputs are established. It must not substitute its front-face selection or contact publication. No source-supported gameplay patch follows from the current evidence alone; the authenticated RA cold authoring remains usable separately.
