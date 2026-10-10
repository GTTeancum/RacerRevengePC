# Original Overlay_Text runtime owner 50KG

## Scope and integration

Two new repository-relative files are staged in `src/game/`:

- `original_ui_text_widget50kg.h`
- `original_ui_text_widget50kg.cpp`

They depend on the frozen `rr-original-ui-pages50kg` navigation, Overlay_Text descriptor and text-position/packet-color leaves. No renderer, main loop, build file, existing stage or active checkout is changed. The owner has not been compiled or executed. No frontend-completion claim follows from these leaves.

Create a stable `OriginalUiTextWidget50KG` for each actual descriptor and bind once. Both copying and moving are deleted; its fixed array keeps line state addresses stable. The descriptor, its strings and authenticated archive identity owner are borrowed and must remain immutable and alive. Binding validates line/count/alpha bounds before publishing any state; failure leaves the unbound owner unchanged. There is no callback allocation or global mutable state.

Expand navigation commands through the actual control's widget memberships in original order, preserving duplicates, and pass each operation to its matching widget owner. Run `updateStatic` only at the ordinary source update point. Pass page/ancestor visibility explicitly. At the source draw point, `projectLine` prepares source packet color and updates the last-rendered ScreenText state. It does not run an implicit update or assert a successful GL draw. Position/font/material assembly remains in the sibling leaves and the parent compositor.

## Source-defined callback behavior

The source cuts are retained in `../rr-original-ui-pages50kg/research/`.

| Callback | Source | Behavior preserved |
|---|---|---|
| Select | `overlay-text-states-2a9e20.txt`, 2AA2A0 | Select bit, visibility flags, all highlight-color setters, then all 29DA40 setups using raw authored alpha |
| Deselect | same, 2AA3E0 | Clear selected bit, deselect visibility, restore base color if highlight exists; otherwise stop each overlay |
| Enable | same, 2AA4F0 | Clear disabled bit, replay current selected/deselected callback |
| Disable | `overlay-text-disable-2aa540.txt`, 2AA540 | Set disabled bit; set authored disabled color if present, otherwise stop overlays |
| ClearSelection | same, 2AA600 | Clear selected bit and optionally restore base color; no visibility or stop |
| Show/Hide | `widget-visibility-141520.txt`, 141630/1415A0 | Hierarchy visibility, distinct from overlay enabled state |
| Reset | `widget-base-2a7760.txt`, 2A79E0 | Inherited show-only callback |

Contradictory enable/disable visibility flags cancel. Selection is not guarded by the disabled bit. A disabled widget can therefore be selected and have its lines restarted while the disabled flag remains set. Likewise showing a stopped overlay does not restart it. Repeated commands are not deduplicated.

## Line gates and alpha order

`overlay-text-setters-29da00.txt` proves:

- 29DA10 color, 29DA40 setup and 29DBC0 alpha each return before writes when +CC is nonzero.
- 29DB10 stop is unconditional: +70=0, +88=0, +84=-1, +CC=0. It does not erase color or alpha state.
- 29DA40 negative f13 sets indefinite +88 and does not overwrite +84. The admitted zero-period blink remains on; f16=-1 takes the no-second-phase path. Setup writes current/target alpha, scale and rotation and their dirty flags. It leaves existing alpha interpolation setter fields alone.
- 29DBC0 clamps its target to 0..255 and captures the low byte of last-rendered ScreenText alpha. It does not immediately write current float alpha.

`overlay-text-2a8f00.txt` proves creation order. Direct Text, authored LineN and dynamic lines call setup(255), then base color and authored alpha setter. Localized/split 2A9AD0 calls the authored alpha setter and base color before setup(255). Consequently a newly bound direct line has authored/clamped target with current 255, while a split line retains target/current 255. Binding does not pretend that an update or draw has already happened.

`text-update-29cc00.txt` 29D080..29D108 compares target alpha to the **low byte of last-rendered alpha**, not current float. For nonpositive setter duration, elapsed=0 is not less than duration, so it selects 29D0A8's immediate setter. This branch does not advance interpolation elapsed. A target equal to the last-rendered low byte intentionally leaves current float untouched. Dirty flags clear at each admitted update.

`text-update-tail-29d460.txt` 29D464..29D5C0 performs color/alpha packet preparation and stores rendered signed16 alpha during draw. `projectLine` therefore remains separate from update. `originalUiTextPacketColor50KG` supplies the already-reviewed RGB >> 1 and signed16 alpha arithmetic >> 2 byte projection. No guessed material normalization is introduced.

The supplementary read-only ELF words in `research/static-anchor-words.txt` confirm constructor 29C850 initializes hold +84=-1, indefinite +88=0, alpha duration +A0=0 and lock +CC=0. They also close the internal 29DCC0 setter: integer-to-float current-alpha store and dirty flag only, with no extra clamp or lock gate. Unit-scale/zero-rotation setters at 29DCE0/29DCF0 have the corresponding current/dirty stores.

## Admission and bounded omissions

This owner supports the static path actually used by construction and selection: negative hold duration, zero blink period, unit scale, zero rotation and exactly -1 second-phase delay. It does not fast-forward a timed animation to that path. Positive alpha setter durations, timed holds, nonzero blink, scheduled phases and non-unit transforms fail explicitly. Nonfinite inputs fail. Locked requests return `lockedOut` before request-value admission, matching the source no-op gate.

Native raw setup/authored alpha is admitted only in signed16 range. These integers are exactly representable as binary32 and survive the source float-to-integer-to-signed16 projection without a host rounding dependency. The alpha setter itself accepts int32 and clamps as the source does. Other bounds are 64 lines and 255 bytes per line, matching the sibling reader. Empty lines and widget ID zero are admitted.

The zero-period blink countdown has no observable effect in this bounded path and is not accumulated. Child hierarchy override flags, widget fade/toggle callbacks, dynamic text replacement, numeric text animation, active interpolation clocks and transformed geometry are outside this owner. It does not resolve fonts, alter page navigation, upload assets or submit graphics.

## Verification boundary

Review is by source inspection only. No Git, active edits, build, syntax check, test, importer/decoder execution or gameplay run occurred. The source anchors establish the callback semantics and bounded API contract; actual original asset authentication, compilation and integrated rendering remain parent gates.
