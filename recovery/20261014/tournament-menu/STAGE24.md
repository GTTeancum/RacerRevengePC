# Stage 24: selected-list joins and connected Tournament patch scope

Recovery-only follow-up to de9a396. No production files changed. No builds, tests, syntax checks, conversion, gameplay, project execution, or guest execution/emulation. Evidence is raw static SLUS_202.68 data from the authorized disc directory; VA maps to file offset VA-100000+80. Frontend admission stays closed. Shared renderer dispatch and the separately blocked Runtime lifetime task are untouched.

## Correction to stage23: selected means bit1, not bit3

At27EC20 the search loads member+3. Words27EC24=000217BC and27EC28=000217FE are dsll32 v0,v0,30 followed by dsra32 v0,v0,31. They extract bit1 (shift62 then63), not bit3. The 27EC2C branch skips insertion when this bit is zero. The earlier stage23 bit3 interpretation is withdrawn. The distinct common disabled gate27EB70=0003177C extracts bit2. Entity child gate141DA8=0003173C extracts bit3; these must not be conflated.

A second correction: 27E1E8=30020001 is andi v0,zero,1, NOT a read of a prior return register. 27E148=300A0001 also uses zero. Consequently SearchableGameObject constructor27E130 explicitly sets member bit0 and clears bits1/2; untouched bits including bit3 are preserved. The exact final writes are27E218 bit0=1,27E22C bit1=0,27E240 bit2=0. Registry type lookup does not provide those flag values. Stage23's warning about return-derived values is superseded.

Binder1D3C60 calls27E130, installs3D0180 and initializes its geometry/owned pointers. Init1D3D80 calls141C20 (identity return), configures primitive38, attaches primitive with mask8, and registers binder+30 through2809C0. In the captured concrete path2809C0 ->27F590 ->27E3C0/27E570 ->280BF0, SearchList insertion writes the slot, not member flags. 280BF0's portal path is absent for the actually constructed flat UI zone; it cannot be silently omitted for arbitrary zones. Binder material callback1D3F90 updates corners/material and calls141C30; it contains no member+33 store. Thus the constructed binder remains selected-bit1 clear in this bounded path. There is no need to invent a bit3 writer or set bit1 to force its draw.

Bounded static inventory of byte stores at offsets03/33 in code100000..300000 is retained in the evidence. It is a discovery aid, not exhaustive alias analysis or proof that no external writer can exist. Constructor/setup/callback call-chain inspection is the relevant proof. Other classes do set their searchable bit1 (e.g.2758D0..27 toggles it and27593C..48 sets it), but no evidence connects those owner-specific functions to this binder. Do not transplant them.

## Actual selected-list consumers and order

Camera draw114D50 requires actual global36ADA8. At114DF4 it reads camera38; nonnull invokes27FEE0(zone,zone) at114E00. This is distinct from material camera114D00 ->27F650.

27FEE0 increments global443120 recursion depth, increments shared16-bit8BE4 only on outermost entry, stamps zoneA6, updates zoneA0 through280230, then visits actual portals before its own selected search. Portal traversal reads each next link after callbacks. Debug/depth gates remain actual source branches. At28011C..134 it dispatches zone34 virtual48, then280138..15C dispatches zone38 only if nonnull and different from the CURRENT zone34 value. The second pointer and alias comparison are read after the first callback: do not snapshot both up front. On outermost return, the global443108 additional zone's34 virtual48 may run under the actual A4 epoch condition; then depth is decremented.

Actual SearchList virtual48 is27E920 (table3D2D78). It iterates allocated blocks and calls27ECA0 once per block. 27ECA0 ignores the passed block/count parameters: it reads this+18 selected-list count, fetches index0 upwards through27F000, and calls each Entity vtable8 virtual48. It increments index and rereads count after every callback. 27F000 rereads the current list storage on each fetch. There is no extra active/selected flag check and no renderer prelude in this selected-list loop. Appends can be visited during the same loop; shrinking can end it; replacement/reallocation affects later lookups. It is not an immutable copied vector traversal. Source does not protect a null entry before dereferencing its vtable; a native bounded implementation must reject invalid ownership rather than deliberately dereference it.

27F050 clears the prior selected array and resets count before EACH block's material scan27EAF0. 27EEA0 appends entries and can grow storage via27F090. Combined with27E920/27ECA0, a multi-block population has nontrivial behavior: final selected array comes from the final material block, and draw invokes that same array once per allocation block. Do not silently normalize this into one combined selection. A bounded initial implementation may admit one ACTUAL allocated block (capacity0x400), provided it counts all real manager registrations and rejects a second block. It cannot assume only the three pod/binder members exist in the UI manager.

## Why a non-selected binder still draws

Selected model Entity virtual48 is table3D1F90 ->2418E0. The thunk subtracts8 and jumps241280. 241280 rejects model191 bit0 clear by returning1. Otherwise it performs actual model draw/packet work, then241868..870 calls141D70(model+8). The packet paths241798 and241828/241844 converge on this continuation. Native adaptation must preserve those material/palette/packet requirements, not skip them to get to children.

141D70 requires parent aggregate mask24 & 0x8, walks children18 in attachment order, requires child29 bit0 and !bit3, and child own mask20 & 0x8. Each eligible child first receives actual renderer virtual20(0), then its Entity virtual48. The next link14 is read AFTER callback. This admits binder attachment mask100A (0x8 set), unlike material141C30's1000 gate through the widget's mask2 model edge.

Binder virtual48 is1D4280. It calls actual primitive38 virtual78 in corner order binder+50,+40,+70,+60 with vertex indices0,1,2,3. It then calls141D70(binder), reaching its primitive child attached with mask8 and the same actual renderer prelude. The existing numerical projectOriginalUiRiderBinder50KG already documents this native corner order. The primitive's real draw/material ownership remains required.

Therefore the bounded authentic sequence is:
- Search material callback: model selection/materials; independent binder callbacks update each binder geometry/material regardless of selected-list admission.
- Selected-list draw: selected model draw, then binder children in attachment order, then each primitive child.
- A binder whose member bit1 remains clear is NOT independently appended/drawn again. Forcing it selected or flattening binder draws into the search list would create an extra draw path.

## Smallest connected production scope proposed to root

The next unit should be the selected career's DynamicUserRotate pod WITH its optional real binder pairs, owned by the existing Tournament career resource/session boundary. It is not the entire menu or a new host tournament menu, and not a generic SearchList callback utility.

Proposed exact ownership before edits:
1. platform/linux/native/original_ui_tournament_career_resources50kg.h
2. platform/linux/native/original_ui_tournament_career_resources50kg.cpp
3. platform/linux/native/original_ui_tournament_pod_contribution50kg.h (new)
4. platform/linux/native/original_ui_tournament_pod_contribution50kg.cpp (new)

The existing resource owner already pins the genuine Tournament archive, loadTournament pod identity, selected profile mode2 snapshot, source widget mapping, and current-stamp check. Extend that connection rather than accept arbitrary models/selected flags from a caller. The contribution must retain that same resource owner; load genuine model graph/optional endpoint pairs; register model then binders in actual UI search owner; service material search and selected-model draw as TWO distinct phases; emit model packets then attached binder primitives; use genuine canonical Tournament material occurrences and same-owner retirement. Culled selected model emits neither model nor its attached binder draw, even though binder material preparation can have occurred. No generic success lambdas or synthetic receipts.

This is the smallest complete vertical slice to IMPLEMENT ONCE the actual canonical Tournament material issuer and source borrow/retirement owner are supplied by root. The source graph alone is now sufficient to specify traversal order; it is not sufficient to authorize renderer submission or release. Existing OriginalUiRiderPodScene50KG::load requires OriginalUiRiderMaterialLifetime50KG and a Rider-gated footprint. Simply calling it with loadTournament assets is not an admitted implementation. Preserve numerical reuse for clipping, fog, palette and binder geometry; a Tournament material issuer must retain actual Tournament identity. Do not weaken Rider archive checks as an incidental helper edit.

No safe complete render production slice can currently be asserted within the four files alone: shared material issuance and actual renderer/retirement entry points are prerequisites. An isolated CPU-only graph or draw-plan helper would leave exactly the ownership joins requested here unresolved, so it is deliberately NOT the proposed production deliverable. Once root supplies those existing coordinated owner interfaces, implement and wire the four files together in one patch. Build-list integration, frontend connection and all validation remain root-owned; frontend admission remains disabled until the whole page/session is proven.

## Remaining blockers (narrowed)

- Actual Tournament canonical pod/binder material issuance, post-registration parent virtualA4 synchronization, primitive submission and same-owner retirement. Do not reopen the separately blocked Runtime lifetime task.
- Shared renderer lease/service at source boundaries, supplied by root without changes here.
- Actual whole UI manager population and zone/portal/debug state; single-block admission must reflect real registrations, not an invented three-member manager.
- Concrete callback mutation domain: reject unsupported membership mutation before effects when provable; after actual irreversible service failure retain/latch owner for root retirement. Do not claim source supports a snapshot traversal.
- Remaining page text, popup, other widget and full mode2/result lifecycle integration remains outside this pod slice. No profile/result file changes or unlock/manufactured results.

Exact evidence delivery files: this report and stage24-selected-list-words.txt. No test or runtime correctness claim.

