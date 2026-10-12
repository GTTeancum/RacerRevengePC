# Shared selector RNG guard/cache candidate

Source-only candidate, 2026-10-12. Root owns integration, main, and the hourly validation slot. Nothing in this directory has been built, compiled, syntax-checked, tested, run as a project script, or executed as gameplay. No live repository source was edited and no branch was created.

## Integration set

Apply only `minimal.patch`, or use the five files under `changedfiles/` against the corresponding four existing files under `preimages/`. `SOURCE-MANIFEST.json` records the exact preimage and candidate identities; `BASE-COMMIT.txt` records the source checkout HEAD. The new common header had no preimage. There are no new translation units or build-manifest additions.

`optional-completed-prefix.patch` and `OPTIONAL-COMPLETED-PREFIX.md` are explicitly excluded from this integration set and the next validation build.

## Definite bug addressed

The native race host passed the same actual `s.noise.sharedRandomState` to two independent seed caches: `sessionLazy` for Dax/lap/contact music and `gcAnimationLazy` for GC animation. Contact music also retained its own range guard/cache independently of GC. These are copies of the same original storage, not source-private selector slots. A successful GC animation call could seed first, yet a subsequent first Dax/lap/contact selection still found its own cold cache and seeded the raw stream again.

The candidate creates exactly one `SharedRandomOwner50ID sessionRandom` outside the racer snapshot bank. Its `seed` and `range` subobjects are passed by reference:

- Dax preparation and category6/mode1 flush borrow `sessionRandom.seed`.
- Dynamic-contact music borrows `sessionRandom.seed` and `sessionRandom.range`.
- GC world service borrows the full `sessionRandom` through the retained `ServiceShared` API name.

The source raw RNG and application clock references are unchanged. No initialization is inferred from unrelated raw RNG draws or seeding. A nonzero cached timestamp suppresses the extra seed across these consumers. A zero timestamp still permits the next selector to sample and seed again, as in source. No draw, clock sample, range assertion, or callback was moved within its existing successful-call sequence.

## API and lifecycle

The existing `LapEventRandomOwner50ID` and `DynamicContactRangeOwner50IW` definitions move unchanged in member order, types and defaults to `src/game/shared_random_owner50id.h`. Their consuming APIs remain unchanged. `gc_animated_world50IG::ServiceShared` remains a public type name and the service call signature is unchanged, but that name now aliases the common aggregate; its former direct fields are expressed as `seed` and `range` projections. Source inspection found no existing direct-field callers outside the updated GC implementation. This is source-level compatibility for current callers, not an ABI guarantee.

Current native race policy remains explicit: a new `runSession50DU` reconstructs `f::Fixture` and its physics raw RNG, so it also reconstructs both selector guard/cache pairs at the same boundary. The application monotonic clock survives race restarts. Original statics instead have application lifetime. This candidate fixes intra-session sharing without claiming cross-race original parity or persisting a nonzero seed cache across a separately reset raw RNG.

The small common header can be borrowed by the excluded Course/Rider voice candidate later, without importing contact-event machinery. That later integration must remove its private cache, pass the correct shared owner and raw RNG, and choose one consistent application/menu/race lifetime. This candidate does not add an application field, enable menu-to-race, integrate Course/Rider voice, or add fog volumes.

## Preserved limitations

- GC service still stages guard/cache/raw changes and publishes them only after `advance` succeeds. Original stores/draws occur before later traversal and assertion failures. This existing native failure deviation is intentionally preserved in `minimal.patch`; the separate optional patch illustrates the prefix-preserving change only.
- GC service still performs its existing unconditional safe-domain rejection; it does not newly honor `rangeCheck3D6B38 == 0` as permission to admit an out-of-range draw.
- Category6 still performs its existing direct numeric/range check rather than initializing or consulting shared80AC at source1A583C. That pre-existing assertion-model gap is not expanded here.
- No floating-point configuration, conversion, rounding, numeric domain, draw arithmetic, source schedules, participant order, or emulation policy changes are included.

## Source evidence

The source-only reviewer identified the same addresses in the retained original `SLUS_202.68`, SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`, with file offset `VA - 0x100000 + 0x80`:

- World service10189C..1018E0: guard gp-0x7F58 (80A8), zero/cache3D6B30, conditional101A50, then308020. Range guard/cache80AC/3D6B38 occurs at101900..101938.
- Contact music22A410..22A454: the same seed guard/cache; clock at22A43C, cached result store22A448, seed22A450.
- Last-lap1A57D8..1A581C: the same seed guard/cache. Range guard read at1A583C remains outside this bounded assertion-policy change.
- Fog160998..160A44 and Rider1EA65C..1EA704: identical seed and range static addresses. These routes supply identity evidence only; neither is admitted by this candidate.
- Shared308020 stores into the actual libc RNG owner+58.308030 advances that word using the retained LCG and returns its low31 bits.

Useful retained review inputs:

- `/workspace/scratch/74cd1d3d7a45/course-shared-selector-review-20261012.md`
- `/workspace/shared/gc-local-fog-research-20261012/evidence/fog-populate-160520-160eb0.txt`
- `/workspace/shared/gc-local-fog-research-20261012/evidence/shared-rng-308010-308060.txt`
- `/workspace/shared/course-recovery-20261012/rider/evidence/confirm-static-elf-extract.txt`

Only source reading, file copying/editing, diff generation, and artifact hashing were used. No validation pass is claimed. Root should reconcile the preimages against main before its own centrally authorized integration and validation.

## Root integration status

The five-file ownership change has been integrated into main source after independent static review and exact preimage verification. It remains uncompiled and untested pending the next single centralized integrated GC session. The optional completed-prefix patch was not applied. Course source remains excluded from build; its future menu/race shared-owner join is still pending.
