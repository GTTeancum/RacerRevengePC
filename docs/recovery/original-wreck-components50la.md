# Native one-piece wreck component and physical preparation, 50LA

## Delivered code

Three header-only files provide concrete prerequisites for the real one-piece chariot owner:

- `original_wreck_components50la.h`: embedded native component subobjects, genuine intrusive ownership links, source constructor field initialization/global object-counter update,141960 append/mask propagation,141520/141560 recursive notification semantics, piece activation2CBFB0→166780, and exact141ED0 child eligibility.
- `original_wreck_physical50la.h`: the sole canonical D0 body, separate DC point/generated storage, body/contact/piece constructor fields, actual graph-model binding, empty-linked-body1653C0 enable/disable, and complete167B80 body320-nonzero/empty-child cache refresh using existing numeric kernels.
- `original_wreck_tuning50la.h`: real one-piece aggregate primary binding,166400/22C310 authored mass/inertia/contact tuning, and2CA620 zero-attached recomputation.

No full activation, live query, visibility, effects, or whole-frame completion receipt is manufactured. No generic successful callback is accepted. The parent is composing these with actual cold assets and three independent registered query owners.

## Ownership and source order

Each component record is embedded in its actual owner and is noncopyable. The domain is only a borrowed address registry with session/key identity. It owns no second forest. Original model/collision entity subobjects live at owner+8; their source-relative component fields still have the usual10/14/18/1C/20/24/28/29 meanings.

Source165C30/165E90 appends these piece children in this exact order:

1. model14C entity, mask0 (`165CDC..165CF4`)
2. D0 body, maskFFFFFFFF (`165F34..165F40`)
3. D4 gravity, mask0 (`165F8C..165F98`)
4. E0 collision entity, mask0 (`166084..16609C`)
5. DC spring/contact, mask0 (`166160..166168`)
6. D8 detector, mask0 (`1661C8..1661D4`)
7. E4 collision entity, mask0 (`1662C8..1662E0`)
8. E8 collision entity, mask0 (`166310..166328`)

22BFD0 appends piece then group to the collection, both mask2.2CA020 later writes primary piece.mask20=0 and wake30=1. Its body child remains present with maskFFFFFFFF.22CA60 deactivates the group and ORs piece.mask20 with2, giving the released piece its independent update slot.

141960 appends rather than prepends. It stores child.mask20 and OR-propagates child.mask20|child.descendants24 into every actual ancestor. The implemented cold branch admits a single fresh child, as these constructors call it; prelinked-chain splicing is a guarded unsupported topology. Actual global423F58 nonzero returns the distinct attachment-locked status without writes. Actual global423F60 is incremented by represented141300 initialization. Neither is replaced by a private counter or assumed unlocked flag.

## Activation and notification

141560 changes only its receiver29 low two bits, then invokes slot2C. Every admitted concrete vtable's slot2C is1414C0, a recursive notification that skips children with29 bit3 set. It does not set child29, consult child.mask20, or enable a physics body. The corresponding deactivation base141520/141460 clears only receiver29. Unknown reached notification methods remain guarded; a bit3-skipped unknown child need not dispatch.

The helpers validate the bounded native tree before the isolated activity write. Because the proven notification functions have no other state effect, prevalidating then publishing the scalar activity write is observationally equivalent under the declared exclusive access. Piece2CBFB0 tail-jumps166780: write piece12C positive zero, then141560. Body enable/reset, model visibility and query registration are separate earlier/later source operations.

Calling the base deactivation helper is not a substitute for overriding collection22CE10, piece2CBEB0, or group2CC9C0. Their reached effect/removal operations must execute first.

## Canonical physical construction

`OriginalWreckPhysical50LA::body` is the existing `SecondaryBodyState49M`, including its ONE `RootBodyState48H`, actual caches, enabled flag and W lanes. The49M vehicle-only time864 and phase are not wreck source fields and are never read. The49V supplement and49W extra lanes retain only otherwise-unrepresented source words. The group borrows this body; no group duplicate exists.

164ED0 initializes mass26C/inverse270 tozero and inertia100/inverse140 to identity. The current quaternion1F0 comes from382B60, whose four original words arezero. It must not be replaced with identity. Source3C9190 initializes433AD0 to(0,0,0,1);3C9210 similarly initializes433B10. The constructor consumes actual application-global projections, with a source-backed initialization helper.167920 calls existing resetSecondaryActivation49V, establishing current and previous derivative buffers with the exact independent433B80/382B60 copies. Source axis enables2A0..2B4 areone, body320 initiallyzero, linkedCount268/awake208 initiallyzero.

165E90 binds actual model288 via1652E0, sets320=1, then disables204. Native binding copies only a projection of the actual graph cache; it does not force refresh, load a model, or register/activate anything. The graph remains sole transform authority. Fields not consumed by these recovered owners are not claimed initialized by a whole-object constructor receipt.

295A40/165E90 contact fields are stored in existing SpringRecordState48N/SpringGeneratedParameters48K/SpringResponseParameters48L. DC30 binds the same canonical body and DC34 becomes1.295C20(50) reserves two50×0x60 record arrays: POINT at80 and GENERATED at8C. pointCount7C and generatedCount84 beginzero. Unused payload slots are not source-valid simply because C++ storage has an initial value. The real append/detection producers establish valid records. Actual4435A0 supplies the constructor custom-center/contact-velocity vector; it is not guessed from a no-contact frame.

The collection layer must subsequently perform its22BFD0 writes: DC.C4=raw3E4CCCCD(.2), piece130=raw40000000(2), body27C=raw49034ACF. These writes are deliberately outside the165E90 constructor helper.

## Exact authored tuning

The helper consumes `OriginalWreckAssets50LA::selected().tuning`, its actual sole `pieces[0]`, and the same model's source28D670 aggregate half extents. This is a cold pre-activation route: changing/animating the graph invalidates that cached geometry boundary.28D670 and26F4B0 use the same child bounds union/half-extent construction in this cold route.

166400 doubles each half extent, computes the full-extent vector length, and updates inverseExtent118 only if positive. Mass is Weight/floatbits4200CCCD, through165300: a nonpositive requested mass preserves the previous mass/inverse. Inertia diagonals are independently `Inertia*(mass/12)*(Y²+Z²)`, correspondingXZ, andXY, preserving source scalar operation order. All16 inertia100 words are stored;14E910 cofactor inversion reproduces signed-zero lanes and an all-zero exact-singular inverse. The inverse v15 iszero, notone. The helper reuses the existing source-reviewed cofactor algorithm from authored_geometry49O because no public generic inverse entry exists.

295C90 computes spring stiffness `abs(32.2*(Spring*mass))`, then damping `(2*Damping)*sqrt(mass*stiffness)` only for positive mass and stiffness. The impulse pair follows the same source order.22C4D8 supplieszero to166400's stack restitution argument, so DC.B8/BC/C0 arezero. Authored Restitution goes to piece110 afterward. Friction changes only material0 via295DD0. DC.B4 becomeszero. Water drag/rate/buoyancy become1,200,.5 from the caller's explicit arguments. Drag/rate are also copied into base100/104.

2CA190 sees the primary40 equality and returns without consuming BreakRate/BreakScale.22C5B0 saves the newly tuned root mass and full inertia. For a genuine one-piece group,2CA620 has capacity34=attachedCount30=0: restore saved center50, multiply itsXYZ by1/(0+1), setW1, rerun165300 and165340 using saved mass/inertia. This preserves the root parameters while retaining the source inverse recomputation and center operations. Bounds center is not body1E0.

## Exact body enable and child frame

1653C0(false) always stores enabled204=false and returns.1653C0(true) stores enabled204=true, then returns immediately if actual208 is nonzero; otherwise it traverses actual268/28C linked bodies and sets208=1. The delivered enable helper closes only the actual zero-count branch or already-awake branch. A reached nonzero linked list is guarded.

Body slot38 is167B80, distinct from integration slot3C167C40. With source320 nonzero it skips integration, refreshes quaternion angles through14FC00 and speed through the exact RSQRT/reciprocal kernel refreshCachedSpeed48E, then141ED0. This happens even when body204 is disabled. Native cache refresh closes the actual empty child route only; body320 zero requires the real integration path and nonempty eligible children require real dispatch. Source14FC20 vector default establishes cacheE0 fourth lane1.

## Remaining live scheduling joins

141ED0 first checks parent.descendants24 bit2, then traverses child18/next14 in order. Eligibility is child29 bit0 set, bits2/3 clear, and child.mask20 bit2 set. Immediately before EVERY reached child call it invokes actual renderer442F70 slot20(0), then concrete child slot38, and reads next14 afterward. No prelude callback or caller-supplied success receipt is introduced. The existing renderer service244D60 can execute its genuine empty-queue branch, but nonempty renderer queues/callbacks remain real dependencies.

For this one-piece route:
- Attached collection: piece.mask20=0 skips the piece; group slot38=2CC1B0 calls2CA840 against the SAME rootD0, then its real effects.
- Released collection: group29 inactive skips it; piece slot38=2CB8F0 calls166E40, then its real effects. Source166E40's terminal141ED0 reaches the D0 child167B80 cache refresh; other eight-list children have mask0 unless genuinely modified.
- The flight worker is166790, not167790. It applies167250 forces, calls D0 integration3C, clears DC.contact3C, stages an actual E8 segment from old/current body-model world positions and invokes121920 or122660 from actual piece148. A real miss completes the loop. A hit updates contact/normal, may clear38, performs reflection/restitution/correction and queries again. There is no source arbitrary iteration limit or permitted invented miss.
- The nonflight route calls D8 once at full elapsed factor, then source substeps of at most floatbits3C888889:167250 forces, DC2964C0 response, retain componentwise largest-absolute1C0 impulse, D0167C40 integration, maxima of acceleration norms. Movement correction166C60 and actual E0/E4 query updates follow, then167640 settling and141ED0.
- Force167250 consumes the actual D4 ordered body-target list and mutable gravity40, drag/rate fields, and real water ownerB0 (or its genuinely null branch). A no-water world cannot be inferred from an absent provider.
- MaxNumberOfDamageEmitters zero only suppresses smoke/fire arrays. Dust168, splash16C and group sparksC0/explosionC4 depend on real context pointers. Their absence must be the original reached null-context branch.

These are outstanding concrete joins, not claims that registration or a numerical leaf alone completes a wreck frame. Root whole-frame guards must remain closed for any reached unjoined owner.

## Evidence and restrictions

All files in `evidence/` were generated from the supplied original SLUS_202.68 by a private copy of the explicitly permitted read-only rawword formatter, with only output path and selected spans changed. Important spans include141300..141A70,141ED0..141F90, constructors/vtables,164ED0..1652E0,167920..167C40,166400..1666C0,295C90..295E40,14E910,28D670, and2CA620..2CA840.

Source text/ELF words only. No compiler/build, syntax/import check, test, lint, sanitizer, asset decoder invocation, game run, or PC operation was performed. SHA256SUMS identifies packet artifacts; it is not an execution result. The central repository and earlier frozen launch/settling packets are unchanged.
