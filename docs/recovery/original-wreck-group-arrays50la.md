# Original nonzero wreck-group backing and tuning

Source-only incremental implementation based on `0b31d18`. No build, test,
syntax check, sanitizer, gameplay, project decoder, or guest execution was run.

## Implemented boundary

`OriginalWreckGroupArrays50LA` owns six stable native arrays for one genuine
shared collection group. `OriginalWreckRacerStorage50LA::adoptCompleteInventory` reserves
these using the retained inventory's actual piece count minus one. This cold
step constructs no source Entity and changes no source participation flag.
The actual driver already includes the racer-storage header, so the new header
is reachable by the next parent-owned full build.

The separate explicit field phases are:

- `initializeFields2C9E40`: represented fresh base fields, after a genuine group
  component exists, including the actual supplied vector4441F0, identity saved
  inertia, and raw threshold3CA3D70A. It does not claim Entity construction,
  derived effect construction, or initialization of source-unwritten38.
- `bindPrimary2CA020`: bind the actual primary piece and its canonical D0,
  initialize44/A0/B0, apply primary wake30=1 and component mask20=0, and retain
  original mass26C, inertia100 and center1E0. Body constructor completion and
  matching component session are required. Source141C20 is a no-op return.
- `registerMember2CA190`: skip the primary; update only BreakRate/BreakScale
  for an existing member; otherwise insert in the first null44 slot, preserve
  exact authored XYZ subtraction order, set offset W=1, and increment30.
  Full capacity reports the source no-write case distinctly. Numeric rejection
  occurs before insertion. Exceptional/subnormal arithmetic is not generalized.

The cold collection exposes only const group backing. These field methods are
available to a future genuine runtime constructor, but are not invoked by the
cold owner or ordinary racing. Borrowed piece/component/body lifetimes must
outlive the group. Native array destruction frees backing only; it does not
produce a source2C9F30/1413E0 retirement receipt.

## Source proof

Authenticated SLUS_202.68, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Addresses are original source addresses, inspected as data.

- Base2C9E40: zero30/34/3C/40/44/48/4C/A0/A4/A8/B0; copy4441F0 to50;
  identity60; AC=3CA3D70A. Derived2CBFC0 additionally owns effectC0/C4.
- Allocation2CA050..2CA0C8:44, A0,48,A4,A8,B0. Vector48 uses a source array
  header; native ownership does not reproduce its allocator layout.
- Explicit initialization2CA0D8..2CA100 touches only44,A0,B0. Vector element
  constructor1048E0 stores vf0=(0,0,0,1). The native bind initializes every48
  element accordingly. A4/A8 await tuning; access is withheld until its44
  member is populated.
- Primary writes2CA128/130; saved primary parameters through2CA174.
- Existing tuning2CA1AC..2CA1DC; new insertion/relative vector/count
  2CA21C..2CA2B0. Original names BreakRate and BreakScale are at3B3120/3B3130;
  collection tuning supplies them through22C454..22C4D4 and22C57C..22C58C.
- Array retirement2C9F30 frees all six arrays;48 backing includes a source
  header adjustment2C9F6C..2C9F84. Source then retires its Entity separately.

Important:30 is registered member count, not current attached count. Detach
never clears44 or decrements30.38 is the separate attached-detector count,
reset2CA9DC/increment2CAA28..2CAA30. Existing one-piece field naming is retained
for API compatibility, with its interpretation clarified in its declaration.
B0 is a break-event latch, initialized2CA0F4..100, cleared2CAE08..10 and set
2CAEC0..ECC during eligible updates; consumed/cleared2CC7F8..2CC8E4. It must not
be reset merely by attach or detach.

## Canonical exclusion storage

`OriginalWreckQueryWorld50LA::groupPieceCapacity` reserves actual collection
capacity cold (default1 preserves existing one-piece behavior). E0 reserves
N+1 words: own E4, own E8, and every other piece E4. E4 retains two words.
Logical C4 count is separate from backing capacity.

`addOriginalWreckExclusion123EB0` scans all logical entries for duplicates,
then the first zero hole, then appends and increments count. A native capacity
failure is explicit before append. Source anchors123EC0..123F7C.
`removeOriginalWreckExclusion123FA0` clears the first match only, preserving
holes and logical length (123FA0..123FEC). The query owner's group helpers use
its actual canonical E0 list and the target's actual E4 identity, requiring
constructed/coherent queries in the same world and canonical object namespace.
They never change E4 lists or E0.A0, and are not an attach/detach receipt.

## Exact next integration prerequisites

1. Construct all actual collection pieces and the shared group in source order,
   with real Entity references and canonical body/query owners. The existing
   query constructor still takes a one-piece owner; allocating several such
   owners is not a valid full-group substitution.
2. Execute the actual per-piece22C310 tuning and final root saved-parameter
   refresh22C5B0..22C5F0 before activation. This patch implements only2CA190's
   array portion, not the complete collection tuning routine.
3. Complete2CC060's actual nonnull resource/particle-parent path: sparks
   15F100/15F1E0, stop84, append402; explosion15E800/15E8F0, stop84, append402.
   Its null-input early return is genuine, but cannot substitute for real
   nonnull authored inputs. Effect ownership/retirement remains required.
4. Preserve2CA480 exactly: primary mask0; ascending slots set A0=1 even for
   null44; nonnull piece mask0/wake0, body90(false)/AC reset, DC.30 rebind to
   primary D0 via295C80. Child E0.A0 remains its own body. Current response
   code rejects a different contact body; its ownership split must be joined.
5. Then add E0 exclusions in source order: primary->childE4, child->primaryE4,
   then for every nonnull j INCLUDING self, j->iE4 and i->jE4
   (2CA544..2CA5E0). Helpers exist; the group sequence is not yet implemented.
6. Detach2CA2F0 rebinds DC.30 to own D0, ORs mask2, wake1, removes the same
   pairs including self, clears only A0, and recomputes. Removing own E4 is
   real; own E8 survives. Do not preserve self exclusions specially.
7. Implement2CA620 from saved root mass/inertia/center: include44&&A0 members,
   add48 XYZ; add mass; add child inertia diagonal plus
   offsetAxis*(childMass*offsetAxis) separately, preserving source order.
   Divide center XYZ by registered30+1 even after detach, W=1, then primary
   mass78 and inertia7C setters. This is the original diagonal formula, not
   a conventional perpendicular-axis replacement (2CA62C..2CA82C).
8. Full group query/effect/reference retirement and external-borrow release
   remain mandatory before destructive death callbacks or KO attribution.

No active driver call, death activation, KO write, fake callback completion,
zero-capacity guard weakening, or full-group release claim is introduced.

Follow-up: `original-wreck-collection-tuning50la.md` records the subsequent
ordered initial22C310 tuning/root-refresh join and isolated295C80 binding leaf.
Its shared-response and activation/retirement gates remain explicit.
