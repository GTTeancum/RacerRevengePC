# Vehicle draw surviving template fields

Status: UNTESTED source change. Baseline 63c0a54b, 2026-10-17.

The final publication loop in src/game/vehicle_draw50cm.cpp previously copied
all 24 Vertex48B template bytes, then always overwrote position and conditionally
overwrote color. It now copies only the eight UV bytes; the existing generated
color loop remains, with a raw four-byte template-color copy when sourceColors is
absent. The position assignment and bounds arithmetic remain unchanged.

src/assets/native_asset48b.h declares float position[3], float uv[2] and
uint8_t color[4], with sizeof(Vertex48B)==24. The existing projector asserts
IEEE binary32 floats. Those fields exhaust all 24 bytes on the supported layout;
no padding bytes are omitted. Raw UV/color memcpy preserves bit representations,
including UV signed zero or NaN payloads, without new floating-point operations.
Every destination member is assigned exactly as before.

All fallible preparation precedes this loop. The header's distinct live
input/destination/workspace/output contract excludes aliasing. No callback or
allocation is introduced; diagnostics, transaction boundary, normal publication,
geometry order and bounds are unchanged. The copy no longer reads template
positions that would immediately be replaced, or template colors when generated
colors replace them. Structurally this removes 12 or 16 copied bytes per emitted
vertex. A compiler may already eliminate overwritten stores; no measured gain or
memory-traffic claim is made.

Runtime reachability: tests/native/terrain_interactive_drive50du.cpp calls
buildVehicleDrawBatch50CO for the followed player and DrawOwner::prepare for
opponents. src/game/vehicle_draw_batch50co.cpp calls buildVehicleDraw50CM for
each requested component. No racer AI, controls, handling or physics code changes.

Successor validation: rebuild and compare full vertex buffers and bounds for
source-color and template-color routes, multiple components, and rejection paths.
Measure equivalent race/renderer workloads before attributing FPS changes.
No build, test, syntax check, benchmark or gameplay ran for this patch.
