# Main Movement_Control and camera reconstruction

This directory is a source-staging deliverable. None of these files has been integrated into the active repository, compiled, syntax-checked, or run. The original executable was read as data only. No project parser, importer, test, gameplay, or guest instruction was executed.

## Result

Main widget 3 is a menu navigation input leaf. It does not move a camera or draw anything itself. Its authenticated mappings are left = previous and right = next. The actual Main page camera is instead a borrowed reference to the single Main-manager camera, configured from `UI\UIMain\ui.dat` `[CameraInfo]`.

The five camera fields in Main's `main.dat` `[PageInfo]` are preserved as original metadata but unused by Main's actual virtual factory override. That is a Main-specific source fact, not permission to discard unknown fields or to ignore camera configuration on other page classes.

## Files and dependencies

Four translation units, each with a matching header, are under `files/src`:

* `assets/original_ui_movement_control50kg`: reads all 16 mappings, GridSize, manager-owned joystick subscriptions and OnTop; retains unsupported mappings in the descriptor rather than pretending they do nothing.
* `game/original_ui_movement_widget50kg`: bounded actual-Main owner; widget selection/visibility callbacks, exact input gates, previous/next group navigation and the proven-empty Main manager notification.
* `assets/original_ui_main_camera_config50kg`: actual manager CameraInfo reader and Main-only raw PageInfo camera metadata retention.
* `game/original_ui_main_camera50kg`: source-ordered cold numeric preparation of the manager camera, reusing the recovered `camera_view49f` implementation. This is not a live scene registration or update scheduler.

Dependencies are existing `original_ui_pages50kg`, `original_ui_archive50kg` through that header, `original_ui_navigation50kg`, and `camera_view49f`. The asset reader requires the newly corrected `originalUiBoolean50KG` helper. Compile the camera translation unit with the project's existing strict binary32/no-fast-math/no-contraction policy. Root owns any build-system changes and all execution checks.

Descriptor, DAT, archive and page owners must have stable addresses and outlive bound leaves. The Main metadata holder intentionally stores pointers to the original immutable raw entries. No source file stores a guest address as a runtime object pointer.

## Movement lifecycle and dispatch contract

`readOriginalUiMovementControl50KG(dat, 3, descriptor, diagnostic)` is a cold descriptor read. Main admission uses the authenticated archive catalog identity, not a basename heuristic. `OriginalUiMovementWidget50KG::bindMain` supports the source-authored Main domain: GridSize 0 and mapping values 0, -1, -2. Other retained mappings fail admission explicitly. Generic grid algorithms and positive direct-slot mappings are not implemented by this bounded owner.

The base constructor sets selection/input eligibility (`widget+38`) to zero. The manager recursively hides loaded widgets. `bindMain` therefore begins deselected and hidden. The seven common control operations have these effects:

| Operation | Source effect |
| --- | --- |
| show | shown = true; selection unchanged |
| hide | shown = false; selection unchanged |
| select | selected = true; always shown at end |
| deselect | selected = false; always hidden at end |
| clearSelection | selected = false; visibility unchanged |
| setEnabled(value) | selected = value and shown = value; this is UIControl's call to 2A7A00 |
| reset | shown = true; selection unchanged |

`setEnabled` here is the source UIControl operation. It must not be confused with UIWidget virtual+80/+84, which clear/set a separate disabled field at +4C. No current Movement input code reads that field, and this bounded API does not claim to implement those additional calls.

The owner supplies `OriginalUiMovementDelivery50KG`:

1. The input container was reached by native event routing.
2. The active page's input-enabled field +268 permitted processing.
3. The active page input callback returned nonzero.
4. This registered leaf's selected flag was true in the container's snapshot.

The container snapshots every registered leaf's +38 after the page callback and before any leaf callback. It does not re-read visibility, selection or enable state after another leaf modifies it. The mixed owner must preserve registration order and repeated references.

Important: Main callback 1E5E00 **always returns 1** at 1E6284. Its popup/source-input-block/controller-nonzero paths suppress Main's own navigation, but do not reject container leaf delivery. Do not calculate `pageAccepted` from a native `noChange` result or from those page-context flags. Leaf delivery still needs the other container and snapshot gates.

`dispatchMain` also requires the descriptor's subscription for the requested controller. It uses the current page's actual controller-attached group. A missing attached group returns no change before reading the mapping. A zero mapping does nothing. -1 requests next; -2 requests previous. Native navigation retains its existing bounded fail-closed behavior for an impossible all-ineligible group.

After a nonzero mapping the original calls manager virtual+84(mapping,1,0,0). Main's vtable points to the two-instruction empty function 2A4330. The returned notification records this actual call and target; no external transition callback is fabricated. Even if navigation returns no change for an empty group, a reached nonzero mapping still records the source notification.

Dispatch works on a private navigation copy, publishes only after successful navigation, and returns ordered control commands. The composition owner must itself use a private navigation copy until every command and widget effect has passed its own preflight, then publish state and apply commands transactionally. Failed admission leaves navigation, command output and notification unchanged. A successful no-change result clears commands/notification unless the source notification was reached.

### Authored selection is not an invented shortcut

Main Control 1 contains widget 3. Group 1 declares `NumberOfAssociations=0` while retaining a stray `AssociationID1=1`. The count controls the source reader; the stray key is not an association. No other authored Main page references widget 3. Do not select or show it simply to make left/right inputs work. Preserve the original control lifecycle and registered selected snapshot. Main's own up/down page callback already handles navigation; invoking it again for Movement would double-dispatch input.

Movement vtable update/render entries point to inherited child traversals 141ED0/141D70. The authenticated factory creates no children. There is no own geometry, texture, pose, animation, timer, matrix or camera operation. This is the evidence for a leaf that contributes no draw packet, not a substituted no-op implementation of its input behavior.

## Main camera contract

Main manager 1CF2C0 allocates a PSXCamera, constructs it at 236180, and creates it at 2361C0 with width 640, height 448, and exact float bits `3FAAAAAB` (4/3). Its virtual+B0 implementation 1CF4F0 conditionally reads `[CameraInfo]` from `ui.dat`.

If the section is absent, the manager returns without invoking the camera pose setter. The base camera constructor defaults are position (0,50,35), look (0,0,1), up (0,1,0), FOV 77, roll 0, near 1 and far 100000. If the section exists, per-key reader defaults instead are position (0,0,0), look (0,-1,0), up (0,0,1), FOV 35 and roll 0. Setter 114F90 copies position, normalizes look/up, writes roll and writes FOV only when nonzero. No invented zero-vector fallback or epsilon is added.

The authenticated CameraInfo values are position (12.249284,18.657846,115.269455), look (0.636078,0,-0.771625), up (0,1,0), FOV 30 and roll 0. Matching-looking page metadata does not change which reader actually runs.

The camera's source aspect is evaluated in the original binary32 operation order:

* source viewport bounds are 2048 plus/minus half the original width/height;
* fullRatio = height / width;
* viewportRatio = (bottom - top) / (right - left);
* aspect1F8 = viewportRatio / (aspect1DC * fullRatio).

Do not replace these operations with a desktop framebuffer ratio or algebraically collapse the result. `prepareOriginalUiMainCamera50KG` passes the resulting aspect, constructor near/far and actual manager pose through existing recovered `composeCameraView49F`. Output is the core view/projection before PS2 viewport/screen adjustment, the same boundary used by the existing native GL adapter. It must be applied to scene geometry, never authored 2D overlay XY.

Main page vtable 3D11E0+C4 points to 1E5CF0. Its complete body copies manager+2B0 into page+4BC and returns whether it is nonnull. It never uses the supplied parser. Thus `CameraPosition1`, `CameraLook1`, `CameraUp1`, `CameraFOV1`, `CameraRoll1` remain source-unused raw metadata **only for this authenticated Main page and factory**. Generic camera reader 1D7A90 really reads those fields and is not interchangeable with the override.

### Live scene boundary still belongs to composition

Main manager also owns scene/resource object +2B4, constructed via 1B2870 at 1CF3E8. Loader 1CF630 uses stem `mm` under `ui\UIMain\`, textures under `ui\textures\`, geometry under `ui\geometry\`, and animations under `ui\anim\`.

Attach 1CFAF0 calls 1B48A0 to add scene entities to the manager, unregisters any previous camera handle through 1148D0, and calls 114770(camera, scene+4C0[0], 2). That allocates a native-original visibility handle at camera+34, sets camera+38 to the selected scene, and registers through 2809C0. Camera is then appended to the manager entity list through 141960, followed by 1B4CA0 scene attachment. Entry 1E5D84 activates the borrowed camera and sets it as current through the root virtual+44 call at 1E5DBC.

Ordinary camera update 2363B0 requires the global debug-pose override bit 2 clear. Base 114AE0 updates pose/motion history and only builds matrices if camera entity flags29 bit2 is clear; virtual+78 resolves to 236DD0 then 115350. The staged cold preparation intentionally does not claim to implement those entity gates, motion metrics, scene visibility registration or render scheduling. The native scene owner must provide those explicit contracts or report its unsupported boundary.

## Smallest end-to-end composition join

1. Own the authenticated Main archive/DAT descriptors and a fixed-address 49-slot widget bank. Construct 37 Overlay_Text owners, nine Icon owners and this Movement owner. Source Empty widgets 4 and 10 are null slots, not dummy visual widgets.
2. Keep all five authored page descriptors but admit only supported page-controller factories. In the actual Main factory, retain the five raw PageInfo camera fields through `retainOriginalUiMainUnusedCameraMetadata50KG`; do not relax a generic page's unknown-field checks.
3. Own one camera configuration and one numeric preparation from Main `ui.dat`, plus the independently recovered Main scene. Main page 1 borrows this single manager-owned camera. Prepare scene render batches with its core camera output and existing native adapter; keep overlays in their source screen coordinate system.
4. Expand page/group/control commands through the shared bank in exact authored order, including repeated references. Null Empty slots preserve source absence. Preflight every leaf before publishing manager/page/navigation state.
5. Run the existing Main page callback once per accepted input; then capture registered leaf selection snapshots and dispatch Movement only through the exact container contract above. Do not manufacture a selection association for Control 1.
6. Drive source-backed page entry/aperture/opened effects and manager/scene render order. Report unsupported animation, route/backend or scene effects explicitly; the cold camera function and Movement leaf are not evidence those effects ran.

## Evidence

The original ELF SHA-256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. `evidence/manifest.json` records source range byte hashes, vtable bindings and authenticated DAT hashes. All `.txt` instruction listings retain the raw original word next to a best-effort static decoding; unknown words remain explicit.

Useful source anchors:

* `movement-2AAF40-2AB5B0.txt`: constructor, all readers, dispatch/mappings, notification, selection and deselection.
* `widget-base-2A7650-2A7A50.txt`: base state and source common callbacks.
* `container-2A6FD0-2A7650.txt`: input subscriptions, page gate, snapshot and ordered callbacks.
* `main-input-callback-1E5E00-1E6290.txt`: Main page dispatch and unconditional acceptance.
* `main-camera-create-1E5580-1E5D10.txt`: final 1E5CF0 body is the actual Main camera alias override.
* `page-camera-read-create-1D7A00-1D7D10.txt`: different generic camera reader.
* `main-manager-ctor-1CF1E0-1CF350.txt`, `main-manager-camera-1CF350-1CF810.txt`, `menu-camera-create-236180-236450.txt`, `base-camera-construction-1143F0-114770.txt`, `camera-pose-setter-114F90-115380.txt`: camera creation, reader and numeric setup.
* `main-scene-load-1CF630-1CFAF0.txt`, `main-scene-camera-bind-1CFA30-1CFB80.txt`, `main-scene-attach-1B48A0-1B4D50.txt`, `camera-scene-registration-114770-114920.txt`, `main-entry-1E5D10-1E5E00.txt`: scene ownership, binding and activation.

The originally named `page-camera-reader-1D7D10-1D8200.txt` is a retained early exploratory listing outside the camera reader. It is explicitly not evidence for camera-field handling. The correct generic-reader range is 1D7A00..1D7D10.
