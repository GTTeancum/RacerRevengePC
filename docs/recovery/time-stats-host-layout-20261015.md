# Host HUD coexistence with authentic TimeStats

Source-only candidate, one focused CPP patch. Screenshot inspected: track-1.png from passing57f081f; native live-clock/header and top of standings visibly overlap the original LAP/TIME/LEAD panels. No generated image or runtime check was performed.

## Gate and scope

`submit` now prepares actual TimeStats packets before building host HUD geometry. A requested invalid TimeStats frame still rejects. Only a nonempty renderer-accepted packet copy reserves space. The occupied bottom is computed from those actual copied quad vertices, transformed with exactly the renderer's `min(width/640,height/448)` scale and vertical letterbox offset. No new public input field, caller readiness flag, or original coordinate change.

With that reservation, the native duplicate LAP and live RACE/LAP TIME block is omitted. Compact POS plus the existing status/countdown/GO/wrong-way text is placed below the original geometry. Standings and completed-lap/best information follow four supplemental cells below it. Bottom heat/damage and top-right NEXT retain existing coordinates and scale. Results panel path remains unchanged. Empty/absent TimeStats leaves the previous layout unchanged.

## Bounds

The supplemental group begins eight existing HUD-scale pixels below the original occupied bottom. Its common scale is capped by existing HUD scale, available width (drawable width minus68 HUD cells), and available height (drawable height minus reserved top minus68 HUD cells). The entire compact status32 + gap4 + standings/completed-laps group fits before the heat panel. This retains at least four HUD cells above the heat panel and eight HUD cells to the left of NEXT at the maximum-width bound. Status and standings share their contracted scale rather than independently colliding.

For the shown640x448 frame, original panel bottom93 gives native status top101, status bottom133, standings top137 and zero-completed-laps standings bottom277. Heat remains at384. At1280x720 with HUD scale2, the original bottom maps to approximately149.46; native status starts165.46, standings237.46 and ends517.46, above heat592. At960x540 the group contracts jointly when its original2x size would not fit. Other aspect ratios use the actual vertical letterbox offset rather than assuming the original panel starts at drawable y0.

If a drawable has no positive supplemental space, only the compact status and standings are omitted. It does not reject valid gameplay or fall back to the overlapping native lap/time block. Heat/damage, NEXT, authentic TimeStats packets and their strict renderer admission remain unchanged. Finite-positive drawable extents are validated before these calculations.

## Integration

Apply `host-layout.patch` to `platform/linux/native/vehicle_window_gl50ds.cpp`. The only changes are private host geometry layout and moving the already-existing TimeStats frame preparation earlier within CPU-only submission preflight. No gameplay, AI, clocks, source formatting, authentic resources, source packet construction, GL submission order, tests or build files change. Root owns integration and verification.
