# Native ALL8 TimeStats frame boundary

Source-only implementation, not a Windows or Linux runtime result.

The native adapter binds genuine Bank read handles to the existing timing, lap,
impact and vehicle-state owners. It verifies the actual camera generation,
manager identity, native ALL8 bootstrap and published viewport. Rank and active
count are copied from the actual original-AI manager. It introduces no driver,
clock, input or independent standings owner.

A frame is a synchronous scalar snapshot after the existing gap-write and camera
publication boundary. Bank reads prohibit active leases; there is no separate
cross-thread manager epoch. Viewport and preferences are borrowed only for that
synchronous preparation. Calling from another thread or before manager updates
is outside this admission.

The TimeStats owner prepares original state/layout and bounded CPU draw packets:
six panel quads and at most six times forty glyphs, in eighteen packets maximum.
Actual canonical final leaves determine stable submission order. Effective map
state is revalidated each preparation; unsupported state fails closed. Three
canonical texture demand tokens are resolved during cold preparation and used
without temporary string allocation in the display path. Text depth is the
retained actual Video depth, not an independently selected overlay depth.

Advancing the accepted native world invalidates any previously prepared draw.
Preparation also clears readiness before validation. Renderer consumers must
retain a strong owner until GPU resources and pixel borrows retire; they may not
queue raw frame pointers across world updates or camera teardown.

This is an explicit bounded native-host scheduling policy. It does not complete
or claim the retail 1AAD40/1891A0 parent hierarchy or its global traversal gates.
The renderer and terrain activation require separate integration and Windows
verification. No software test was run for this source change.
