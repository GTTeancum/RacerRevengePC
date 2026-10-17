# Summarize immutable diffuse raster IDs within one publication

UNTESTED. No builds, tests, syntax checks, smoke or benchmarks.

Each newly constructed private StaticDiffuseStream starts allRastersEqual=true.
After assigning each nonfirst surface raster ID, preparation clears that flag if
it differs from the first ID, never restoring it. Failed preparation destroys
this unpublished stream; a new stream never inherits the old summary.

visitDiffuseSurface retains its initial currentness check. For a nonempty uniform
stream, it performs exactly the prior scan's one live diffuseRasterCurrent and
passFlags check on the first ID. Previously the loop walked every remaining row
only to skip the repeated ID. Mixed and empty streams retain that exact original
adjacent-deduplicated loop. The binary target lookup, callback and final current
check are unchanged. No admission result is cached and no allocation is added.

Current cold admission permits only the unique black.psm family for native
static diffuse lighting. The stream emits only those lit rows, proving a real
uniform path. The source removes N-1 immutable ID comparisons per uniform visit;
no surface count or FPS gain was measured. Two independent design reviews accepted
failure, empty, mixed and lifetime reasoning. Successor should validate these
cases and live material invalidation before comparing frame timing.
