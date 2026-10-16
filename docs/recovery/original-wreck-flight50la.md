# Original one-piece wreck flight, forces, and live observer integration

## Result and boundaries

This packet adds four source-derived native headers for the genuine one-piece chariot owner:

- `original_wreck_forces50la.h`: actual one-target gravity2827D0 and complete167250 force producer, including the real optional WaterHeightProvider50JC query.
- `original_wreck_integration50la.h`: actual167C40 body integration, with the real model19C visibility update at1682C4 between pose publication and direction-cache writes.
- `original_wreck_movement_query50la.h`: actual E8 one-pair staging11CA80, preparation11FF60, registered update120230, and nullable-E4 world query121920(0), using the existing canonical world geometry/filter/query kernels.
- `original_wreck_flight50la.h`: actual166790 orchestration, including the original repeated correction loop. The no-hit route and hits that clear flight38 complete. A retained-flight38 rebound executes its exact impulse/velocity/cross-normalization prefix and reports the remaining query scratch-lane dependency at166AC8.

These are genuine providers, not caller-supplied post-step snapshots or successful-step callbacks. They do not claim a complete wreck frame. Component141ED0/renderer244D60 scheduling, attached/nonflight contact, E0/E4 caller updates, effect construction/parent dispatch, multi-piece groups, and engine collections remain separate dependencies. Existing whole-owner/frame guards must continue to reject a reached unjoined dependency.

All work was source-only: static text inspection and the allowed raw ELF word formatter. No build, compiler, syntax/import check, test, lint, sanitizer, gameplay, asset decoder, or PC operation was run. The central checkout was not edited. This packet adds files only, without changing graph_body49w or widening its observer guard.

## Native owners and call contract

The sole persistent physical authority is OriginalWreckPhysical50LA::body/root, with its existing SecondaryActivationSupplement49V, GraphBodyExtraLanes49W, full vector W lanes, and parameters. GraphBodyWorkspace49W is caller-owned temporary transaction storage. OriginalWreckGravity50LA owns actual count30/capacity34, acceleration38 and force40, with target50 borrowing this sole physical owner.

The cold gravity helper implements282670,282750(1),282790. Its initial acceleration is float bits4200CCCD. force40 comes from the actual application4431C0 vector. The collection's later22C610 must overwrite acceleration38 with the authored Gravity multiplied by32.2 in binary32 order. No guessed gravity vector or water surface is supplied.

The real WaterHeightProvider50JC pointer is the actual pieceB0 binding. A null binding takes the original null route and clears40/3C. A present binding requires actual WaterHeightGlobals50JC and invokes queryWaterHeight50JC on canonical body60. Equality between water and body height is wet. Wet entry writes3C from previous40, then writes40=1 before wet coefficients are consumed. No constant waterline or fabricated dry result is accepted.

OriginalWreckFlightGlobals50LA is an application-global projection, shared by all wreck pieces, not a per-piece member. Its two lazy bytes and scalar words represent actual gp852C/433B50 and gp8530/433B58.1667B8..1667CC initialize bits3CCCCCCD;1667DC..1667EC initialize50. Already initialized values are retained and only read if a hit reaches them.

Integration verifies visibility.boundTo(assets), the actual parentless model288 binding, and the real body's empty141ED0 child route. It does not create an observer receipt. The canonical body/graph prefix is published before visibility.update(); that operation performs the genuine28EA20/280D40 registered model update. Failed later stages retain the completed prefix and must not be retried from the beginning of the frame.

The movement query borrows the actual OriginalWreckQueries50LA world descriptors, world object/state/copies/geometry tables, normal-mode owner and its own E8 registered membership. E8's bodyA0 is actually null and its callbackE4 actually null; this is not a disabled callback substituted for a nonnull source callback. Existing world query admission still rejects a reached target responseE8 it cannot dispatch. PHYSICSBODY and WATER exclusions come from the actual query owner and are not rebuilt here.

## Exact source order

### Forces167250

1.2827D0 sets gravity.force40.Y=(-mass)*acceleration38 and accumulates its XYZ into force180, preserving W.
2.167288..1672B8 normalizes velocity with the original VU zero-length rule;1672C4..1672FC separately computes scalar speed and stores cached34.
3.16731C..167384 accumulates direction*(speed*((-dragEC)*speed)).
4.16738C..1673FC accumulates (-rateDampingF0)*angularB0 into torque190.
5.167400 checks actual B0. If present,2B10F0 queries a copy of body60;167434 tests surfaceY<bodyY.
6.167444..167464 publishes entry/wet flags;167468..1675AC repeats drag/rate damping using F4/F8.
7.1675C8 first multiplies FC*mass, then gravity38*that product.1675EC..167614 adds (+0,lift,+0), retaining the signed-zero effects of explicit X/Z additions.

Each numerical phase uses individually rounded operations. The finite normal-or-zero domain rejects unrepresented EE exceptional behavior; dead water fields and later hit thresholds are not pre-read. Force and torque W words remain unchanged.

### Integration167C40

The reused exact kernels are pointGraph49W, matrixToQuaternion48G, integrateBody48C, quaternionToMatrix48G, setParentlessWorldMatrix49P, direction49U, rotateAxisGraph49W, addParentlessTranslation49P and inverseDirection49U. The cold167920 class fixes the actual14EE40 integrator and1654B0 callback.

Before1682C4, the implementation preserves original previous90, derivative2B8/2D0/308, C0.W=1, D0.W=2, center-offset positioning, optional rotation correction, second point getter, and relative-null translation add. It then publishes to the real graph and invokes its real model19C owner. The two cache getters execute afterwards, followed by exact clearing of all six full load/correction vectors to XYZ+0/W1 and callback238=0. Disabled bodies take the actual skip-to-clear route without consuming dt or force coefficients.

Observer scratch carry is source-grounded, rather than assumed to remain the null-observer value:

-28E428 raw4A20025B writes VF9.W=1.
-28E46C,28E4A0,28E4D4 raw4BE04A68 each ADD.xyzw VF9,VF9,VF0, leaving4.
-280D40/281020 have no VU instructions. Reached230BD0/231150 portal routines use VF4..6;106790 is scalar XYZ/W assignment. The admitted dynamic List selection/insert/remove/update and preallocated pool operations are integer-only. Allocator growth/unsupported methods remain outside the bounded registry admission.
-Consequently a live attached observer leaves4 for28BBB0, producing local80.W=5;28B9B0 then produces worldA0.W=6. Actual null19C preserves the second point getter's1, producing2/3.

The original advanceGraphBody49W observer rejection is untouched.

### Flight166790

The pre-integration model position is a source stack vector initialized with W1.28B1A0(null) overwrites XYZ only. It is not piece.previous80. Forces and integration run once, then DC.contact3C is cleared at166874.

Every loop iteration obtains the current model translation, stages the SAME original start and the current end into the actual one-pair E8 query, updates its real registered membership and performs121920(0).11FE50 is genuinely a no-op for shape3: raw jump table388A2C is11FF4C. No model preparation is omitted or faked. A nonzero actual piece148 reports explicit-list122660 rather than substituting the world query.

If F8bit5 is clear the original function exits. A hit sets DC.contact3C=1 and copies the complete selected normal into pieceA0. Strict abs(dot(normal,velocity))<433B58 clears flight38. Cleared flight38 skips rebound arithmetic. The correction uses scalar MAX.S ordering for dot(selectedPoint-current,normal) versus433B50, multiplies by2 then normal, adds to current translation, performs actual28ADD0(null) XYZ-only local translation replacement, and calls the real observer28EA20 before repeating. The loop has no invented iteration cap or forced miss.

A retained-flight38 hit computes source projection, impulseC0=-(1+restitution110)*projection with W1, updates only velocityXYZ, subtracts the old projection from that updated velocity, computes normal cross tangent, and VU-normalizes it. Its cross result W is+0 (166A7C VSUB.w) and166AAC preserves that W. These source fields are published. The next source28BBB0 at166AC8 requires the final VF9.W from the preceding complete geometry query, which existing query kernels do not expose. The function reports angularQueryScratch at that exact boundary. It does not supply a guessed4, selectedNormal.W, or successful angular receipt. Later friction114/inverseExtent118 and speed34 are not consumed on this guard.

## Remaining smallest joins

1. Expose the genuinely produced VF9.W across the canonical121920/type3 geometry traversal, including later misses after a hit, unsupported-shape misses, compound child order, and nullable callback paths. It cannot be inferred from hitCount or the selected normal alone. Then join the already recovered166AC8 inverse-direction,166AF0..166B14 scale and continue the existing correction loop.
2. Join the concrete166E40/2CA840 caller to its genuine branch, E0/E4 prepared registry updates, acceleration measurements,167640 settling, and actual141ED0 child/renderer dispatch. Parent owns the aggregate and effect coordination.
3. Keep actual optional water/effect/model/query providers and authentic cold resources. Missing nonnull providers must stop their source path; no fake callbacks, empty contact arrays or fabricated fragment descriptions.
4. Extend past genuine one-piece chariot only when collection pieces/group offsets and scheduling are sourced. Engine collections are not covered.

## Evidence

`flight/evidence/format_words.py` is a copied permitted read-only scalar formatter. It does not execute the original instructions. Its output includes166790..168390,28E230..28E5D0,28EA20..28EA80,280BF0..2814A0,230BD0..231300,106790,106E70,27E3C0 list primitives,27F590 dispatch,105720 pool primitives,28ADD0/28B1A0 setters/getters and11FE50/11FF60 shape dispatch with actual388A00/388A20 tables. Raw unsupported VU words are retained verbatim and the relevant lane operations are identified above. The ELF file offset mapping is the same approved address-0x100000+0x80 mapping used by prior packets.
