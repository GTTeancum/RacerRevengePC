# Index retained RA archive names during cold preparation

UNTESTED. No builds, tests, syntax checks, smoke or benchmarks.

The private sourceEntry helper previously normalized the query and every archive
entry name on each lookup to establish absence, a unique match or ambiguity.
Cold preparation now builds an owned map from the same normalized names to entry
indices and an ambiguity flag. A repeated canonical name marks only that key
ambiguous; unrelated queries are not rejected. Each query still normalizes its
input and returns null for absent/ambiguous keys, otherwise the retained original
entry at the recorded index. A defensive index bound check is included.

Build occurs inside the existing prepare try after resolver binding and before
the first lookup. The source owner is noncopyable/nonmovable and decoded once;
world ownership is shared_ptr<const Owner>. Its public decoded view is const,
and the two source friends lease only mutable live-world state, not archive
entries. Root source search found no archive mutation in those friends. The map
owns strings and numeric indices, not borrowed rows or names. Close clears it
before releasing source ownership. No provider, archive revision, member byte or
publication check is removed.

New map allocations can fail earlier than old per-query normalization; the
existing caught failure leaves staged=false and no retry/publication. Duplicate
handling preserves query-specific ambiguity even for unrelated duplicates.

On a successful RA world, 176 collision-only placements each require two full
lookups over 1,010 entries: 355,520 archive-name normalizations from that branch
alone. The index normalizes those names once, then normalizes each query and uses
map lookup. Other pinned PSM/image lookups also reuse it. This is source-derived
loading-time work reduction, not measured startup or FPS improvement. Two
independent design reviews accepted it. Successor should compare all lookups,
case/slash aliases, missing names, duplicate ambiguity and failure cleanup, then
measure actual cold load time separately from steady-state rendering.
