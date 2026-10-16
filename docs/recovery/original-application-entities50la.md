# Actual native Application Entity startup and root lifetime

The process `original_main_entry50KG::Application` now constructs one retained
`OriginalApplicationEntities50LA` before its first UI or direct native race.
The caller already lives outside the serial race loop. The same owner survives
Course entry, whole-race restart, return to Main and Video close/reopen.

This is the genuine represented13FDC0..13FE0C prefix: construct base Entity26C,
construct distinct base Entity10, then append10 to26C with FFFFFFFF. Both are
real noncopyable component records. The existing141300 helper now also performs
its actual1048F0 base reference00=1 store before the Entity stores. Constructor
increments reach one Application-owned423F60 word;423F58 and423F60 start from
the source BSS zero state and never reset per race. These are contributions
from actual represented constructors, not invented retail object totals.

The owner also retains the actual functional383408 type-name registry. Its
2A31E0 operation searches128-byte case-sensitive names, appends only on a miss,
and returns the actual insertion index. No names are pre-registered and no
retail boot type numbers or collision-category bits are substituted. The
SearchableGameObject accessor carries one shared lazy cache. Its native fresh
cache uses the source FF invalid sentinel; the original file-backed gp801C
initial byte is not claimed verified by the available evidence. A first miss
performs the original lazy assertion initialization; a native allocation
failure latches the already-incremented source count and stops future use.

Application pins this owner with a shared native lifetime. Final cleanup runs
the proven two-base-root142590/104970/1413E0 route only when the actual topology
and source references still match that route: release child10, repair26C.head18,
decrement child reference/count, then release26C. It neither resets masks nor
pretends to destroy a reached unknown child. Additional children or extra
source references keep cleanup blocked; final destruction fails closed rather
than leaving dangling ancestry. Future owners must extend the real teardown
before publishing such children.

## Activation scope

The real Application constructor/destructor is connected. The type registry
is now a shared callable provider, not yet reached by an integrated racer
constructor. Source app10's later TextureManager, LightManager, UI, race and
other child attachments are not asserted. No race/manager ancestor is added,
no141ED0 root scheduler is enabled, and no state4/5, deathDebris, one-piece
collection, frame or renderer activation gate changes. The two-node initial
root domain is explicitly not a complete later Application forest.

The next racer prerequisite is the corrected27E130 /210770 component prefix
with real AIDamage ownership and cleanup. The older pending header has a
separate defect: it typed race8C as a scene filename. Actual1A9284..1A9300 stores
integer race modes there;199980 copies that integer to overlay40 and199E78
compares it with1. It must be corrected before caller integration. Existing
rig/model/physical component constructors, full ordered370/374 collections,
and actual outer ancestry remain independent missing source obligations.

## Review and evidence

Source evidence: original-text-all.txt100008..100030,1048F0..1049B4,
13FDC0..13FE0C,1411AC..1411C8,141300..141450,142590..14266C,
27E130..27E250,2A31E0..2A3320 and3097C8..30997C; original-tail-all.txt
383408,3CB140..3CB168 and string3BBE60. Original ELF SHA256
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

Only manual source/evidence reading and source-byte packaging were performed.
No build, test, compiler/syntax/import check, lint, scanner, sanitizer,
asset-decoder, gameplay, emulator, guest instruction or PC operation ran.
