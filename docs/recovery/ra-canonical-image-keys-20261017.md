# RA owned canonical image keys

Status: UNTESTED source change, 2026-10-17. No measured FPS gain.
Baseline: a043ec5990b8db3791179ecde7a8fd92897bba74.

## Change

ColdFamily retains an owned canonicalImageKey. The existing normalization after
selectImageSource now writes directly into that field; the local key used by
image duplicate/extension admission is a const reference to it. The raw archive
sourceMember spelling and original source bytes remain unchanged.

Only two downstream expressions change: the expected imageName in
 decodeStaticMaterialProfile and the expected crowd image comparison in
crowdSurfaceProof. The latter is called by both serviceCrowdSurface241848 and
currentCrowdSurfaceMap24B26C; the successful query then decodes the profile.
This removes three repeated expected-name normalizations along that complete
service/query route. It does not cache or remove actual image/texture source
normalization, original-byte comparison, collector checks or currentness gates.

## Ownership and failure reasoning

selectImageSource is the sole family.sourceMember writer in the retained world
includes. It authenticates the retained archive member before assigning the raw
name. Each new family receives its key before duplicate/extension admission and
before moving into imageFamilies_. Later image building updates other family
metadata and copies the raw name into separate ColdImage rows; it does not edit
the retained family name. Public exposure is const and the owner is serial.

The key is an owned string, not a pointer into an archive entry or a local map.
The cold local reference is used only before the family moves; no use follows
the move. Vector relocation preserves string values. Both hot consumers require
coldImagesCurrent; imagesComplete_ becomes true only after complete preparation.
Preparation is single-attempt, and close clears the families and completeness.
Failed cold allocation remains within the existing exception/nonpublication path.

The prior temporary normalized string becomes retained metadata, increasing cold
memory lifetime. Native structure layout changes; no old-binary ABI or padding
promise is made. Replaced hot normalizations no longer have their allocation
failure opportunities. All remaining validation and diagnostic order is retained.
No persistent admission result, animation state or material state is cached.

## Successor validation

Rebuild and compare authenticated RA cold setup and crowd service/query behavior,
including close/failure paths. Compare the same route, renderer, resolution and
warm-up interval before attributing any performance change. This source has not
been built, executed, syntax-checked or benchmarked by the outgoing session.
