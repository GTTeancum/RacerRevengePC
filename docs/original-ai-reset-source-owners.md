# Original AI reset: concrete auxiliary owners

Source-only candidate, 2026-10-12. Central source was read only. No build,
test, syntax/import check, asset decoding, game launch, guest execution or
user-computer operation was performed.

The immutable original input is SLUS_202.68, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The adjacent evidence file retains raw address/word pairs; observations below
were interpreted manually. Code/data file offset is VA minus 0x100000 plus
0x80. Raw byte searches and word reads do not execute the original program.

## D44 is PodCollisionResponseEmitter

The literal at 3C1DD0 is `PodCollisionResponseEmitter`; descriptor 3C1E98 is
referenced by vtable 3D4BE0. Its base is `MaterialSoundEmitter`. The pool's
parent is `PodRaceAudioManager`, literal 3AA680, descriptor 3AA6F8 and vtable
3CEE00. The source ownership chain is concrete:

1. Setup 1A3C00 enters an eight-iteration loop at 1A3CA4. Each iteration
   allocates 0x250 bytes at 1A3CAC, calls constructor 2C20F0 at 1A3CC0 with
   a1=1, stores its object at manager.C0[index] at 1A3CC8 and calls 2C21C0
   with the actual manager+50 material setup packet. The counter comparison
   at 1A3CDC is eight. This is session-owned storage, not a frame temporary.
2. Constructor 2C20F0 calls base 2928B0 at 2C2118. At 2C2128 it installs
   vtable 3D4BE0. The stores at 2C2130/2134/2138/213C establish material240=-1,
   key244=-1, vehicle248=null and disabled24C=0 respectively.
3. Actual AI creation 1AE510..1AE540 selects race manager+10C, then C0 indexed
   by opponent ordinal plus the human count in settings1125C bits5..6.
   1AE544 stores that real object in vehicleD44. 1AE554 invokes retain
   virtual0C; 1AE564 appends the object through 141960. 1AE570 calls 2C2230,
   whose delay-slot store is D44+248=vehicle. In the admitted native all-AI
   config, packed1125C=0x0403, the human count is zero and the selected row is
   the actual manager ordinal.
4. Teardown 1A3AF0 visits the parent manager's twelve C0 slots, releases each
   present object through virtual10, and clears the slot. The first eight
   slots contain the material-response objects above. These objects must
   outlive all vehicle borrows and must not be reconstructed during a reset.
5. Reset 216A50..216A84, when D40 is present, clears its real22C bit2, reads
   the real vehicleD44 and invokes 2C2810 with argument0. 2C2818 writes24C=0;
   2C2820 observes zero and skips virtualA8. This path reads neither the
   material arrays nor240/244/248, and preserves their state.

`OriginalPodCollisionResponseOwners` is the narrow native field projection of
that actual eight-row pool. It provides canonical240/244/248/24C storage,
source-derived cold initialization, stable selected-row borrowing and a
terminal close. Its 248 projection points to the actual session-owned racer's
stable identity inside its real D40 owner; it does not point at a bank-switched
frame address or a temporary identity copy.

The pool deliberately does not report resource resolution, 2C21C0 material
setup, source reference counting, child dispatch or playback as complete.
The zero-reset path needs no such receipt. Any later material-audio integration
must consume these same row fields rather than maintaining parallel240/244
or24C. The existing selected-player host material subscriber is not this pool.

### Integration

- Retain one pool outside the mutable racer-bank arena, alongside the stable
  native original-AI audio owner.
- Prepare once with the actual session identity and actual packed1125C.
- After each existing real D40 has completed its cold setup, call
  `bindAllAiRacer(actualD40)` and retain the returned Emitter pointer as that
  stable native racer's D44 borrow. No allocation occurs in reset.
- For the selected racer, obtain `resetField24C(actualD44, actualD40)` and
  pass it alongside `&actualD40.flags22CLow4` to
  `resetUndertrackBasePrefix50ET`. This changes the actual owners.
- Retire every D44 borrow before closing the pool. Close is terminal.

## The extra category is POD

The global3B14B0 is not an unknown permanent zero. Static initializer3C9FF4
forms literal3B3000, which contains `POD`. 3C9FF8 calls registry lookup11AE30;
3CA004 stores the returned mask at3B14B0. The existing source metadata already
proves3E0718=`PHYSICSBODY` and3E0720=`STATICOBJECT`.

The current native namespace was audited from its actual runtime producers:

- CHARACTER=1: gc_animated_candidates50ig_fixture.h and
  ga_animated_candidates_fixture.h.
- PHYSICSBODY=2: canonical main/secondary dynamic masks, native racer contact
  query/target filters and terrain binding at terrain_interactive_drive50du.cpp851.
- STATICOBJECT=4: canonical material masks and the same terrain binding.
- TRIGGER=8: GC gate and GA/MA/SA trigger candidate owners.
- FLOOR=16, WATER=32, TUBE=64: terrain_interactive_drive50du.cpp594 passes
  `{4,16,32,64}` to the real track authoring owner; lines851..852 retain the
  corresponding terrain callback/filter bindings.

No actual live producer in that selected runtime uses128 for a category.
`OriginalNativeCollisionCategories` names all these bindings and assigns the
new unused native bit128 to POD. This is an explicit native namespace choice,
not a guessed guest numeric mask. It preserves all existing bits. Consumers
must check their canonical existing owner masks before using the new binding.
The authentic selected694 category is PHYSICSBODY|POD. Reset movement7A4 is
PHYSICSBODY with its STATICOBJECT category list; terrain348 selects FLOOR.

### Actual694 filter and its constructor self-peer exclusions

Source2121E4 allocates the collision object,2121F8 calls11B190(a1=1), and
212200 stores the resulting real object as694. This constructor calls141300
on its embedded GameObject at+8 using the same a1=1. Thus flags31's low
nibble is3; its upper nibble is opaque preserved input.11B190 establishes:

- B8=FFFFFFFF at11B3A4.
- Identity-list modes BC=C8=1 and category-list modeD4=1 at11B3AC/B4/BC.
- Both identity arrays and the category array/count are initially empty.
- E4=E8=EC=F0=null at11B348/350/358/360. The694 setup does not add a
  target-response callback. Its owner9C is the actual vehicle (21224C),
  physical bodyA0 is the actual340 (212258), and A9 stays constructor0.
- Copied40 and50 are the real source3E02C0=(0,0,0,1), at11B450/45C.
  Static initializer3C81A0..3C8220 proves this sentinel independently.

Source212208 applies11B570 mode5. That setter clears bit3, clears bit1 and
sets bit2; constructor-defined bits0/4/5 are zero. The resulting low six F8
bits are4, with6/7 preserved.212228 stores PHYSICSBODY|POD.212230 calls123EA0
with1, retaining the first-list exclusion mode.212240 loads the actual podboxf
resource with11B920 and main190; this shape/resource path does not set E8.

The later source construction block must not be skipped:

-212984 calls123EB0(694,690).
-212990 calls123EB0(694,7A4).
-21299C calls123EB0(694,348).

These append, in that order, to the first identity array C0/countC4. WithBC=1
they are exclusions.694 does not receive its own identity or364 from this
block. On the other side,690 receives364 on the actual secondary-present
branch, then694,7A4,348 in both paths.364 receives690,694,7A4 in that order.
Both of348's own identity lists remain empty. The underlying filter
predicate checks both directions, so these are genuine self-peer exclusions,
not permission to suppress694 from a camera query or registered traversal.

The source reference list is in pod694-filter-construction.words.txt. The
source's later part-owner exclusions, when those real parts exist, remain
separate;212AE0 appends694 to each actual part's E0 query. Do not manufacture
part identities for missing native objects.

### Actual7A4 construction

Source2123DC calls11B190(a1=1), and2123E4 stores its result as7A4.2123EC
applies11B570 mode3, giving F8 low six bits2 with6/7 preserved. Its generic
defaults are flags31 low nibble3, B8=FFFFFFFF, BC=C8=1, empty identity lists,
E8=null and copied40/50=(0,0,0,1), just as above.212424 writes9C=vehicle;
212430 writes E4=21EE10, its genuine movement-query callback.21243C sets
categoryA4=PHYSICSBODY.212444 constructs one segment via11C710(1,0,0).
212450 sets categoryModeD4=0 and212460 appends STATICOBJECT through124170.

No single-racer constructor call appends to7A4's own identity lists. The
cross-racer additions at1AD74C/1AD7E4 belong to the mode3 partner construction
path, which the admitted native all-AI mode2 configuration does not take.
Other own objects exclude7A4, and the actual filter predicate is bidirectional.

The exact first-list matrix after ordinary secondary-present construction is:

-690: [364,694,7A4,348], calls212918/212960/21296C/212978.
-364: [690,694,7A4], calls212924/212930/21293C.
-694: [690,7A4,348], calls212984/212990/21299C.
-7A4: empty.
-348: empty.

All second identity lists remain empty. The21299C call has a0=694 and
a1=348; it must not be reversed into a fabricated348 exclusion list.

The current central movement input has callbackEnabled=false. Authentic7A4
has E4=21EE10, so any reached original movement-query continuation must retain
that explicit integration boundary until the real callback is wired. This is
independent of the null target-response E8 proven for694 and7A4.

## Vehicle254 is the existing PodCables owner

The separate visibility worker recovered this exact owner and provided raw
words: 214200 allocates128 bytes, 214210 calls173CC0, 214218 writes vehicle254,
and214228 calls173EB0 using the original cable count and9 samples. Constructor
173D1C initializes its field50 to zero.

The only direct caller of setter174360 is225588. It passes vehicle254, the
actual current7F0 membership, and the source vector.174360 stores its a1 into
field50. Thus field50 is a cached visibility membership used by cable lighting,
not an arbitrary success bit.216AC8 clears it after a real7F0 removal. The
canonical field must be retained on the actual existing per-racer Cables owner
and republished by the corresponding real graphics step. The parent owns that
join; this candidate does not create a second cables owner or edit its files.
