# Stage21: Tournament inherited Entity traversal and concrete owner boundary

Static recovery after85bd5f6. Only this report and inherited-traversal-words.txt are changed. No production code, shared frontend/renderer, profile/result or clock changes. No tests/builds/syntax checks/conversions/project/game/guest execution. All new ELF reads came solely from the authorized disc/SLUS_202.68 and were treated as raw bytes; instruction meaning was interpreted manually. Existing DAT transcriptions remain evidence, never runtime input.

## Key correction: page inheritance is not manager widget traversal

1F26C4 calls141C30 with the actual Tournament page (s5). This is the tail of the page's material-demand update1F24B0, after portrait, outline/medal pairs, gated trophies, marker aggregate and notches. It is not a loop over the authored Control1/Control2 widget IDs, nor a loop over all63 widgets, nor a render call.

141C30 exact ordered behavior:
-141C3C/40 read parent+24 and require0x1000. No children are visited when the aggregate mask lacks that bit.
-141C4C reads the actual linked-list head parent+18. Each candidate must have child+29 bit0 set and bit3 clear (141C58..74), then child+20 bit0x1000 set (141C78..84).
-For an eligible child,141C88..9C obtains the actual global442F70 object, loads its+40 vtable pointer, calls that interface's+20 with a1=0. This operation precedes EVERY eligible child's callback. Its concrete target/side effects are not yet resolved; do not replace it with a no-op or claim an identity-matrix reset without proof.
-141CA0..AC calls the actual child's vtable+40 with that child as this. Return value is ignored.141CB0 reads child+14 AFTER the call, then loops. Precomputing an immutable callback vector or aggregating returned bools would not reproduce this traversal.
-141CC8 returns1 after the walk, including empty/gated walks. That source integer is not evidence of completed native material/GPU work.

Neighbor141CE0 uses mask4 and virtual44;141D70 uses mask8 and virtual48 (including the same preceding interface call). These are distinct phases. Virtual38 is also distinct: User_Rotate uses1D38B0 there. Do not invoke rotation/draw as a replacement for virtual40.

## Actual page children and construction order

Tournament ctor1ED7EC invokes1D5EA0. Its underlying Entity ctor141300 clears head18, sibling14, parent1C and masks20/24.1D5F78 clears camera4BC. Tournament vtable is3D0EF0; virtual40=1F24B0,44=1F26F0,48=1F2700,84=1EDAA0,C4=1D7A90.

Tournament init1EDAB8 invokes inherited1D63B0. Within that routine:
1. HasPopup controls allocation of actual popup438 at1D6670..8C, ctor1FEC80, then initialization1FF020 at1D6698.1D66AC invokes its virtual18=141520, disabling it. Popup construction occurs early, but it is not appended until later.
2.1D6814 calls page virtualC4=1D7A90.1D7AEC allocates0x340 bytes;1D7B00 invokes camera ctor236180;1D7B08 stores the actual pointer at4BC. The source camera consumes PageInfo camera values and actual manager+2A4 ownership, not a caller-made view snapshot alone.
3.1D6DC0 constructs helper2A6FD0;1D6DD0 initializes it through2A7100 with actual page and actual manager270.1D6DD8 stores it at134;1D6DE4 appends it via141960 with maskFFFFFFFF.
4.Control/group arrays are populated as references.1D7148 appends actual popup438 through141960 with maskFFFFFFFF.
5.After inherited init returns,1EDAC0 tests actual camera4BC;1EDAD0 appends it with maskFFFFFFFF.1EDAE0 then calls1B86D0 on the actual application camera owner.

141960 appends to the existing tail, sets sibling/previous/parent linkage, assigns the supplied mask20, and propagates child20|child24 into ancestor24. Thus the proven normal page list is helper134 -> popup438 -> camera4BC, with camera absent only when actual construction failed/missing. This is a construction-order result, not permission to assume complete successful construction or ignore later real attachment changes.

The helper ctor installs vtable3D3D00; virtual40 is141C30. Its2A7100 initializes eight input-registration list heads and stores page34/manager30. Those registration lists are not Entity18 children. No Entity children are attached by that constructor/init slice. Later mutation must be justified by a real source owner, not caller-supplied children.

## Concrete page child callbacks

Popup ctor1FEC80 installs3D0990, whose virtual40 is2017C0. That complete callback is now captured:
-If actual popup3B0 is nonzero, visit ScreenTextures+40 and+A0 in that order, via2DF8A0; for each nonnull resolved map resource floor its own demand10 to10 using the literal source comparison/store.
-Then test actual popup34 bits1,2,4,8 in order and floor the resources for ScreenTextures160,1C0,220,280 respectively when enabled. Null2DF8A0 results are skipped.
-201944 invokes141C30 on the popup after these own operations, then returns.
-The page's parent traversal skips the whole callback while popup Entity29 bit0 is clear. This is independent of Tournament1F24B0's own direct updates, which have no popup gate.
-This recovered callback is not implemented by the career-resource packet owner. Existing Course popup-clone preparation is archive-specific and does not certify Tournament popup state, clones, resources or lifecycle. Actual popup/save state remains unsupported.

Camera236180 installs vtable3D1C10; virtual40=114D00.114D10 reads global36ADA8; when nonzero and actual camera+38 exists,114D28 calls27F650 with both a0 and a1 equal to that+38 object.114D34 then invokes141C30 on the camera. No material owner may silently discard the guarded27F650 operation; its full semantics remain to be recovered/joined. Existing Rider/Course camera preparation computes authentic view math for those archives but does not implement this Tournament Entity callback or its live object ownership.

The call through global442F70+40 virtual20 remains an unresolved concrete interface before each eligible helper/popup/camera child. It is a real missing source boundary, not a generic extension callback slot.

## Actual widget population and separate manager order

Authenticated Tournament Widgets.Dat contains63 IDs:31 Overlay_Text,23 User_Rotate,4 Icon,4 Empty and1 Movement_Control. Empty IDs5,12,19,62 retain null slots. Movement is6; icons27..30; User_Rotate39..61; other nonempty IDs are text. No OnTop key is authored, so default false applies to this immutable population.

Factory and attachment evidence:
-1CD890 parses OnTop default0;1CD8A0 calls factory1CD160.
-Factory string3AD6C8=Overlay_Text ->ctor2A8F00 ->vtable3D4140; +40=141C30,+48=141D70.
-3AD6D8=Icon ->ctor2AC4B0 ->vtable3D3E20; +40=141C30,+48=2ACD20.
-3AD6F0=Movement_Control ->ctor2AAF40 ->vtable3D4000; +40=141C30,+48=141D70.
-3AD718=User_Rotate, DynamicLoad branch ->ctor1D30C0 ->vtable3D0200; +38=1D38B0,+40=141C30,+48=1D3C50.
-3AD738=Empty ->factory1CD70C returns null.
-Nonempty widgets are initially hidden through virtual1C at1CD908 and stored in manager154's actual ID slot. Ordinary objects append directly to manager via141960 at1CD9F8, in source ID order. Null entries remain null at1CDA10.
-OnTop uses a separate head list and is appended via1CDA94 after setup/page children; it is not an alternate rendering pass. No OnTop objects occur in the authentic Tournament DAT.

Consequently the expected ordinary manager segment is1,2,3,4,6,7,8,9,10,11,13..18,20..61,63. The genuine manager's other setup/page children still require actual construction proof and retained ownership; this list alone cannot be used as a complete manager traversal receipt. Page controls reference widgets1,2,3,7,37,30 and27,28,29,38,31..36,62,11,9,10. They are not Entity ancestors. In particular authored62 is Empty: do not copy Rider's blanket rejection of Empty control members or manufacture a widget at62.

Each widget's inherited+40 reaches actual children only. Font/text overlay children, dynamic model/binder children and visibility-list material work must be retained and dispatched under their actual phase/masks. A material-name inventory or presence of a parsed pod PSG does not establish those attachments. The existing Rider backend documents model attachment mask2 and binder mask100A: that distinction must be checked in the Tournament issuer rather than treating every model node as a direct0x1000 child. Complete text-line and pod/binder attachment-to-material-call closure is not claimed in this stage.

## Existing native callbacks versus missing Tournament joins

-Text: OriginalUiTextWidget50KG owns real per-line lifecycle and static update/projection; descriptor/archive pointers must remain pinned. It supports the stated bounded timing domain, not arbitrary interpolation. Missing: Tournament manager population, actual line Entity attachment/masks, canonical material registration and traversal consumption.
-Icon: original_ui_icon_layout50kg plus ScreenAssets/ScreenMaterialReference supply authentic geometry/material primitives. Missing: Tournament typed widget owner and actual Entity material/draw joins. Geometry projection is not virtual40 acceptance.
-Movement: OriginalUiMovementWidget50KG and navigation/controller primitives exist. Missing: real Tournament helper134 input registrations and manager/page delivery. Its inherited material callback cannot be substituted with input dispatch.
-User_Rotate: advanceOriginalUiUserRotate50KG supplies native rotation math; OriginalUiRiderPodAssets50KG::loadTournament already admits the genuine selected career pod. OriginalUiRiderPodScene50KG implements retained graph/material lifetime and rotation, but requires an actually acquired footprint/ledger. Rider dynamic owner/backend are fixed to Rider population and session-private receipt ownership. Missing: Tournament pod attach/detach, real visibility membership, binder ownership, canonical registration and accepted draw/retirement join. Do not spoof a Rider ID/archive or ledger issuer.
-Page/helper: TournamentCareerResources owns bounded genuine page packets; MaterialUpdate prepares ordered direct demands; ScreenMaterialReference applies the bounded marker aggregate floor. Missing: actual page Entity state and inherited helper traversal.
-Popup: source2017C0 now recovered; Course has cold clone support only for Course. Missing Tournament popup owner, real enabled state/clone material refs, exact floor order, save lifecycle and post-effect terminal handling.
-Camera: existing49F view and projection math can be reused. Missing Tournament-specific retained camera source owner and114D00/27F650 interface join.
-Aperture: OriginalUiApertureOwner50KG already recognizes Tournament archive. It is not the entire page's material traversal, popup lifecycle, prompt owner or renderer retirement.

## Proposed exact production ownership and API shape (not implemented)

Tournament-specific files only:
- src/game/original_ui_tournament_widgets50kg.{h,cpp}: final noncopyable manager population owning authenticated page data, fixed63 ID slots, real typed text/icon/movement/pod objects and source link/mask state. Bind takes shared_ptr<const OriginalUiTournamentPageData50KG> and actual localization owner. Actual session alone issues source lifecycle commands. No public addChild, arbitrary callback array, setTraversalComplete or caller-crafted registry receipt.
- src/game/original_ui_tournament_entity_owner50kg.{h,cpp}: concrete page/helper/popup/camera attachment owners. Construction takes the same page/widgets and actual retained application/canonical construction context. Private source-point methods execute materialUpdate1F24B0 and walkChildren141C30; dispatch is by privately constructed concrete child kind with live links and proven masks, not caller registration. Failed post-effect walks latch the session failure; they do not roll back source effects or issue successful receipts. The unresolved source interface must be a real existing owner or a separately recovered concrete owner before this method can be implemented.
- platform/linux/native/original_ui_tournament_draw_contribution50kg.{h,cpp}: actual complete draw composer retained by Tournament session; consumes the above actual owners and existing career resources. Reuses OriginalUiImageBank50KG/MaterialTextureGl50EB. It stages material/group state, emits only original ordered contributions and commits source binding history solely from the actual renderer callback. No public readiness setter.
- platform/linux/native/original_ui_tournament_session50kg.{h,cpp}: actual selected career/profile and retained registry/resource/Entity lifetime aggregate. Private root/frontend binding; owns source-phase sequencing, terminal failure and cleanup obligations. It must not issue full-frame readiness from this report or packet resources.

These are proposed ownership boundaries, not a claim that four files alone solve all source requirements. Shared frontend, OriginalUiSourceFrame/Owner friend declarations and renderer integration remain root-owned. Existing profile/result and Audio clock files remain untouched.

## Material grouping and exact remaining evidence

Traversal order determines material insertion/occurrence order; it is not the final GPU span order. Existing Rider composer explicitly drains24B7B0 ascending canonical groups,29C150 FIFO within the final redirect leaf. A Tournament composer must consume the actual canonical collector/group registrations of its own retained objects, preserve repeated aliases and redirected leaf order, and carry real accepted binding history through renderer retirement. Copying Rider's sorted vectors or a material-name list would fabricate this provenance.

Recovered here: complete141C30 dispatcher, actual normal page-child construction order, concrete helper/popup/camera virtual40 targets, complete popup floor callback, camera guard/call/recursion, exact widget factory virtual40 identities and manager ordinary/OnTop attachment rules. Still unresolved: concrete442F70+40 virtual20 target and effect,27F650 camera leaf, actual line/model/binder attachment/material ownership, the manager's full setup/page-child chain and full draw-tail1D7550 integration. Thus complete native traversal is NOT yet proven. No production edit or successful traversal receipt is supplied.

Next recovery should resolve those concrete leaves/issuers before implementing the proposed typed owner, with full source ordering and failure semantics. Frontend stays closed, real profile/results stay authoritative, and root owns all validation.

Camera leaf follow-up: raw27F650..27F70C shows real mutation, not a safe no-op: it increments the GP-relative16-bit counter8BE0, stores it into object+A4, writes bounds4C/50/54/58 to -1/+1/-1/+1, then enters further application-dependent work. This prefix strengthens the requirement for actual camera visibility ownership; the function remainder is not recovered here. The proposed session file is a coordination boundary for root assignment, not an expansion of this helper's current traversal/composer-only production scope.
