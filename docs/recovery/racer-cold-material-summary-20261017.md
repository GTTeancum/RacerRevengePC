Cold racer material summary (untested)

Base: cc6ff68525b3e0711a5f1af382f3f56ceac301f7
Patch: rr-racer-material-summary-cc6ff68-20261017T0020.patch
Scope: platform/linux/native/vehicle_window_gl50ds.cpp only.
Independent and integration source reviews accepted. No execution validation performed.

Change
During the existing cold player/archetype material copies, accumulate two
booleans per component: whether any policy requests source diffuse RGBA and
whether any policy requests the source specular pair. RacerView exposes a
const view of those summaries. retainedMesh consumes the summaries rather
than scanning the entire private material bank to recompute the same ORs.

Exactness and ownership
- validateMaterials requires each binding count to equal the corresponding
  immutable scene's complete material count, for player and every archetype.
  Both cold loops therefore accumulate exactly the policies previously scanned
  in retainedMesh, including unused material entries.
- Summary booleans start false. Neutral mode performs no material copies and
  leaves false/false, exactly as the old null-material-bank branch did.
- Racer policies are copied only by createMaterials/createRacerArchetypes.
  Frame updates modify separate UV fields. bindOriginalUiTextures replaces only
  original UI texture fields/publication and never touches the racer bank.
- create refuses an existing Storage and allocates fresh Storage for a new
  initialization. initialize only marks it initialized at its successful end.
  A partially created owner is either destroyed after cleanup or retained
  non-initialized for cleanup retry. It cannot submit or be recreated in place.
  Successful close destroys Storage; later recreation starts fresh summaries.
- This adds fixed in-owner booleans only, with no allocation or resource cleanup
  requirement. No borrowed summary survives its owning Storage.

Validation boundaries
All live sourceColorMaterialModes byte checks, span checks, geometry identities,
source publication/provenance checks and normal checks remain unchanged. The
existing stack normal-receipt scope and ready() callback boundary are unchanged.
No GL commands, callbacks, error gates, geometry, material selection, lighting
arithmetic, or source rendering policy changes.

Source evidence
- platform/linux/native/vehicle_window_gl50ds.cpp: validateMaterials,
  createMaterials, createRacerArchetypes, retainedMesh, initialize, create,
  bindOriginalUiTextures, cleanup, close.
- platform/linux/native/vehicle_window_gl50ds.h: lifetime contract requiring
  loaded assets to stay unchanged until close; private renderer ownership.

Expected work reduction is one full cold-policy OR scan per retainedMesh call.
This is a small CPU preflight optimization; no frame-rate or wall-time gain
has been measured. No builds, tests, syntax checks, gameplay runs, benchmarks,
or instrumentation execution were performed. Untested.
