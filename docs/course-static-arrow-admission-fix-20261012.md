# Course resource preparation: static-arrow controller admission

Source-only candidate, 2026-10-12. Base commit: `c6814e73ccfd179c26c700a4337f5005a1d8b0b2`.

## Result

The fresh Course rejection is explained by a deterministic resource admission mistake: `OriginalUiCourseDrawResourceError50KG::animation` is ordinal 8. The seventh authored material loaded by `OriginalUiCourseDrawResources50KG::load`, `ui/textures/arrwlefttop_d`, uses the genuine static `arrwleft_d.pst` while its authenticated PSM declares `TextureAnimation=unique`. The existing `addImages` conflates stored ATM frames with an authored controller declaration and rejects that combination. The same mistake affects all four disabled-arrow materials. Earlier fonts and the enabled left arrow do not have this mismatch.

`course-resource-static-controller.patch` is the proposed two-file source fix. It narrowly authenticates the four existing disabled-arrow PSM/PST pairs, preserves and parses each original unique-controller configuration, and leaves the actual static PST image ownership/selection intact. It does not request nonexistent disabled-arrow ATMs, borrow enabled-arrow frames, invent a one-frame animation, suppress the authored metadata, or relax the general non-ATM animation guard. Existing enabled-arrow ATM parsing, frame bounds, animation clocks and independent material instances remain unchanged.

`course-resource-diagnostics.patch` is independently optional. It adds the existing nested Course resource, widget, material, decoder, font and text diagnostics to the rejection receipt. It changes no admission or gameplay state. The existing material/image index fields are not a claim that every preparation stage assigns them.

## Runtime evidence inspected

`/workspace/shared/recovery-20261012/course-validation-03/take-01/game.log` and `summary.json` retain the failure after successful Main and Rider19 opening:

- `screen=5 page=1 status=2 preparation_status=10`
- `rider=3 rider_pc=2008580` is the outer rejected-Confirm preflight result.
- `course_resource=8 course_resource_pc=0`; the old guard supplies no PC.
- No destination or gameplay was entered; world ticks remained zero.

The candidate also preserves parser failure PCs rather than overwriting a nested animation failure with a zero-PC outer failure. It assigns `0x29B950` to bounded animation-config rejections.

## Authenticated authored inputs

The Course archive's committed catalog identity is 1,227,424 bytes, SHA-256 `f45585dd7fe7ba2a24cfc5b6ce4d3fb5efa6bc279f636e071aecf5ccf970c855`. Its two disabled textures are actual PSTs; neither corresponding ATM exists in the catalog:

| Texture member | Bytes | SHA-256 |
|---|---:|---|
| `ui\\textures\\arrwleft_d.pst` | 2076 | `a43491d2613d6b0920cb9df458eb8eb9f0cefa180f5e8bf13da72f22749af6fc` |
| `ui\\textures\\arrwrght_d.pst` | 2076 | `9548ccdc542e3dfec68a565318c82b80733da5033764d474497dda2f90973c44` |

Each of the four PSMs has `TextureAnimation=unique`, reverse-on-end 1, loop-to 0, with omitted initial-frame and looping fields retaining original defaults 0 and true:

| Material | Texture | Authored FPS | PSM SHA-256 |
|---|---|---:|---|
| `arrwlefttop_d` | `arrwleft_d` | 8 | `a5d49c37534e75ec408d83a58bd3312d071695f21a48716a25924d2b0f7599be` |
| `arrwleft_d` | `arrwleft_d` | 16 | `a324ab97242eb460ca250bf83f09aeb9309cb73052953f630774653bdc22ca7c` |
| `arrwrghttop_d` | `arrwrght_d` | 8 | `233874168c767be19710a41378c2ce885a0b37074ce520c5d58e8909b65ddefb` |
| `arrwrght_d` | `arrwrght_d` | 8 | `233874168c767be19710a41378c2ce885a0b37074ce520c5d58e8909b65ddefb` |

The retained previously extracted PSM bytes were read and hashed against the committed member catalog. They are local original-data evidence, not source files to commit. The patch itself carries only native code and explanatory comments.

## Original ownership and getter proof

Original ELF: `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`, SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Raw-word excerpts are retained in `evidence/original-controller-and-getters.words.txt`; their address column is file offset, and VA = file offset + `0x0FFF80`. No original instructions were executed.

- `29B890` calls controller constructors independently of texture storage. `29B950` reads the authored animation mode, constructor defaults and overrides; `29BAA0/29BAA8` retains unique ownership.
- Original PST construction at `24E1B0` calls `29C6E0`, installs vtable `3D21C0`, and uses its `+0x18` texture accessor `29C6D0`.
- `29C6D0` is `jr ra; por v0,a0,zero`: it returns the same static texture regardless of a requested animation frame.
- The separate ATM accessor `102050` loads owner `+0x24`, indexes it by the requested frame, and returns that stored texture. Its loader `102230` reads the authored frame count into `+0x20`. Those semantics must not be invented for a PST.
- Original `29A020` reads a presumed animating texture's `+0x20` as a count. The static texture instead has its resource-name storage beginning there (`29C6E0` construction and `24DF08..24DF18` name copy). The candidate does not reproduce that layout-dependent cast or claim numeric phase equivalence for these static resources. Its bounded native projection retains the controller declaration/configuration and actual fixed PST getter result; only genuine ATM resources advance the native numeric frame-selection helper.

This last boundary is explicit: the patch corrects native resource admission and source texture selection. It does not create or tick a live native numeric controller for static PSTs and does not claim parity for the original object-layout-dependent numeric state. The authentic PSM remains in each independent material binding. The Course composer currently registers each pass by its own identity and original registry token and sorts by that order; it does not call the general equivalence comparator. This patch therefore does not establish original unique-controller scheduling. No generic shader-policy or material-equivalence rule is changed.

### Numeric-controller side effects

The full original `29A020..29A304` was inspected. Its normal present-resource numeric path reads the shader's map handle, performs the texture-registry lookup, reads the existing application clock, and writes only its own timestamp, phase and direction/stopped flags. Its only numeric library calls are the retained `302CB0` floating-remainder function. The virtual shader map-handle getter `24AE60` is the four-word pure indexing/getter sequence `00051040 00441021 03E00008 8442001A`. No RNG call or audio, profile, gameplay, particle or event mutation appears in this normal numeric path. Missing-resource assertion/reporting and math-error handling remain original exceptional behavior; the patch continues requiring authentic present resources before publication.

The outer `29AC90..29AD68` has repeat-timestamp and pause gates, an optional callback, and optional vertex/pixel-animation calls. Constructor `29AAA0` initializes callback/context to zero (`29AB20` and `29AB34`) and previous timestamp to -1. The four disabled-arrow PSMs declare neither vertex nor pixel animation nor animatesWhilePaused. Thus no authored callback/RNG side effect was identified for these resources. This read-through does not prove every possible external mutation of the original controller; none is implemented or silently assumed by this bounded metadata-admission fix.

The immutable actual PST choice is independent of the omitted numeric phase because the static getter returns its own object for every requested frame, rather than indexing a frame array. Shared RNG, menu time, pause gates for real ATM animations, profile generations and renderer lifetime are unchanged by the candidate.

## All Course resource rows: same-class audit

All 15 authored rows and all five fallback rows in `addImages` were inspected against the retained original PSM field text and the committed authenticated member catalog. `evidence/course-material-animation-fields.txt` contains the bounded field excerpts and source-file hash lines. No project parser or decoder was run.

| Binding indices | Materials | Source/image-controller finding |
|---|---|---|
| 0–4 | denmrk16top, denmrk16, denmrk14, denmrk14top, venus16top | One ScreenObject pass; no TextureAnimation field. Explicit map names match, or the original material-basename default supplies denmrk16/denmrk14. Static font PST/CEL path is consistent. |
| 5, 7, 9, 11 | arrwlefttop, arrwrghttop, arrwleft, arrwrght | One map to left/right ATM; unique, FPS16, reverse1, loop-to0. Initial0/looping1 are original omitted defaults. Actual same-member ATM identities are already authenticated. No static/animated mismatch. |
| 6, 8, 10, 12 | arrwlefttop_d, arrwrghttop_d, arrwleft_d, arrwrght_d | Exactly the four mismatches fixed by this candidate; original FPS8/8/16/8 and fixed PST selection retained. |
| 13 | overlay1 | One ScreenObject pass and one overlay1 map, no TextureAnimation field. Its genuine texture comes from controllerPopup. |
| 14 | notexture | One ScreenObject pass, explicit zero maps, no TextureAnimation field. It deliberately bypasses addImages and requires untextured material policy. |
| 15 | nameblur | Authenticated absent-PSM fallback; original screenTextureFallback constructs an empty authored section. Static Course PST. |
| 16–19 | icon_x, icon_tri, icon_sqr, icon_cir | Authenticated absent-PSM fallback; empty authored section. Cross/triangle use OVERLAYS; square/circle use Course. No animation declaration is synthesized. |

The separate raw `tat_bkg.pst` background has no PSM/controller path in this loader. No additional same-class TextureAnimation-versus-storage mismatch was found in the remaining rows. This is a static field/ownership audit, not proof that later image decoding, native sampler/material admission, widgets, geometry or the Course renderer succeeds.

## Guard and lifetime review

- Whole-archive and member authentication are untouched; the added helper requires the actual Course archive, exact known registered PSM identity, exact PST member identity, one stored image and absent corresponding ATM.
- The actual pass must still have one texture map with the exact expected texture name. All static cases outside the four fixed disabled-arrow pairs still reject authored animation.
- The retained configuration must match each authenticated source's known 8/16 FPS settings and original defaults. No unknown named controller mode is accepted.
- The PSTs remain ordinary source-backed images, deduplicated by actual member provenance. Distinct registered materials and their distinct retained controller configs remain separate even when they share one PST.
- Bank construction, sampler conflicts, material policy conversion, widget preparation, popup ownership, renderer publication/retirement, profile commits, UI lifecycle and race admission are unchanged.

## Integration and verification boundary

Root owns integration and the hourly check. Candidate files are under `candidate/`, exact copied preimages under `base/`. The optional diagnostic candidate/preimage are separate. Main was not edited by this worker. Other root integration changes appeared while the work ran; neither primary target was changed in the worker's checkout.

Only static source, retained logs, original data, diffs and hashes were inspected. No tests, builds, syntax checks, imports, sanitizers, project decoder/parser execution, game launch, gameplay, native or guest execution occurred. No runtime success or compile success is claimed. The reported next eligible root-owned slot is 18:24:36 UTC, subject to root's current ledger.

The next authorized integrated check still needs to establish successful Course cold preparation, accepted Course rendering, fixed selection and Back into freshly prepared Rider19 with zero world ticks. If preparation progresses to a different failure, preserve the fresh nested receipt; this patch does not assert that downstream Course code has already been validated.

## Root integration review

Root manually reviewed the native PST getter, fixed frame-zero selection, exact authenticated pair admission, existing ATM advancement and nested diagnostic field types. No definite source blocker was found. Integrated on top of1499e840; source-only, no new execution check. Runtime Course and Back still require the centralized slot.
