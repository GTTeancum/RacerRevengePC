# Exact lit-vertex normal reservation

Status: independently source-reviewed; UNTESTED. Base29641c0. No builds, syntax checks, tests, gameplay,
profiling or benchmarks. Performance benefit remains unmeasured.

DrawPlanRequirements gains an appended host-only staticLightingVertices count,
leaving all existing field positions unchanged. Static source search found no
positional initializers or consumers beyond the actual world/lighting owners.
During the existing source-order cold plan loop, each lit record contributes its
actual vertexCount. Before addition, the total must not exceed vertices_.size()
and the contribution must fit vertices_.size()-total. No unchecked addition or
extra source pass is introduced.

The authentic lowerPlacement producer appends each surface's vertices once,
recording vertexStart from the vector size before appending. Its ranges are
therefore disjoint; their total is bounded by max_mesh_vertices (1,000,000).
Draw-plan construction already validates every source/range/material identity.
The new count belongs to that completed immutable plan and is not an independent
readiness receipt. Partial plan failure retains the existing terminal admission.

After the initial current/publication gate, Binding::prepare reserves its flat
recoveredNormals_ vector to exactly this lit-vertex count, alongside the existing
metadata reservations. It still appends only successful original normal recovery
results and now requires the final normal count to equal the cold demand before
publishing the Binding. Dynamic lighting and RGBA projection are unchanged.

This avoids normal-vector geometric growth and copies without reserving unused
prelit/world vertices. It adds no persistent owner or cross-publication cache.
An allocation failure now occurs at reserve rather than a later append; the
existing aggregate try/catch destroys the unpublished Binding and reports native
static lighting allocation failure. All source-validation ordering after the
allocation point remains unchanged. No successful partial state is published.
