# 50IT: native original boost decision and control-provider connection

## Scope and status

`ai_boost_decision50it.h` supplies the concrete native173180 boost decision
that50IS previously required through an unimplemented provider boundary. It
owns the real cooldown109C supplement, consumes genuine live source fields,
updates the explicit shared original RNG, and returns the actual action result.
The existing50IS continuation now records FE8 and verifies the provider uses
the same state, frame, pending record, prepared factor and desired speed.

This stage is source-only, separate, unintegrated and untested. No current HOST
candidate or frame mode guard changes. It is usable native source for the named
boundary, not a completed original AI frame or race validation.

## Original173180 branch order

The native constants and comparison directions below follow the retained original
SLUS executable, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

1. If vehicle.7C heat is above the original 0.9 threshold, clear B9C boost bit0 and
   return zero. Throttle/brake actions remain unchanged.
2. Convert signed vehicle.340->cachedSpeed34 with the original unit-conversion factor. If greater
   than supplied desired speed, clear boost bit0 and return zero.
3. If abs(raw steering B5C) exceeds0.9, return zero WITHOUT clearing boost.
4. If count1038 or count103C is nonzero, clear boost and return zero.
5. AI stateFE8==4 clears boost and returns zero.
6. StateFE8==1 reads the genuine target1008's B9C. If that target is boosting,
   clear this racer's boost and return zero. Missing target ownership rejects;
   it is not treated as a non-boosting target.
7. Recompute the signed cached-speed conversion. If below400, clear boost and
   return zero. Exactly400 remains eligible.
8. If boost bit0 was already set, preserve/set it, write B7C=1/B80=0, return1.
9. If probe348->10C is nonzero, set boost, write B7C=1/B80=0, return1.
10. Read persistent cooldown109C. If positive, subtract this call's prepared
    factor, without clamping it to zero, and return0. This path preserves boost
    flags/actions. Earlier exits do not advance this timer.
11. Otherwise store cooldown=1.0 before the shared308030 draw. Advance/store
    the full 32-bit RNG state using the original wrapping linear-congruential
    update; convert its returned low 31 bits to float, then apply the original
    normalization factor. The exact constants remain in the native implementation.
12. Compute heat7C squared with separate binary32 rounding. Source2A2580 is a
    pure node+64 BoostScale accessor, but173180 never consumes its return.
    No BoostScale probability multiplier has been added.
13. If randomUnit<=heatSquared, return0 preserving the existing action fields.
    Otherwise set boost bit0, write B7C=1/B80=0, and return1.

Raw channels and B9C bits1..7 survive every branch. The zero return is a source
control-flow result, not the provider's success/failure indicator. In50IS, only
a successfully completed zero return invokes the ordinary173770 brake leaf.

## Genuine owner mapping

- `AiBoostDecisionState50IT` holds only missing cooldown109C plus an internal
  initialization marker. `initializeAiBoostDecision50IT` writes6.0 at the
  actual cold constructor16E350 or reset16E878 boundary; never each tick.
- Heat comes from the same racer's actual thermal owner (`BoostState50FK.heat7C`).
- Cached speed and probe10C come directly from the same native frame's main
  body cache and probe record. No planar-velocity estimate is introduced.
- Raw steering/actions/flags are the exact pending after-direction record.
- FE8 is borrowed from the real native AI state owner and must match the state
  recorded when50IS selected the target speed.
-1038 and103C are actual counts. Source17108C..1710D8 indexes lists1028/1024 by
  these counts and increments them. The provider borrows live uint32 owners;
  it does not create zero-filled stand-ins or fabricate neighbor-list updates.
- Target1008 is a resolved native control-owner reference. It is required only
  on the original state1 target branch.50IS's ordinary172C50 dispatcher path
  itself admits states0,2,3, while the numeric173180 component retains state1
  and state4 semantics for a future complete source caller.
- Prepared time and desired speed are exactly the values captured by50IS.
- Shared random state is an explicit external reference to the one actual
  serial C-runtime stream, shared across all racers and other308030 consumers.
  It is never seeded or reset by the new decision/provider.

The provider authenticates the actual pending record by the identity of its
`afterDirection` argument, not only by equal values. It also checks frame
identity, callback phase, unchanged canonical control/cache/damage histories,
FE8, prepared time, and desired speed before committing boost effects.

## Important RNG correction to the earlier owner notes

The current HOST grid does NOT maintain one original cross-racer random stream:
`native_racer_owners50hn.inc` snapshots the full frame per racer, including noise;
`native_racer_grid50hn.inc:77` XOR-seeds that frame RNG per racer. This is explicit
HOST behavior. Its field name `sharedRandomState` means shared within that
snapshot, not proof of original cross-racer ownership.

Therefore50IT requires `actualSharedRandom` separately. Do not bind each banked
racer's frame RNG merely because a pointer is available. A genuine integration
must establish the original shared owner and source consumer order. Moving or
re-seeding all current consumers is outside this stage and remains unperformed.
The earlier50IS statement identifying the existing frame RNG as sufficient is
corrected in this follow-on's replacement documentation.

## Concrete connection to50IS

1. Install the staged revised `ai_speed_controls50is.h`; it adds the observed
   FE8 snapshot and corrects the shared-RNG contract. Other adapter behavior is
   unchanged. It depends on the unchanged50IR header.
2. At real AI cold/reset setup, initialize the decision supplement once. Bind
   all `AiBoostNativeOwner50IT` pointers to genuine stable owners. In particular,
   frame/pending must be the same objects being processed and all source state
   must have been produced by the correct earlier AI stages.
3. Run50IS prepare after original navigation/state/packing/repair/direction.
4. Use `nativeAiBoostProvider50IT(owner)` as the provider for
   `resolveAiSpeedBoost50IS`. No “always decline” adapter is needed or permitted.
5. On completed source decision,50IS commits either the boost actions or the
   ordinary173770 actions through existing native shared control binding.
6. A missing/changed owner or numeric-domain failure rejects.50IS retains its
   once-only boundary semantics: no automatic replay of timer/RNG effects.

The numeric decision stages its timer, RNG and action changes together and
commits only on success. The provider publishes its handled/result receipt only
after that commit. Subsequent binding failures reuse the completed decision,
so retrying numeric binding cannot draw random numbers again.

## Source-compatible thermal continuation

Static original vtable evidence establishes AI table3CE1E0+A8 ->173CB0, whose
body is a no-op return. Shared2176E0 invokes this slot at2177A4/217878 when boost
state transitions require it. The existing50FK implementation already models
the same no-op callback for the player subtype, and its numeric branches retain
vehicle58's original player-only profile override condition.

The new `advanceAiThermal50IT` therefore reuses50FK with a required actual
vehicle58==1. Genuine absent D40 remains required by that backend; present audio
ownership still rejects. Use the same source parameters, heat, previous boost
state and engine histories. Run this continuation at228788 AFTER control
binding. Running it before173180 would change the source heat sample.

This wrapper establishes only this bounded common thermal continuation. It does
not select an AI subtype, mutate frame admission, initialize the rest of a
vehicle, invent profile fields, or execute D40 audio callbacks.

## Remaining real integration points

- Actual AI cold lifecycle, native navigation/state/packing/direction and list
  count/target updates still need source-owned native producers and scheduling.
- Original cross-racer shared RNG ownership/order is not present in the current
  per-racer HOST snapshot policy and must not be silently approximated.
-49W, chariot49V and static-impact49Y still have player-only mode admissions.
  Their source-complete AI branches must be recovered before a full live join.
- Current HOST input/navigation remain unchanged. This stage is not enabled.

Strict binary32/no-fast-math, FE_TONEAREST and gradual host underflow are caller
requirements. Unsupported nonzero-subnormal/NaN/infinity/overflow arithmetic
rejects without state publication; this is not an emulated FPU or an EE
bit-equivalence certification. No build, syntax check, native test, gameplay,
original instruction execution, VU emulation, importer or active edit occurred.

## Integration correction: shared navigation cooldown write

BoostScale's return is unused only inside 173180's probability comparison. Original navigation at 170068..170090 reads the newly entered node's actual BoostScale and resets cooldown109C to zero when it exceeds 1. The same persistent 50IT owner must receive this genuine navigation write; the provider alone does not produce it. No probability multiplier is implied. See the [first runnable slice plan](original-ai-first-runnable-slice.md) for shared RNG ownership, source ordering and remaining admission boundaries.
