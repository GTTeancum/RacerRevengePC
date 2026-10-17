# Reuse the retained normalized material name within a pass check

UNTESTED. No build, test, syntax check, smoke or benchmark.

The reached RA crowd profile decoder validates original and representative passes.
Each validatePass previously copied and normalized sourceFamily.materialMember
for map lookup, then repeated the same normalization for a provenance comparison.
An owned local string now serves both uses. The normalizer only converts ASCII
case and separators; the retained const family does not change between them.

Actual material-source normalization, original byte comparisons, both validator
calls, all pass/state/provenance and freshness checks remain unchanged. The local
string cannot dangle. Two duplicate string copies/scans are removed per successful
profile decode. Small-string optimization and compiler effects are not assumed;
allocation timing and bad_alloc incidence may change, but the existing exception
handler remains fail-closed. No persistent cache or new authority is introduced.

Two independent source-only reviews accepted the design. Exact runtime behavior
and performance remain untested. Successor should validate crowd profile admission
and rejection diagnostics before comparing timing. This is not a Windows crash fix.

The same bounded reuse is applied in crowdSurfaceProof. Its null-family branch
returns the exact previous rejection message before normalization. One owned
materialMember value is then used in the existing family predicate and later
psms_.at lookup. SourceMember normalization, collector provenance, byte equality,
frame/lease checks and catch behavior are unchanged. This proof is reached from
both crowd service and current-map query. The local value survives only that
proof invocation; no admission is reused across calls or surfaces.
