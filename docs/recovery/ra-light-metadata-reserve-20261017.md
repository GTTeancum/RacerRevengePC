# RA publication metadata reservation

Status: independently source-reviewed; UNTESTED. Base f767dc7. No builds, syntax checks, tests, gameplay,
profiling or benchmarks. No performance measurement or speedup claim.

Binding::prepare reserves its Snapshot and RecoveredNormalSurface vectors once,
using the actual immutable drawRequirements_.staticLightingSurfaces count after
the existing initial current/publication admission. No normal-bank capacity is
reserved from the unrelated all-world vertex count.

The cold draw-plan producer starts the count at zero and increments once for each
appended row whose actual raster requires native static lighting. It marks the
plan complete only after authentic source/material/range validation. current()
requires drawPlanCurrent; the retained same world does not mutate this plan.
Binding iterates exactly these lit rows and retains its existing final count
comparison. Thus reservation uses its genuine target metadata extent.

reserve changes capacity only, leaving row order, size, source validation,
normal recovery, current lighting and all publication gates unchanged. It avoids
geometric reallocation/copy of the two metadata vectors. The normal byte vector
still grows from actual recovered vertices as before.

Either reserve can throw at this earlier native allocation point. The existing
aggregate try/catch owns the unpublished Binding in a unique_ptr, reports the
same native static lighting allocation diagnostic and destroys any partially
reserved backing. An allocation failure can therefore precede the first source
row validation; this is an explicit native allocation timing change. No partial
publication or success is introduced, and the first current gate still precedes
all new allocation. Actual runtime benefit remains unknown.
