# Own canonical RA material keys in retained cold families

UNTESTED. No builds, tests, syntax checks, smoke or benchmarks executed.

retainPsmSources normalizes the requested material member into its map key.
sourceEntry accepts only a unique archive row with the same normalized name.
OriginalStandardMaterialSource50LA::prepare stores that row's member verbatim.
Therefore the existing PSM map key equals normalized(family.materialMember) when
prepareImages constructs its sole family row. Copy that key into an appended
owned canonicalMaterialKey field; retain the original raw materialMember.

The hot expected-key locals in profile pass validation, profile classification
and crowdSurfaceProof now borrow const references to this family-owned string.
Actual material-source normalization, original byte comparisons, both pass
checks, family/collector checks and all currentness gates still execute.
Nothing is inferred from a name alone and no runtime admission result is cached.

Root's repository source inventory found one family construction/insertion and
one raw materialMember writer. Later cold mutations concern image/context data,
not names. Public exposure is a const vector. prepareImages is single-attempt;
imagesComplete remains false until the entire cold stage succeeds, so failed
partial families cannot be read as current. Close clears families and disables
currentness. A copied/moved family owns its key; no map iterator/pointer survives.

This adds a small cold string allocation per family, covered by existing caught
failure before publication. Removed hot allocations can change bad_alloc timing.
The ordinary successful service/query route can avoid five expected-name
normalizations per crowd surface (two proof calls and three profile uses). This
is source-path counting, not measured frequency or timing. Two independent design
reviews accepted the proof. Successor should validate original/case-varied source
names, distinct representative families, stale/closed owners and cold failures,
then compare equivalent runtime output before measuring performance.

This is native metadata only; rebuild native consumers. No external precompiled
binary-layout compatibility is promised. Original asset formats are unchanged.
