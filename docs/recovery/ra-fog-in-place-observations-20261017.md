# Build private RA fog placement observations in place

UNTESTED. No build, syntax check, smoke, benchmark or gameplay run.

The reached per-frame RA preparation creates a fresh fog Binding and reserves
its exact placement count. Previously each completed PlacementObservation was
built locally and copied into the vector. It is now emplaced before filling,
with a reference to that row used by the unchanged validation and arithmetic.
The final push_back copy is removed.

Every nonstatic member has an explicit initializer. In particular packetModel
starts with zero lanes before the world-space identity/translation writes.
The reference is used before the next insertion. Existing reserve covers all
rows; no pointer is retained across bindings or publications. No numeric
operation, finite/range check, camera revision check or source ordering changes.

prepare is private and called on a newly allocated unpublished Binding. The
aggregate publishes only after success. A failed row can now exist inside the
failed temporary, but it is destroyed and never exposed. current() does not
read placements_. No portable padding-byte equivalence is claimed. The change
removes an explicit source-level copy per completed row; compiler behavior and
runtime/FPS effect have not been measured.

Two independent source-only reviews accepted the minimal replacement and
publication/failure reasoning. Successor validation should compare the existing
RA route and observation values before assessing any timing effect. This change
is later than the b4c303e Linux smoke and Windows executable; it is unrelated to
the unresolved Windows loading report and must not be called its fix.
