# Type2 endpoint allocation and W-word provenance

Static original-byte inspection only. Original SLUS_202.68 SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Listings in this directory were produced by the retained data-only word formatter; no guest execution or runtime observation. This supplements ../ra-trigger-reachability/PLAN.md and the retained descriptor-history proof.

## Bounded conclusion

The actual type2 loader allocates uninitialized backing. Neither the identified preparation branches nor the identified transform/setter path writes endpoint offsets12/28. Consequently those bytes are not determined by the authenticated COL file, nor by a zero-initializing allocator contract. They depend on the actual returned allocation's previous payload history. This is a concrete unresolved lifetime dependency, in addition to the separately unresolved retained vertex descriptor3DFE60. Reachability of main690/type2 into sphere5 remains established conditionally on the real spatial/filter schedule; this finding neither proves a particular hit nor licenses a replacement geometry test.

## Exact allocation chain

-1253DC allocates the0x60-byte type2 shape through2E8D90.1253E4 stores it at query+8C;1253F0 writes shape44,1253F8 writes normal mode54=0;125400 calls126640 with that actual shape. These are not writes to the later endpoint backing.
-126D38 loads the file endpoint count.126D3C calls2E8CA0 with count<<5 in its delay slot126D40.126D44 stores the returned pointer at shape+50.
-2E8CA0 is a plain allocation wrapper:2E8CA8 calls2E8D90 and2E8CB4 returns the result. There is no array-element constructor, cookie adjustment or payload clearing here.
-2E8D90 retains the requested byte count; zero size is changed to1 at2E8DB0.2E8E20 calls305700 and returns a successful result through2E8E34. The failure/new-handler retry branch does not provide payload initialization.
-305700 supplies allocator context from38544C, calls305990 at305714 and returns its result.
-305990 rounds the request using(size+19)&~15, minimum16. The reused small-bin path3059EC..305A2C unlinks free-list headers and marks the next chunk in use, then returns through3060DC..3060E4 as block+8. No payload clearing occurs. The reused large-bin path305FF8..306020 likewise returns block+8 after metadata updates. Split/top paths write allocation/remainder headers, not a zero-fill loop over requested bytes.
-306538 and306540 are both JR ra/NOP. They do not hide a clearing implementation.

A concrete successful reused-block return is sufficient to exclude a guaranteed-zero allocation contract. This does not assert which bin or prior allocation the actual game uses for any given endpoint array, or what bytes an initial system allocation happens to contain. No BSS inference applies to this heap payload.

## Loader writes to the backing

126D50..126E10 loops over32-byte pairs. Calls to12FB30 explicitly request one4-byte field into offsets0,4,8,16,20,24 at126D70/8C/A8/C4/E0/FC. Offsets12 and28 are skipped.126E18 writes the root index at shape40, not endpoint data. There is no post-read vector construction loop before return126E30.

## Preparation and transform writers checked

Actual table words388A00 select11FA30 for type2;388A20 selects11FED8 for type2's scene-transform wrapper.

-11FED8..11FEF4 obtains the matrix from actual shape44 through28B550 and calls11F680.11FF60 invokes this wrapper and then120230 membership update;11FF90 supplies an explicit matrix to the same11F680 preparation.
-11FA30..11FA64, when shape4C is nonnull, copies64 bytes into the shape's matrix00..3F.11FA78..11FAE4 reads the node bounds and writes query center60..6C/radius80. The vector store at11FB14 targets the query center address established at11FAE0. It does not address shape50's allocation.
-When shape4C is null,11FB1C..11FB38 still copies only the matrix.11FB44..11FB5C and11FBB4..11FBCC read endpoint XYZ. Remaining writes are local temporaries, query center60 (11FBAC), query radius80 (11FC4C), and transformed query center (11FC7C, address established11FC48). The explicitly created W=1 values at11FBA4/11FC14 belong to temporary bounds vectors, not endpoint W.
-11FFC0's type2 branch12003C..12005C writes translation at shape30/34/38. It does not dereference shape50.
-11DD00's type2 reset tail11E2A4 operates on the separate query+90 result allocation through124F70, as retained in the earlier descriptor-history proof; it does not initialize endpoint backing.

Unknown arithmetic/vector opcodes in listings are not executed or guessed here. Destination register/address provenance distinguishes their query-cache stores from endpoint memory; no endpoint pointer is supplied to those stores.

## Similar-looking constructors that do not close the gap

-11BBC0 allocates query+90's result object and later constructs its auxiliary arrays (including a result-object+50 array around11BC90). This is distinct from query+8C's type2 shape+50 backing. Its vector constructors cannot establish endpoint W.
-11CA80 explicitly sets query+88=3 at11CA88, then copies supplied16-byte vectors into shape+8. This is the explicit type3 route, not a hidden type2 endpoint initialization. The ordinary main query route with detector60 absent instead uses11FF60 at2959EC.

## Remaining exact input boundary

For the actual type2 allocation, source-faithful W values would require establishing the returned block identity and its previous payload writes, or proving some additional concrete alias writer before108E90. The reviewed actual construction/preparation path supplies neither. A broad reconstruction of allocator/guest-memory history is outside this bounded recovery and is not proposed.

The endpoint/20-byte-leaf reinterpretation remains independently relevant:108EE4..108EF4 selects20*(tag&7FFF), and normal shorts12/14 of leaf0 overlap first endpoint W. Other leaves can cross32-byte pair boundaries. No native implementation may silently substitute zero/one for those bytes, infer valid indices, or replay unchecked external memory. Keep the canonical native caller closed until its actual required input ownership is established.
