# PositionStats radar shared strip corners

Status: UNTESTED source change, 2026-10-17.
Baseline: d352a14129b243fce76a1f7c40d6c9619fd74ab7.

## Change and active path

In tests/native/original_position_stats_hud50la.h, prepareNativeDisplay expands
each enabled radar strip packet into quads. The first quad still calls
convertOriginalRadarOverlayDepthIndependentVertex50LA for all four corners.
Later quads copy their two shared endpoint vertices and colors from the preceding
successful quad, then convert their two new corners. Reuse resets at each packet.

Current source order is {v+1,v,v+2,v+3}. At the preceding v-2, corners 3 and 2
therefore read exactly source indices v+1 and v. They become current corners 0
and 1 respectively. After the unchanged quadCount increment, the preceding quad
is at quadCount-2. The v>0 guard ensures a prior completed quad in this packet,
regardless of the packet's starting global output offset.

The converter in platform/linux/native/original_radar_overlay_material_adapter50la.h
accepts only the same per-packet material and source STQ/RGBA arrays. It performs
material-policy, finite-component and color-range checks, then copies XY and
color with inert Z. It has no vertex ordinal, time, callback or allocation.
Output is a separate fixed-size array. No packet source/material mutation occurs
between these iterations; a previous failure already returned. All new endpoint
checks, depth assignments, capacity guards, packet order and diagnostics remain.
A failure retains the same completed prefix and leaves draw_.prepared false.
No reuse crosses packets or frames and no dynamic material check is cached.

The producer is not restricted to one quad: traverseRadar1924C0 in
src/game/original_position_stats_radar_route50la.h emits predecessor, visible
node and closing successor. finishRadarElement192624 in
tests/native/original_position_stats_primitives50la.h doubles point counts into
strip vertices. Three points can produce six active vertices and two quads;
packet capacity reaches 72 vertices. This establishes source reachability, not
measured runtime frequency. For Q emitted quads within one packet, converter
calls decrease from 4Q to 2Q+2, saving 2(Q-1) calls. GPU geometry and draw count
are unchanged; no measured CPU or FPS gain is claimed.

## Successor validation

Build and compare first/multiple quads, multiple packets, disabled map preference,
invalid material/endpoint rejection, packet boundary counts and failed-prefix
publication. Confirm radar geometry/colors/order against the original route and
measure on an equivalent renderer/workload before attributing gains. No build,
test, syntax check, benchmark or gameplay ran for this source change.
