# Retained RA placement lookup

UNTESTED. Base53f7ebc. No builds, syntax checks, tests, benchmarks or gameplay.

prepareRaWorldFrame resolves each actual selected worldInstance to its retained
visual Placement. The old code scans the entire placements_ vector from the
beginning for each selection. This patch changes only that search to lower_bound
and retains the exact equality/null/LOD rejection immediately afterward.

## Sorted ownership proof

ra_static_world50la.h's cold source loop visits data->world.instances in strictly
increasing wi order. A collision-only record is skipped without a visual entry.
Every visual record allocates a nonnull Placement, sets worldInstance=wi and
pushes it once. Hence the completed vector has strictly increasing keys despite
gaps. Its sole push_back is this producer; there is no insertion, reorder,
sort or key mutation elsewhere. Failure prefixes cannot become a current
complete draw plan.

The renderer retains the SAME Owner and prepareRaWorldFrame first requires
raWorldCurrent. That validates aggregate identity, renderer publication,
complete draw plan and current GPU image borrow. Owner::canClose rejects a live
renderer or image publication. The only later vector mutation is ordered cold
release/clear during close, after publication retirement. Public placement
access is const; no new mutable view or cache is introduced.

The new lookup therefore returns the same unique pointer as the linear search,
or null for a missing collision-only/out-of-range key. Original selected-record
order, per-surface processing and all downstream source/material checks remain.

## Cost limit

A selected record now requires logarithmic comparisons among1085 retained
visual placements rather than a prefix-length linear scan. It adds no heap
allocation, source copies, state lifetime, GPU work or numerical operation.
Actual frame-time benefit is unknown; no measurement was performed.
