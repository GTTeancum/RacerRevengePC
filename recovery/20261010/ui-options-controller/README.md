# Original Options menu controller reconstruction

## Deliverable and integration boundary

Two **new files only**, staged under `src/game/`:

- `original_ui_options_menu_controller50kg.h`
- `original_ui_options_menu_controller50kg.cpp`

No existing repository files were modified; consequently there are no existing-file preimages. No build, software test, syntax check, parser execution against the implementation, sanitizer, gameplay or guest execution was run. Evidence generation used only byte-to-text static instruction decoding, raw word reads and input SHA-256. This is untested native C++ code for central integration and review.

The class authenticates/pins the existing original Options archive and owns the original parsed menu and movement descriptor. It does **not** pretend to own the Options renderer. It generates ordered native widget callback/navigation proposals for the original lifecycle and movement, and calls the existing confirm/back route leaf. It returns sound/flush requests rather than silently declaring audio playback complete.

Add the `.cpp` beside the existing native original UI sources in the centrally owned build definition. No CMake candidate was created because that file is centrally changing. No changes to the existing Main-only movement class are required.

## Stable contract

1. `bind(shared_ptr<const OriginalUiArchive50KG>, diagnostic)` only accepts authenticated archive ID `options`. It parses original `UI\UIOption\menu.dat` and `Widgets.Dat`, preserving repeated DAT fields. The menu contains 9 controls (including 0), 3 groups (including 0), and uses the 357-widget shared Options population. `widgetsDat()` gives a read-only descriptor source for the future real mixed-widget bank.
2. `initialNavigation()` creates the initial native navigation snapshot. The archive/page owner must outlive it, its proposals, and route processing.
3. `phase()` generates the full ordered widget portion of the selected source callback. It never performs GPU, audio, popup, background, camera, profile or save operations. Preflight every callback against the real widget owner, then apply callbacks and publish proposed navigation/page flags together. Do not commit navigation first. A failure preserves output.
4. `pageInput()` handles only the source page callback. Directions 2/3 request original sounds 3/4 and do not navigate. Confirm/back reuse `dispatchOriginalUiOptions50KG`; unavailable keeps the exact prospective destination and blocker. Never commit rejected profile proposals. `pageAccepted` is the original return1, not route admission.
5. Snapshot Movement_Control widget5's selected flag AFTER the page callback. Call `movementInput()` once with that actual snapshot. It applies the authenticated mapping to the actual controller-attached group and produces ordered callbacks plus native nav. It preserves the source no-op Options manager notification (vtable +84 -> 2A4330). It does not borrow Main's identity-restricted movement bind.
6. Page/widget plans are not native route readiness. Keep screen12/page1 admission closed until the real mixed-widget, background/camera/effect, popup and lifecycle owners are integrated. The readiness context is intentionally caller-owned and closed by default; this module never sets its masks.

## Exact authored menu

Screen12/page1 is `menu.dat`; page4 `options.dat` is GameOptions. Both are present in `ui.dat`, which also maps pages2 and3 to the same config.dat with different controller constructor variants.

| Slot | Control | Label widget | Description widget | Localization key | Actual destination |
| --- | --- | --- | --- | --- | --- |
| 1 | 2 | 6 | 100 | STR_CONTROLLER_CONFIG_PAGE_TITLE_1 | screen12/page2 |
| 2 | 3 | 12 | 106 | STR_CONTROLLER_CONFIG_PAGE_TITLE_2 | screen12/page3; needs second controller |
| 3 | 4 | 7 | 101 | STR_GAME_OPTIONS_PAGE_TITLE | screen12/page4 |
| 4 | 5 | 8 | 102 | STR_HALL_OF_FAME_PAGE_TITLE | screen11/page1; original mode34=5 |
| 5 | 6 | 9 | 103 | STR_MEMORY_CARD_PAGE_TITLE | screen12/page5 |
| 6 | 7 | 10 | 104 | STR_CREDITS_PAGE_TITLE | screen12/page6 |

Group1 lists Controls2..7 and associates Control1/widget5 Movement_Control. That widget wants joystick1 input, uses Lpad_Up=-2 and Lpad_Down=-1, and has default zero mappings elsewhere. Group2 contains Control8/widget97 (STR_OPTIONS_PAGE_TITLE). Group0 contains Control0/widgets1,2,98,99,105 (Continue, Back and original three-part overlay). Widget3 is separately controlled by the page callbacks despite having no menu control membership; a bank limited strictly to control members would miss it.

The authentic styles, localization IDs, positions and art remain in Widgets.Dat; no English substitutes or placeholder rectangles are authored here. Label positions are x320/y164,188,212,236,260,284; label color 242,133,27 and highlight 204,204,204. Description widgets share x320/y30 and original text/material/font properties. Art98/99/105 uses original `overlay1`.

## Lifecycle proof

`options-vtable-raw.txt` is a raw read of vtable VA3D0AE0. File offsets are VA minus100000 plus80.

- +74 -> 1F9550 entry; +78 -> 1F9640 input.
- +90 -> 1D7390 requested-page/reset; +94 -> 2A6810 previous-page/hide.
- +AC -> 1F9D20 opening; +B0 -> 1F9E20 opened; +B4 -> 1F9ED0 closing.
- +48 -> 1F9A80 render/selection-frame routine, **not implemented** by these controller plans.

Entry sets page+42C=0, calls widget SetEnabled(false) for widgets1,2,3 at1F9570/84/98, then group1 and group2 disabled. It additionally registers the camera/background owner, clears popup+3D8 and sets an external active object; these non-widget effects are not claimed complete.

Opening enables Group1, restores one-based original profile+6E4 via2A6BC0 (select slot only, NO controller attach), then deselects descriptions in exact order100,106,101,102,103,104. First direct callback is1F9D70, subsequent calls spaced1C.

Opened sets+42C=3; enables widgets1,2,3; deselects widget3; enables Group2; attaches controller0 to Group1/saved slot via2A6BE0 at1F9EB8. This selects the movement association and restores the description for the selected slot. It must not be collapsed into the opening phase.

Closing sets+42C=0, disables widgets1/2, deselects descriptions in the same order (first1F9F20), then disables Group2. It does not disable Group1 or widget3 in this callback.

Group-enable body2A5000 writes group enabled, then visits member controls BEFORE associated controls. Each invokes control2A5B60 and nonvirtual widget2A7A00. Plans distinguish these from text enable/disable virtual methods. Requested-page/reset and previous-page/hide use the same inherited source helpers already reconstructed for Main.

## Input proof and missing destination behavior

`options-input.txt` records1F9640..1F9A6C. Original page ignores nonzero controller and active popup, but still returns1. Up/down only request sound3/4 at1F9A00/34 then flush at1F9A18/4C. Movement comes from widget5 later in the inherited container dispatch, with selected-state snapshot semantics already documented by `original_ui_movement_widget50kg`.

Activate requests sound1 at1F9950 and flush1F9968; Back requests sound5 at1F99D0 and flush1F99E8. Missing second controller invokes popup2060A0 before remembered-slot/audio work. The existing route leaf retains this as `controllerRequired` without a substitute popup. Native rejected profile changes remain atomic and uncommitted.

The caller's supplied `sourceInputBlocked` is an additional native gate, not a newly discovered Options-page instruction.

## Honest unfinished boundaries

- No complete screen12 manager/widget renderer is delivered. Shared Options Widgets.Dat contains 357 widgets belonging to six pages. The existing 49-widget Main manager must NOT be reused under a fake Main identity.
- Real original Options camera/background, dynamic selection-frame rendering at1F9A80, global entry effects, popup body and sound playback remain external requirements. The source selection-frame routine reads the selected label's actual text geometry/width, then updates two animated edge objects; substituting a rectangle would be inauthentic.
- Original ControllerConfig pages2/3, GameOptions page4, MemoryCard page5, Credits page6, and HOF screen11 are not implemented by this module. A metadata route is not a functioning destination.
- No host audio-option reinterpretation, gameplay mode remapping, save emulation, PS2/VU execution or guest instruction execution exists here.
- Central review/testing is still required before integration. Do not report this as finished game UI.

## Follow-up source review: missing controller and audio

The second-controller-required path **does not return before sound**. At1F972C, failed controller presence branches to1F9754. The popup call at1F9878 returns, and1F9880 unconditionally branches to1F991C (delay slot NOP). The common tail fetches the selected slot at1F9920, stores profile+6E4 at1F9940, calls sound292360 at1F9950 with a1=1/a2=1/a3=0, then flush267BE0 at1F9968. Only after that does1F9970 branch to the1F9A54 epilogue. This ordering assumes the called popup routine returns normally, as the original call site does.

By contrast, popup-already-active at1F9668 and nonzero-controller at1F9670..78 exit before this tail and before any sound. Native sourceInputBlocked is an added admission gate. The wrapper already handles these before dispatching the route leaf. A defensive explicit routeStatus==noChange early return has now also been staged, so a no-change route result cannot request sound/flush even if future route gating changes. Native unavailable preserves sound requests because native destination admission is not an original early-return branch. The existing all-or-nothing rejection policy still prohibits committing the prospective remembered-slot write separately.

This revision is source-reviewed only; no checks or executions of the implementation were run.
