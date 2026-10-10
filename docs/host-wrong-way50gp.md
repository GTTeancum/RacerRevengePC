# Host wrong-way warning 50GP

The optional Linux 50DS host overlay reuses the existing top-left lap panel for
a caller-supplied direction warning. When enabled, `WRONG WAY` replaces
`HOST LAPS` in red-orange (`0xFF6556`), while the completed-lap count remains
visible in its existing light color. Both labels occupy the same nine-character
width. There is no additional panel, inferred race result, or original HUD
parity claim.

## Caller contract

`VehicleWindowHostHud50DS` appends `bool wrongWay = false`. The renderer reads
this flag only inside the existing `lapCountValid` panel branch. A true warning
with `lapCountValid == false` has no visual effect; it does not enable the panel
or change layout. Clearing the warning restores `HOST LAPS` immediately.

The caller owns the direction decision and supplies the recovered source flag;
the renderer does not infer heading, motion, route progress, or lap state. The
completed count contract is unchanged: enabled values from 0 through 999 are
accepted, larger values reject before frame GL work, and disabled counts are
ignored. The flag does not increment, suppress, reset, or otherwise alter laps.
Source production and live caller wiring are separate from this renderer change.

The existing cold `enableHostHud` opt-in and per-frame `hostHud` pointer retain
their behavior. A null pointer still hides the entire HUD. Heat, damage,
boost/overheat priority, and the optional HOST NEXT route cue are unchanged.

## Exact fixed-storage bound

Each horizontal run in a host-authored 5x7 glyph uses one quad. The added W
uses 17 runs; Y and the other warning letters already existed. Spaces use none.
With the current glyphs, the largest admitted simultaneous geometry is:

| Component | Quads |
| --- | ---: |
| Lower-left panel border and fill | 2 |
| `HOST HUD` | 77 |
| `OVERHEAT` (larger than `BOOST` or `IDLE`) | 83 |
| `HEAT`, `L DMG`, `R DMG` | 39 + 46 + 51 |
| Three bar borders, backgrounds and nonzero fills | 9 |
| Base HUD subtotal | 307 |
| Route panel border/fill, `HOST`, `NEXT`, shaft/head | 2 + 39 + 42 + 2 = 85 |
| Lap panel border/fill | 2 |
| `WRONG WAY` | 106 |
| Completed count, maximum at `800` | 35 |
| Warning/count panel subtotal | 143 |
| All features together | 535 |

Zero has the most runs of any digit (12), but three-digit formatting never
starts with zero. Eight has the most runs of any allowed leading digit (11),
so `800` reaches the exact 35-run count bound. For comparison, `HOST LAPS`
uses 74 runs and the normal count panel tops out at 111 quads; its exact total
is 503. The earlier 50GI bound of 504 was conservative. Replacing its label
with the warning increases that exact total by 32, exceeding the old
512-quad capacity.

Cold fixed capacity therefore increases modestly to 576 quads. CPU and GPU
vertex storage each grow from 49,152 to 55,296 bytes (48 to 54 KiB), using four
24-byte vertices per quad. The immutable GPU index buffer grows from 12,288
to 13,824 bytes (12 to 13.5 KiB); its temporary cold upload array grows equally.
Capacity leaves 41 quads beyond the exact admitted worst case. That frame
submits 2,140 vertices and 3,210 indices through the existing single HUD draw.
Cold index readback verification retains its 256-index chunks but bounds the
last chunk to the remaining 128 indices: 3,456 indices do not divide evenly
by 256. Neither GL readback nor CPU comparison runs past the enlarged buffer.

All storage remains bounded and allocated only during cold HUD setup. Number
formatting still uses four stack bytes, and there are no new hot allocations,
per-frame GL resources, or draw calls. The existing overflow guard remains.
The warning changes only the selected label and its color in the existing
batch. Panel dimensions/position, scaling, track culling, material/cable
rendering, GL state restoration, private publication and cleanup are unchanged.

## Verification scope

This is a source-only change. Glyph-run arithmetic and the scoped source diff
were inspected; no builds, tests, runtime screenshots, or live-source behavior
were executed or claimed. Original renderer files are preserved under
`solo-wrongway50go/preimage/platform/linux/native/` without overwriting any
existing snapshots.
