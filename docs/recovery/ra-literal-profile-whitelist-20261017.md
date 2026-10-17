# RA literal ordinary-profile whitelist

Status: UNTESTED source change. Baseline de99212b, 2026-10-17.

In tests/native/ra_static_world_profiles50la.inl the same 33 ordinary material
stems are now complete textures/<stem>.psm string literals. Comparisons against
the authenticated canonical name no longer construct a temporary concatenated
std::string for each member. All entries, ordering, full traversal after a match,
classification and the unknown-family rejection remain unchanged.

This is cold/loading work. The draw-plan builder classifies each retained family;
33 ordinary families each compare all 33 members, producing 1089 concatenation
expressions in the old complete ordinary-family path. The per-frame crowd branch
matches earlier and bypasses this whitelist. No gameplay FPS claim is made.
Static literal storage replaces temporary strings; removed allocations also
remove their potential failure points. All actual provenance and state checks
before and after classification remain.

Successor validation: rebuild and exercise authenticated ordinary families plus
unknown names and all special profiles. No build, syntax check, test or benchmark
was run for this source change; runtime loading benefit is unmeasured.
