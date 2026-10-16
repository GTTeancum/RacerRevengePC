# Cable-selection state prerequisite

Inactive partial implementation; source review only, entirely untested. No full
224720 callback, dynamic method38, GPU, or RA runtime entry has been added.

## Implemented

The actual persistent `native_racer50HW::Cables` now embeds a noncopyable typed
`OriginalPodCableSelectionState50LA` instead of a standalone membership50 word.
The existing visibility/reset consumers still borrow the sole member50 by the
same `visibility50()` API. The typed owner also retains constructor position40
with the exact173CE8 vf0 value `(0,0,0,1)`. No mutator publishes a speculative
position, arrays, strip flags, or completion token. The owner lives outside bank
swaps, as the existing Cables owner did. This is a scalar projection of the real
PodCables owner, not a claim that173EB0 or224720 completed.

The retained `OriginalRacerEntityCold50LA` now applies the genuine enclosing
20F390 constructor store210430 after successful SearchableGameObject type
construction and before the represented detached/overlay cold prefix. The store
sets only racer33.bit1, preserving other bits. It does not use static model191
or clipping. Scanner27EAF0 must still execute229AE0 before reading the flag.
This corrects the previous recovery note's unresolved producer:210414..210430
is a proven producer, although a full all-alias write audit is not claimed.

## Exact source operation still to join

224720's five shared lazy words are4347B0=`4392AA7F`,4347B8=`3F800000`,
4347C0=`00000000`,4347C8=`40A00000`,4347D0=`40400000` on their first guard write.
Warm words must remain shared Application-lifetime state; they are not per-racer
defaults to reset each frame. Initialization precedes the actual254 null branch.

The distance branch reads actual renderer442F70->scene0 position180/184/188,
Application382B80->race28C->B4->flags58 and race1B0, plus racer190's transformed
origin. Threshold words are4745C100,481C4000,461C4000 on the source branches.
States40==3/4 or the source distance rejection invoke174370 for slots0..3 with
zero and return WITHOUT the final174360. They do not zero endpoint/tangent rows.

For the remaining path,224720 obtains the chariot1B4 axes, uses actual body340
cachedSpeed34 for the state40==0 Y scaling, normalizes the two tangent vectors,
then visits the four attachment pairs in this order:

| slot | first attachment | second attachment | first update PC |
|---|---|---|---|
| 0 | racer1E8 | racer1FC |224AF0|
| 1 | racer1EC | racer1F8 |224D5C|
| 2 | racer1F0 | racer200 |224FC8|
| 3 | racer1F4 | racer204 |225234|

Each pair is skipped only for genuine missing attachments. Array writes use
actual254.count3C guards, in source order. Fields5C/64 receive the second
attachment position/scaled tangent;58/60 receive the first position/tangent.
All vector stores preserve four lanes;106790 sets the tangent result W to1.
Detached branches consume racer48,854,864, original14EEC0 and434660, and transform
through actual190. Do not replace these branches with the connected mesh path.

Tail174370 values for slots0..3 are `{0,1,0,1}` in state1, `{1,0,1,0}` in state2,
and `{1,1,1,1}` otherwise. For each in-range slot174370 first writes6C[slot], then
calls actual strip30[slot]150580 (zero) or1505A0 (nonzero). Those methods set or
clear bit1 of that strip's flags48, not Entity29. A boolean array with no actual
strip owners cannot stand in for the call.

174360 writes membership50, then copies all four lanes of the reached stack40
to position40. This is the live scratch vector left by the ordered endpoint
work, not necessarily the original racer position. The existing visibility
write is correctly documented as membership-only; this patch does not expand
its authority or pretend it completed the position store.

## Concrete ownership blockers

| Source state | Existing native owner | Remaining required join |
|---|---|---|
| racer254 +40/+50 | persistent roster `Cables`, typed scalar state in this patch | actual224720 writer under the same live racer identity |
|254 count38/count3C, scale54, arrays58/5C/60/64/6C | absent from retained Cables | genuine173EB0 construction; count from214120 including original racer258, not inferred two/four mesh count |
|254 strip children30 and flags48 | no retained23BCA0 owner found | concrete strip construction, real material/packet lifetime and detached7F8 child attachment1008;150580/1505A0 must target these owners |
| raceB4 flags58 and race1B0 | actual camera Session's all8Manager.flags58 and private raceFlags1B0_ are already retained | private authentic render-phase borrow, not copied host values |
| renderer scene0 position180 | genuine published SourceCamera viewport/matrices retained | same actual renderer-current-scene and lifetime lease used by source dispatch |
| racer190/1B4 and body340 speed34 | bank arena graph, lifecycle bindings and bodies.main.cachedSpeed34 | canonical bank visit under render transaction; refresh actual graph, not scratch mesh nodes |
| four attachment pairs | Cables retains authenticated PSG endpoint indices | verify each exact source pointer alias in live bank graph; no substituted root/fallback for absent owner |
| racer48,854,864 and434660 | some corresponding physics state is retained; no complete224720 binding established | prove exact field aliases and shared434660 producer before detached paths |
| shared4347B0..D0 guards/words | no224720-specific retained shared owner | Application-lifetime state preserving warm values |
| Application/race scene ancestry | partial cold ancestry retained | actual root dispatch and complete callback forest; no direct public callback or caller-ready flag |

Thus a complete reached224720 implementation is still blocked by actual missing
construction/lifetime owners, principally173EB0 arrays and23BCA0 strips. This
patch intentionally provides no successful callback stub. The existing connected
mesh builder remains unchanged and is not used as the callback.

## Central review checklist

- Verify210430 against the authenticated word evidence; expect successful cold
  racer flags33 low bits to change from1 to3. Do not interpret3 as completed draw.
- Review the moved membership field's stable Cables lifetime and all existing
  callers of visibility50; no new copy, move, or banked instance is allowed.
- Verify173CE8 W=1 and174360's two stores; position40 stays constructor state
  until the complete source writer is joined.
- Keep per-slot174370 strip writes and early-return ordering; retain all AI
  owners and the existing central retireRaFreshVehicle predicate unchanged.
- Root alone integrates and runs Windows validation. No tests/builds/syntax
  checks, converters, gameplay, emulation, central checkout or disc edits ran.

Evidence file SHA256 is recorded per VA range. Original SLUS_202.68 SHA256 is
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Evidence is static word labeling only; unsupported labels remain raw opcodes.
