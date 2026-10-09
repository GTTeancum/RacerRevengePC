# Host completed-lap HUD 50GI

The optional Linux 50DS host overlay adds a small top-left `HOST LAPS` count.
It displays completed laps, including zero, with no leading zeroes. It does not
infer a current-lap number, target number of laps, race finish, or original HUD
parity. The existing heat, damage, boost/overheat and HOST NEXT route cue remain.

## Caller contract

`VehicleWindowHostHud50DS` appends default-disabled `bool lapCountValid = false`
and `std::uint32_t completedLaps = 0`. Set the flag only when actual source lap
state is available. Enabled counts must be 0 through
`maxVehicleWindowCompletedLaps50DS` (999); larger values reject submission with
a frame error before frame GL work. Disabled counts are ignored. A caller that
needs a bounded display of a larger source value must clamp explicitly without
changing the source state. Renderer code does not perform that clamp.

The existing cold `enableHostHud` opt-in and per-frame `hostHud` pointer still
control all HUD resources and drawing. A null pointer hides the entire HUD.

## Bounded rendering

The readout uses the same host-authored 5x7 glyph runs, rectangle builder, fixed
512-quad storage, GPU buffers, indexed draw and state-restoration path. Only
glyphs P and 0 through 9 are added. Number formatting uses four stack bytes;
there are no new per-frame allocations or GL resources. The count panel costs
at most 112 quads, giving a conservative all-features bound of 504 quads with
OVERHEAT, full bars and the route arrow. The original overflow guard is retained.

The count panel sits opposite the top-right route cue. When enabled, minimum
vertical layout space becomes 100 cells so it does not overlap the lower-left
heat/damage panel at small viewports. Disabled layout remains unchanged. Track
culling, scene/cable draws, private publication and cleanup are unchanged.

## Verification scope

This change is source-only. Builds, tests, runtime pixels and live lap-source
integration were not performed by the renderer change. Pre-edit renderer copies
are preserved in the task's `solo-lap50gfgg/preimage/platform/linux/native/`
directory without replacing earlier preimages.
