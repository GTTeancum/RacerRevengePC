# Stage22: concrete renderer prelude, camera visibility and child-material joins

Incremental after035e103. Recovery-only; exact changed files are this report and stage22-camera-interface-words.txt. Original bytes read solely from the authorized disc/SLUS_202.68 (SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30). Raw words were interpreted manually; no source/guest execution or emulation. No builds/tests/syntax checks/conversion/project/game launch. No production patch: several actual lifetime/registration joins remain unproven.

## Pre-child interface resolved: real renderer244D60(0)

Stage21 left141C98's indirect call unresolved. It loads global442F70, then object+40, then table+20. Actual renderer ctor243A80 writes table3D2050 at object+40 (243A94..AA0); init243C80 publishes the SAME object at442F70 (243C9C..CA4). Table3D2070 contains244D60.243BB0 is the destructor, not the constructor: it reinstalls the table and clears442F70. This distinction is recorded explicitly to prevent the destructor slice being mistaken for initial ownership.

Thus every eligible141C30 child first invokes244D60 with a1=0. This is a queue/callback service, not a transform reset.244D6C tests D8 bit0 and returns0 immediately if disabled. With wait=false it skips the wait branch, examines callbackDC, may call it at244E30, preserves it when its result is nonzero, clears it when zero, and consumes queued work according to currentQueue44 and count54/count60. The nonempty path changes queue state and enters further source submission work. Source return0 is not an unimplemented callback or failure by itself.

Existing native implementation to reuse: src/game/original_camera_start_native.h, serviceOriginalRenderer244D60. It accepts the actual OriginalRendererDispatchFields plus the real VehicleWindowGl50DS and diagnostic. For wait=false, disabled-D8 or a genuinely empty admitted queue with no pending callback completes without issuing a GL drain. It rejects a nonnull source callback and queued source work rather than claiming an unrelated host finish discharged it. The constructor proves initial44=2,54=60=0,DC=0,D8bit0=1, but those values are not permission to reset fields before each child.

The inspected UI frontend does not expose this actual application-wide dispatch-field owner to Tournament. Root must join the real retained renderer state, and the service call must occur before EACH eligible child, including recursive levels. Do not copy fields into a Tournament-default struct, pass a success lambda, or add another renderer service abstraction. Nonempty queue/callback paths remain unsupported until their actual owner adaptation exists. The source helper's result must feed a concrete traversal failure boundary, not a manufactured success receipt.

## Complete27F650 body and ordered callees

The entire body27F650..27F928 is captured. s2 is the actual zone/search owner passed from camera+38; s1 is the second argument (camera114D28 supplies that same pointer). It is not the camera object itself and not merely a copied view matrix.

Ordered behavior:
1. Read owner's linked portal list30. Construct two scratch arrays, four elements each through262FD0.
2. Increment the actual GP-relative16-bit8BE0 counter, store A4 on this owner; write bounds4C/50/54/58 = -1,+1,-1,+1.
3. If actual Application382B80->14 exists, perform the guarded diagnostic-index/counter work through3833D0 and application+1940, then133A20 and133900. These are source branches; a production implementation must prove their normal absence or join the real diagnostics owner.
4. Clear global443100. Walk actual portal nodes from30 through each node0. For each,230880(node8, scratch40, current renderer scene0); on success263260(global434940,scratch40,scratch80); on success2806B0(secondArg,neighborZone48). Only when that membership lookup returns false call neighbor27F930(scratch80). Then advance the live list.
5.280700(this) reads the four current bounds and tail-calls280720 on matrix60.280720 reads the actual renderer's active scene0, not an independent camera guess. Existing composePortalZoneView280720 implements the numerical view/projection operation used by the UI visibility owner.
6. Increment real16-bit8BE4 and store ownerA6. If owner34 exists call that object's virtual44. Then call owner38 virtual44 if nonnull AND distinct from34. The identity comparison is literal; two aliases must not dispatch twice.
7. Walk actual list30 again, calling27FC40 on each node4.27FC40 has its own A6 visited gate and recursive continuation; it is not an unconditional no-op.
8. If owner44 and global443108 exist and that other owner's A4 differs from current8BE0, stamp its A4 and invoke its34 object's virtual44. Return1.

The complete top-level body is now recovered, but portal recursion, diagnostic side effects and every concrete virtual44 population are not all admitted. A flat UI specialization can omit those paths only when its actual scene construction proves list30 empty, the additional owner44/443108 path absent, the required diagnostic domain, and actual34/38 ownership. An empty guessed vector cannot establish this.

## Compare actual native camera/visibility consumers

OriginalUiRiderVisibilityOwner50KG (original_ui_rider_visibility50kg.{h,cpp}) retains actual archive, registered model scene pointers, generation handles and selection serials. bind is explicitly Rider-only; pageIdentity is UI/UIRider/rider.dat and widgetIdentity is its Widgets.Dat. prepareFrame requires that same page identity, builds membership and guard matrices, increments its owned viewport/search epochs, then publishes frameReady. It is used by the concrete Rider draw contribution begin BEFORE selected-model packet traversal. selectModel validates current membership, refreshes real model world/bounds, computes viewport/guard clip flags and records the actual current selection. These are useful connected semantics; they are not a general27F650 execution receipt.

Options' OriginalUiControlsModel50KG::preparePacket shows legitimate numerical reuse: it builds a Rider-named numeric camera value while preserving Options identity and never presents it to a Rider owner. It retains the actual Options model/grid/light ownership, refreshes real bounds and calls shared membership/clip functions. Its own scene admission proves one zone/no portals and shared34/38 grid. This is the model for numerical reuse, not permission to substitute an Options grid for Tournament.

Tournament should reuse composeCameraView49F, composePortalZoneView280720, prepareOriginalUiRiderMembershipCamera50KG/ClipCamera numerical helpers, and original model clipping/palette leaves. The existing Rider visibility owner cannot be bound by relabeling Tournament data; its population/archive/scene lifetime contract is concrete. A Tournament-specific owner requires actual manager search construction, exact registrations and consumed handles. Broader generalization of shared Rider APIs belongs to root's coordinated integration, not this recovery helper.

## Exact text attachment-to-material evidence

Overlay_Text factory constructs2A8F00; widget+40 inherits141C30. Text initialization allocates actual TextOverlay children through29C850 (2A9594 in the line branch), stores them in widget60, then appends them with141960(maskFFFFFFFF) in line order at2A9710. Other source initialization branches append at2A97F0 and2A99E4; these are real branch-specific occurrences, not duplicate calls for every text.

29C850 installs vtable3D37F0. TextOverlay+40 is141C30, +38 is29CDA0, +48 is29D3F0. Its complete29CA40 initializer is captured: it acquires font via the actual application font registry, allocates a ScreenText through2E02F0, stores it at30, initializes it through2DFD90 and sets actual color/alpha/depth state. It ends through141C20. That ScreenText is an owned non-Entity resource, not an extra Entity18 child. The initializer contains no Entity attachment call. Thus the inherited material walk is not permission to call text's update or draw leaf, and the ScreenText's material ownership must survive separately until actual draw retirement.

Existing OriginalUiTextWidget50KG owns per-line native lifecycle, supports the bounded static update domain, and projects lines through real retained descriptors. Missing Tournament join: fixed actual line population and hierarchy gates plus canonical font/ScreenMT acquisition and actual lifetime/group events. An arbitrary material-name list cannot mint these registrations. Dynamic text construction/profile mutations and unsupported interpolation remain explicit boundaries.

## Exact model/binder attachment-to-material evidence

Dynamic User_Rotate load1D35F4 calls141960 on the widget with returned model Entity and mask2. Two optional binders append at1D3710 and1D37A8 with mask100A to the actual model object's Entity subobject (model60+8), NOT directly to the widget. Actual unload1D3874 detaches model60+8 only after the preceding visibility/observer operations; ownership cannot be replaced by destroying a CPU graph.

The widget's material+40 is141C30, which filters its child's OWN mask20 for1000. A model attached with mask2 therefore cannot simply be treated as a direct material child because a binder below it has1000. The camera/search selected-model path and actual model dispatcher must account for reaching binder work. This is the key remaining connected traversal proof; do not flatten the binders up into the widget list.

Binder constructor installs3D0180; +40 is1D3F90, +48 is1D4280. Complete1D3F90..1D4270 is captured: missing endpoint80 or84 returns0; otherwise it obtains real endpoint world positions, reads active renderer camera0 matrixC0 column, computes/stores the four geometry corners using authored width88, resolves actual primitive38->30 material token, skips invalid FFFFFFFF handle, otherwise resolves registry382B38 via298E60 and calls29BFF0, then invokes141C30 on the binder. No guest/VU instructions were executed; geometry must use the existing native numerical implementation.

Existing connected native pieces:
- projectOriginalUiRiderBinder50KG implements native1D3F90/1D4280 geometry from actual graph endpoints and active camera column.
- OriginalUiRiderBinderAnimation50KG owns unique per-helper animation and material lifetime; it is not shared merely because cold ATM bytes are shared.
- RiderBackend::attach creates binder candidates in pair order, registers the actual model with its visibility owner, installs retained handles and synchronizes real material events. Detach requires cleanup entitlement, removes visibility, destroys binders in reverse pair order, and leaves graph release to its owner.
- RiderDrawContribution obtains actual pod/binder owners, projects geometry, advances/samples the actual binder animation at the established point, then projects source material packets. This is a real consumer pattern, not a generic callback API.

Those owners depend on Rider-issued population, footprints, private material-consumer and source-submission/retirement entitlement. OriginalUiRiderMaterialFootprint's identity helper gates Rider members. Tournament loadTournament supplies authentic cold pod assets, but that alone cannot acquire a Rider-issued footprint or authorize dynamic attachment. Reuse numerical functions; supply genuine Tournament issuer/registration/retirement ownership before connecting the same lifecycle pattern.

## Production boundary and next proof

No new callback shell, renderer setter, copied queue state, fake readiness or traversal-completion flag is added. The exact prelude target is resolved and has an existing bounded native implementation; the full camera body and its native numerical/visibility counterparts are mapped; concrete text and binder attachment sites are now identified.

Still needed for ONE honest complete traversal/composer:
- Actual UI manager search/zone construction proving Tournament's flat34/38 population and omitted portal/extra/debug branches.
- Its concrete search virtual44 -> actual model callback chain, including how model Entity mask2 leads to binder+40 rather than flattening hierarchy.
- Real Tournament text/model material issuer, canonical grouping/occurrence records and same-owner binding history/retirement entitlement.
- Root-owned application renderer-dispatch lease and final frontend/renderer seal integration.

These are concrete ownership/dispatch gaps, not reasons to recreate native camera math or texture upload. Follow the actual manager/search constructor and model virtual chain next. Frontend stays closed. Root owns all software validation.
