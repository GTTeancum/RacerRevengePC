# Stage10: retained original material-update traversal prefix

Incremental after1099ea9. Source-only; no builds, tests, syntax checks,
executables, gameplay, or SLUS execution. No shared frontend/profile/result edits.

Exact file ownership:
platform/linux/native/original_ui_tournament_material_update50kg.h (new)
src/assets/original_ui_material50kg.cpp (portrait name.c_str() correction)
this report and tournament-material-update.txt (static original byte transcription).

1F24B0 is texture-residency demand, NOT draw sorting or geometry.
2DF8A0 reads ScreenTexture54 -> material index -> actual382B38 table298E60,
then material first pass4C -> signed texture token1A -> actual383400
table298E60. The returned TextureResource10 is raised to10 if below10.
24E670 subsequently consumes demand10 for resident-level selection24E4A0.

Exact traversal, without deduplication:
portrait1000 caller1F24D4;
13 pairs outline4C0+60*i caller1F2508,icon9A0+60*i caller1F253C;
visible trophyE80/EE0/F40 callers1F2584/1F25BC/1F25F4;
markerFA0 caller1F2624;
six rows of actual3D60 counts,1060+780*row+60*n caller1F2668.
No background or numeric text visits. No popup gate in this function.
1F26C4 invokes inherited141C30 after the traversal. Its child traversal,
flag1000 and widget29 dispatch are separate unresolved lifecycle requirements.

The new header-only lease prepares all requests transactionally, at most151,
and retains the SAME career resource owner. Real generation/mode checks gate
preparation and every request access. Stale leases retain immutable assets
until lease destruction; they do not return new valid requests. Borrowed name
pointers require the lease/resource to remain alive. Request image indices
are provenance aids only, not canonical texture tokens or readiness receipts.
medstar uses one map demand visit, never nine per-frame visits.

BLOCKED boundary: no actual texture-demand writes are implemented or claimed.
The canonical MaterialTable/TextureTable APIs keep mutable resource access
private to their material owner, and their bounded population path does not
yet join this Tournament ScreenTexture/unique texture animation domain.
A recipe/image bank cannot stand in for those live registry objects.
Root integration needs a canonical2DF8A0 consumer with same retained tables,
populated nonnull pass/map resources, and the inherited141C30 widget owner.
Only then may these requests raise the real shared demand10 and reach the
actual residency phase. No callbacks accepting arbitrary mutable textures,
synthetic handles, demand setters, or frontend admission were introduced.

Also fixes root's source-review blocker: the new portrait fallback passes
name.c_str() to the existing const-char* screenTextureFallback parameter.
Root's separate trophy catalog fix1d74e95 remains required and unduplicated.
