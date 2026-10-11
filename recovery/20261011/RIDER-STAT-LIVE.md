# Live original Rider custom stat packet owner

Source-only candidate. No build, syntax check, native parser, test, gameplay or guest execution.

`OriginalUiRiderStatOwner50KG` pins real stat assets, constructs all198 actual custom sprite instances, owns persistent dimensions/colors and the real unique medstrsm clock, and emits the exact ordered stat projection as real sprite quads and real font glyph layouts. Labels use original LOC200..205, denmrk14 CEL/PST, capacity20 and source right justification. This is not a complete screen or GPU submission.

## Actual live behavior

- Source constructor2DEF40 initializes sprite white RGB, alpha255, scale1/depth1/angle0.2DF180 resolves actual initial atlas dimensions. Panel sizes persist: entering mode2 changes height135; another mode does not reset it. Right panel width292 and notch width5/height10 follow original writes. Full texture UVs remain0..1 when dimensions stretch.
- Explicit `applyAppearance` corresponds to1EB640 custom-object effects, independently from drawing. It colors panels,13 outlines and six labels with actual profile accent. Notches are recolored only through each actual maximum count; current count selects accent versus0x333333. Earned medals, portraits and marker keep constructor white. Base DAT widget recoloring remains the page/widget owner’s responsibility.
- `project` requires the same source rider whose appearance was actually applied. It consumes real profile observations through the shared stat projection, retains source draw order, and exposes the actual widget4 positioning effect for the page/widget owner.
- Sprite packet bytes use source RGB>>1 and alpha255>>2. Source ScreenText initialization2E0000..2E00D4 likewise sets white/255. Text glyphs are produced by shared numeric layout;2E1534 right justification truncates X after subtracting measured integer width, while Y keeps the original fractional panel motion.
- Marker animation is read from the real PSM: unique15fps, original initial/loop/reverse defaults and overrides. Shared texture animation arithmetic retains source timestamp/repeat semantics. The caller supplies its already-sampled application clock and actual material-update pause gate; no host clock is sampled. A packet references the real stored ATM frame.
- Frame output borrows pinned resource/atlas pointers and owner-local image indices. The complete frame composer must drain previous frames, remap exact identities through the combined bank, apply actual material policies and packet depth/video maximum, and admit the full source target.

## Important correction and remaining source phase

1ECBF0’s float+10 floor is on an actual texture resource returned by2DF8A0, not a material draw-order field or sprite Z. The getter resolves material resource+4C, texture ID+1A, then the texture registry. This live owner deliberately does not invent a material-order field. The exact texture-residency/resource-phase join is separately required; its source listing is retained here. Earlier stat notes suggesting material ordering are superseded by this evidence.

The live stat owner provides no destination-readiness flag. Original page/profile state, widget effects, texture-resource phase, complete frame ordering and root validation remain required.
