# Remove redundant HUD vertex resets before full assignment

UNTESTED source-only cleanup. No execution checks or measured performance gain.

RaceHud, Damage, PositionStats, TimeStats and SpeedBoost preparation no longer
zero an individual Vertex48B immediately before assigning all of its members.
Each path writes all three position floats, both UV floats and all four color
bytes in the same order and from the same validated inputs as before. No failing
operation, callback or consumer occurs between those assignments.

The supported native Vertex48B layout is exactly24 bytes: five binary32 floats
and four uint8_t bytes, with a size assertion in native_asset48b.h. There is no
unassigned struct-padding region in this layout. This change is not generalized
to other structs, alternate layouts or partially assigned vertex records.

SpeedBoost retains its preceding four-slot zero loop, so the unused fourth slot
of a genuine three-vertex primitive remains zero. Cold arrays, all source checks,
packet coverage/range validation and frame publication gates remain unchanged.
Failed preparation leaves frame false and does not expose partially filled data.

This removes one source-level aggregate zero assignment per filled HUD vertex.
It does not reduce uploaded bytes; a compiler may already eliminate these dead
writes. A successor may validate byte-identical prepared vertices, especially
SpeedBoost's unused triangle slot, before measuring whether generated code differs.
