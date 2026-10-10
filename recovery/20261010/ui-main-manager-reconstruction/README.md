# Shared original Main widget-manager reconstruction

## Result

Staged native source, not yet compiled, linked, loaded, tested, or run.

`files/src/game/original_ui_main_widget_manager50kg.{h,cpp}` introduces one fixed-address native owner for the actual authenticated `mainMenu` / `UI\\UIMain` widget population and all five page membership/navigation descriptors. It does not broaden the old static-text container's admission. It binds the original bank once, shares runtime state across Main/Legal/Opening/Demo/Gallery references, and owns real text, Icon, and Movement_Control state transitions. There are 49 original ID slots, 47 actual entities: Empty4 and Empty10 are null factory returns, not synthetic invisible objects.

The existing text owner receives one separate, source-confirmed entry point, `setControlEnabled(bool)`, translating nonvirtual UIWidget2A7A00. A small independent hunk makes the old static-text page owner use it when expanding UIControl2A5B60. The legacy text `apply(setEnabled)` remains the text enable/disable virtual behavior; its documentation now distinguishes the two source entry points. No text animation, resource, input, schema or all-text admission restriction was relaxed.

## Ownership and admission

- `bindMain` accepts a genuinely owning `shared_ptr<const OriginalUiArchive50KG>` whose authenticated catalog ID is `mainMenu`.
- It reads actual `UI\\UIMain\\ui.dat`, the original `PageFile` entries, and `Widgets.Dat` itself. Each PageFile is paired to its actual authenticated original member identity. Page slot is kept distinct from authored PageNo: Demo is slot4 with original PageNo3.
- All 37 Overlay_Text descriptors resolve through the existing original text reader and English owner; copied strings and descriptors retain fixed addresses. Dynamic and joystick text remains unsupported.
- All nine Icon descriptors use the original Icon reader. Only source selection/visibility callbacks are owned here; no missing texture/material is fabricated.
- Movement3 uses the separately staged native movement descriptor/widget implementation. Original Main control1 references it, but Group1's association count is zero; the stray `AssociationID1=1` is not read as membership. No operation here artificially selects it.
- Empty4/10 retain ID slots tagged `empty`; they are absent from attachment order and have no callback, input, image, or runtime entity.
- Page DATs, including unused camera fields and unsupported popup/movie metadata, remain readable. Parsing five page descriptors grants no route, popup, movie, transition, scene or backend capability. Unsupported pages do not block Main's CPU widget initialization.
- Main-specific PageInfo camera fields are not parsed into camera state. The manager's real ui.dat CameraInfo and the separate camera worker own that path.

## Runtime interfaces

`page`, `pageDat`, `navigation`, and `inputRegistrations` use the original one-based page-index slot. There is no inferred active-page or ready-route bit. `textDescriptor`, `iconDescriptor`, `textWidget`, `movementWidget`, and `widgetState` return immutable data/live snapshots of the one shared population.

`applyCommands` preflights the entire command batch and every original control membership before any mutation, then commits in command/member order including repeats. Navigation operations use private navigation copies until callback preflight succeeds. `applyWidget` models a direct source call without changing an unrelated control's state. Text control enable/disable invokes `setControlEnabled`, Icon performs the same nonvirtual selected/visibility writes, and movement delegates to its source-derived owner.

`dispatchMain` invokes the established Main dispatch leaf and returns the exact proposal; it does not execute a route, save profile data, remap race modes, or admit Rider/Course. `dispatchMainMovement` is called after the real page callback, takes the selected snapshot before leaf delivery, and preserves the exact original Main single registration. Main's real page callback returns1 once invoked, even when its own popup/input/controller conditions suppress navigation; a noChange/unavailable leaf result must not be used as a false pageAccepted flag.

`updateText` is the separate real entity-update point. `projectText` computes source position and line packet preparation at draw time, retaining original text bytes, packed source color, raw source alpha, and line ordering. It does not update implicitly. The caller supplies original font GetWidth/GetHeight metrics. The owner does not invent fonts or bind material policy.

`prepareIcon` is a one-time binding of the exact cold ScreenTexture atlas through a pinned `OriginalUiScreenAssets50KG` and real texture index, plus the source video depth maximum. The caller must establish the original material/pass-to-image resolution; matching dimensions alone is not evidence. The stored geometry retains original rectangles, raw packet color, source packet depth and immutable material-name storage. Unresolved resources can be bound later without replacing the widget bank. `projectIcon` checks live source visibility and manager ancestry and returns `missingResource` for a shown unresolved icon. No default image, blended-alpha shortcut or missing mmlogo material is invented.

The separate material reconstruction now provides the source-backed missing-PSM mmlogo fallback. This manager does not manufacture PSM bytes or conceal that provenance. Actual Icon47/48/49 `AlphaEnabled=T` stays in the authenticated DAT: Icon2AC590 has no lookup for that field, so it is not reinterpreted as a GS blending switch.

## Real Main widget callback portions

`applyMainWidgetPhase` executes the widget/control/navigation portions atomically in source call order. It is intentionally not a claim that the entire named page callback is implemented.

- requestedPage90: `1D7390 -> 2A6660` clears controller attachments, then group0 through the declared last group each reset members followed by associations. Reset is inherited show-only, including stopped text overlays remaining stopped.
- previousPage94: `2A6810` hides every group's members followed by associations. Duplicate references remain repeated calls.
- entry74: `1E5D40` disables Continue/widget1 through2A7A00; `1E5D50/60` disable groups1/2.
- openingAC: `1E66D8` enables group1, `1E66E4` deselects it, then real packed profile byte+11018 bits0x60 determine whether original gallery control slot5 and the corresponding panel widgets34/28 versus36/37 are enabled. This is source profile state, never a supported-route shortcut. `1E6808 -> 2A6BC0` selects the saved one-based slot+6E0 without attaching a controller. Direct deselect calls stop descriptions29..33. Original menu-effect9 work following these operations remains external.
- openedB0: `1E6910` enables Continue, `1E6920` enables group2, `1E6940 -> 2A6BE0` enters group1 at saved slot+6E0 with controller0, using exact navigation commands.
- closingB4: `1E6980` disables Continue, direct deselects stop descriptions29..33, and `1E6A1C` disables group2. The subsequent original effect fade remains external.

AC/B0 require `originalProfileKnown`, real savedSlot6E0 in1..5, and actual profile11018 input. Invalid input changes nothing. The caller must also preserve source non-widget writes such as Main page+42C (entry/closing0, opened1), global application/profile/effect/audio/camera operations, source entity ancestry and the original aperture lifecycle. A trace-only sink cannot satisfy those contracts.

## Attachment and rendering contract

`managerWidgetOrder` publishes ordinary non-null widgets ascending, and OnTop non-null widgets descending. The actual source tree is:

1. Ordinary widget entity children
2. Existing original manager setup children and page children
3. Deferred OnTop entity children

Those segments are not separate render passes. A page's controls are callback references, not ancestors. Do not filter the original child list by current-page control membership, select leading IDs from NumberOfStaticWidgets, render duplicates for duplicate control references, skip real middle children, or replace source depth with draw order. Text owns its lines in source line order; movement owns no drawable; Empty owns no entity. Actual entity mask/visibility/update/draw gates are the enclosing tree owner's responsibility, conveyed to per-widget text/Icon methods at their correct source points.

## Integration

1. Integrate the sibling movement files before this new manager; no camera dependency is required to compile the widget owner itself.
2. Integrate `control-enabled-correction.patch` or the three matching staged text/static-owner files. The patch has original preimages in `preimages/` and is independently reviewable.
3. Add `src/game/original_ui_main_widget_manager50kg.cpp` to the appropriate native target(s). Root owns shared CMake changes; no build files were edited by this worker.
4. Bind this shared owner once. Bind only real decoded resources needed for admitted Main rendering; do not require the other page controllers to be implemented before constructing the bank.
5. Drive the actual Main widget phase at each established transition/aperture hook alongside real remaining side-effect owners. Do not treat this bounded widget phase as a full aperture `available` certification.
6. Supply actual draw resources and consume projection output through the native original material/depth/blend pipeline. The original live scene, page primitive/interleaving, sound/effect lifecycle, ATM frame owners and input/application queue stay separate.

## Static source evidence

Original ELF: `native-assets-prepared-01/work/native/project/disc/SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Retained instruction words map VA through `fileOffset=VA-0x100000+0x80` and are never executed. `evidence/static_mips_byte_decoder.py` is the retained stdlib byte-to-text display helper, not an emulator or a project parser.

- `pod-manager-load-1CD000-1CDAC0.txt`: original factories, Empty null branch1CD70C, input flags, immediate versus deferred attachment and null slot stores.
- `base-icon-vtables.txt`, `icon-2AC4B0-2ACD40.txt`, `icon-callbacks-2ACD20-2ACF00.txt`: actual Icon constructor/vtable/create/select/deselect/update/draw targets; there is no AlphaEnabled read in that create path.
- `widget-base-2A7650-2A7A50.txt` and `control-callbacks-2A5900-2A5CD0.txt`: control expansion2A5BA4 calls2A7A00 directly, which writes selected+38 and show/hide only.
- `group-callbacks-2A4D00-2A5090.txt`, `group-reset-2A57A0-2A5850.txt`, `page-show-hide-2A6660-2A69A0.txt`: group member/association order, reset/show/hide behavior and controller attachment clearing.
- `main-entry-1E5D10-1E5E00.txt`, `main-hooks-1E66C0-1E6B90.txt`, `select-without-attach-2A6BB0-2A6BE0.txt`: Main widget portions implemented here.

## Verification limits

No compiler, build, linker, syntax check, project parser/importer, test executable, guest instruction, gameplay, or graphics submission ran. Only static source reading, stdlib original-data byte inspection, file copying, patch construction and checksums were used. Root owns the next authorized test slot. The proposed future assertions below are not test results:

- Authenticated bank binds once, with37 text/9 Icon/1 movement/2 null IDs and47 ordinary entities for these authored no-OnTop data.
- All five pages share identical descriptor/runtime addresses for repeated widgets, especially logo35.
- Main requested/entry/AC/B0/B4 ordering preserves Continue and description state; Group1 never selects movement3 from the ignored AssociationID1 key.
- Both profile11018 gallery branches preserve exact icon34/28 versus36/37 visibility and control6 eligibility.
- Invalid page/widget/command/profile/resource inputs leave existing state and projection output unchanged.
- UIControl enable changes selected/visibility without recoloring, stopping or restarting text overlays; direct text enable/disable virtual behavior remains unchanged.
- Actual material/resource pairing, font metrics, packet order, scene/effect interleaving and native framebuffer must be checked end to end before claiming an original rendered UI.
