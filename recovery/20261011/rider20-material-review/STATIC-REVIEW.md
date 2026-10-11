# Independent source review: exact Rider20 material admission

Reviewed 2026-10-11, final refresh approximately 10:36 UTC.

## Result

No blocking source defect found in the three-file staged candidate. Its new admission is limited to the actual `20_1` and `20_2` specular pass2 PSM identities and the matched `20_shdw` PSM/PST pair. The existing Rider19 policies remain unchanged in behavior. The ordinary RGB-only material/renderer path remains closed to the special blends.

This is source/original-byte review only. It does not establish compilation, native decode success, uploaded GPU readiness, shader compilation, rendering fidelity, or Rider20 movement/session readiness. The candidate is suitable for the parent's authorized source integration subject to the existing verification freeze; it does not clear that freeze or any movement gate.

## Snapshot and scope

Requested baseline: `03df3c864e572e3ef174ec16a920f583f4d9521d`.
Observed clean active HEAD while reviewing: `93083c51a0e5cce4c8e98351b9e73e5ae187182a`.
All three staged preimages independently match both the requested baseline and the active worktree byte-for-byte. The inspected renderer, material adapters, field defaults, and catalog also have no changes between those two commits.

Reviewed patch SHA256: `4b664a31286f8c38b5bcc6c84c702cbc9f46dbe42adfe7a12331daa022ace641`.
The patch was read in full and agrees with the direct preimage/candidate diff. The manifest and hash-list values agree with independently read file hashes.

Candidate SHA256 values:

- `src/assets/original_ui_material50kg.cpp`: `b2c3e0ac3aeb0c01bcfc215cf022cfc4f120cee38ea9d89e89603d91788d4d06`
- `src/assets/original_ui_material50kg.h`: `64601265d98ee91e5ceeca765afb0f1853f93ba6f356646b06eb0343ebe9053c`
- `platform/linux/native/original_ui_rider_draw_contribution50kg.cpp`: `3ba7384a549515d5e0a67d1b5661b940a5d755da14934d9cabd70f0c570de3f4`

No active source was changed by this review. Inspection consisted of source/text reading, Git snapshot/diff reading, original-byte hashing, and Python standard-library fixed-header and byte-equality inspection. No build, test, syntax check, project parser/decoder/importer execution, sanitizer, native/guest launch, GL probe, or PS2/VU emulation occurred.

## Original evidence independently checked

The retained original files under `next-rider-admission/members/rider/ui/textures` were read and their hashes checked against the immutable catalog:

- `19_1.psm`: 5198 bytes, `d63f9544ce9db203c637026850cb631ce3fb1b660e393a86cbecf19c50947856`
- `19_2.psm`: 5198 bytes, `dc58bad6421e57927f51d449f73e44737ca33aa35e568020d65ebe2bf57289a8`
- `20_1.psm`: 5198 bytes, `9b95ee0473d9db39d0e4afdaccfd1f6dbffc9a1584b6dfe046654df68f0cf13b`
- `20_2.psm`: 5198 bytes, `4277ecbecda24ec5a00e54c884d89b4e42f17e5e8a18c4e86b2ba3fe077e3b53`
- `19_shdw.psm`: 2624 bytes, `b7612d86899932fc84faba786d705c29decb6e4201a7d332d0e7cbed8d7015fa`
- `20_shdw.psm`: 2622 bytes, `b648d543ff05022a310db17cd7700d8a1e2452f9180fe10e741b9fb7276bd7be`
- `19_shdw.pst`: 17436 bytes, `74182f87bba2b29cd8e27f8df2542e0e8548fbfecf0cb6b14bf7543abab1b39c`
- `20_shdw.pst`: 17436 bytes, `7bbcaf638b960cb335936a84f5880e5b6da1cfadb3c98e2acd22941f6952d57a`
- `specmap1.pst`: 66588 bytes, `d928592fbf2b0b7bd47075992bdcef7c9be426a766994148dce98ea2360f3f84`

`20_1.psm` equals `19_1.psm` after only replacing the body texture basename, and the same holds for `20_2`/`19_2`. The shadow PSMs have the same relationship after ignoring the final CRLF difference. The shadow images are distinct and cannot substitute for one another.

The three inspected PST headers have revision1, flags0, kind1, and one mip. Their only mip headers at byte1036 are `(128,128,0,0)` for the shadows and `(256,256,0,0)` for specmap1. The full file lengths equal the header/palette/mip-header/index extent. This is fixed-record evidence, not a decode run.

The original two body PSMs actively declare two shader passes. Pass2 is `SpecularMapCompressed`, one `specmap1`, UV0, ALPHA `(0,2,1,1)`, blending enabled, and `zWriteDisabled=1`. The actual DefaultMT and unchanged constructor defaults supply fog ON, alpha-test ON/GREATER/reference0/KEEP, and GEQUAL.

Both shadow PSMs actively declare one lowercase `diffusecompressed` pass, the corresponding own texture basename, ALPHA `(2,1,0,1)`, and blending enabled. The same genuine defaults give fog ON, alpha-test ON/GREATER/reference0/KEEP, GEQUAL, and depth writes ON. The unchanged `inputAlphaD` compatibility alias supplies the source D field.

All five character `20c_*` PSMs actively declare one pass; their two-pass line is commented and their second sections are dormant. The unchanged material reader iterates only through active `TotalShaderPasses`. The candidate does not admit those PSM identities or `20_4` to the specular wrapper.

## Canonical identities and explicit null rejection

The final candidate adds an explicit `!pass.identity` rejection before any catalog equality in both typed wrappers, at material.cpp lines827 and868. Thus a null incoming identity can never match a null failed lookup. A non-null incoming identity also cannot equal a missing catalog member. The PST branches independently retain their explicit `!member` rejection before dereference or image admission.

The added lookups are demonstrably present in the unchanged static Rider catalog:

- `ui\\textures\\20_shdw.pst`: catalog source line776, global member index757, Rider-local ordinal410.
- `ui\\textures\\20_shdw.psm`: line777, global index758, Rider-local ordinal411.
- `ui\\textures\\20_1.psm`: line778, global index759, Rider-local ordinal412.
- `ui\\textures\\20_2.psm`: line779, global index760, Rider-local ordinal413.

`OriginalUiArchiveId50KG::rider` is explicitly8 in archive50kg.h. Catalog entry8 is the Rider archive, with firstMember347 and memberCount552. All four ordinals are inside that actual Rider slice. The other lookups used by these wrappers, including both19 shadows, both19 body PSMs, pod_wndw, and specmap1, are also real entries inside the same slice. DefaultMT belongs to the explicit AppStart slice. The retained original bytes' lengths and SHA256 values above agree with these canonical entries.

The lookup is not a transient allocation or a second identity table. `originalUiArchiveIdentity50KG` returns the address of the static archive entry; `originalUiMemberIdentity50KG` returns `&memberCatalog[firstMember + ordinal]` after its bounds checks (`original_ui_archive50kg.cpp:107-116`). Material `catalogMember` loops that exact archive's memberCount and returns the accessor's non-null address on the exact case-insensitive name match (`original_ui_material50kg.cpp:78-85`). Therefore each listed lookup resolves to the corresponding stable, non-null canonical member object by source inspection.

Authentication remains tied to those same entries. Archive load verifies the compressed archive length/hash, the full member table, each original member extent, and every member hash before publishing storage (`original_ui_archive50kg.cpp:125-171`). `member()` and `find()` return those same memberCatalog addresses only from loaded storage (`:185-196`). DAT loading requires a loaded archive and nonempty member, then carries that canonical member identity (`original_ui_pages50kg.cpp:150-165`); material reading copies it into each actual pass. This review inspected that source path without running archive loading, parsing, or decoding.

The explicit null guards are defense in depth and preserve successful19 behavior. The final refreshed patch differs from the first reviewed candidate only by those two rejection lines; final source and patch hashes were reread, and the two other candidate hashes remain unchanged.

## Exact admission and fail-closed behavior

### Shadow matched pair

At candidate `original_ui_material50kg.cpp:827-843`, the wrapper accepts only the two canonical catalog PSM pointers. The selected PSM determines both the required texture basename and the canonical PST member. A 20 PSM paired with the 19 basename fails the profile check; a 20 PSM paired with the 19 image fails provenance. The reverse mismatch also fails. Neither a suffix nor matching dimensions grant admission.

All previous shadow state checks remain: pass1, DiffuseCompressed, A/B/C/D `(2,1,0,1)`, blending enabled, GREATER0/KEEP, fog, GEQUAL, and depth write. The image still needs a loaded bank, canonical member identity, complete PST extent at offset0, no CEL or IPUM provenance, kind1, one mip, and 128x128 dimensions. Every published RGBA byte must equal the decoded original mip. Result publication occurs only after all checks and common material validation succeed.

The shared private gate still verifies authored-PSM origin, canonical DefaultMT, enabled one-map UV0 profile, known source fields, register widths, revision/flags, TCC/TFX, clamp/PABE/FBA/DAT restrictions, sampler bounds, and exact mip RGBA extent. Only this typed wrapper reaches its private authenticated-shadow route.

The original19 branch has the same PSM, basename, PST, state, image, and common-gate conditions as before. Diagnostic wording changes do not alter its admission.

### Specular pass2

At candidate `original_ui_material50kg.cpp:868-887`, the old body identity set remains exact19_1/19_2; the added set is exact20_1/20_2. The existing pod_wndw identity remains separate. Every accepted input still requires pass2, `SpecularMapCompressed`, and exact specmap1 basename. Body ALPHA.C remains1; only pod_wndw uses C2 with FIX255.

For the two new20 bodies, explicit fog ON, ATE ON/GREATER/reference0/KEEP, and GEQUAL checks match the originals. The unchanged private common gate independently requires ALPHA A0/B2/D1, blending ON, and depth writes OFF, and translates C1 to the destination-alpha additive policy. The 20-only condition does not restrict or broaden existing19 or window behavior.

The specmap1 proof is unchanged: exact canonical PST, complete extent, no CEL/IPUM substitution, kind1/one mip, 256x256, and published RGBA byte equality. The shared source identity, DefaultMT, enabled-map, field, mip, and sampler checks remain mandatory. Dormant character sections, other riders, arbitrary specular programs, different images, and other pass indices remain rejected.

### Dispatch and renderer

The only draw-contribution edit is an exact `20_shdw` basename dispatch alongside exact `19_shdw`. Dispatch still calls the typed identity-and-image wrapper, then the existing matching-bank converter. It does not itself establish admission. All other generic, indexed body, IPUM, window, specular, ordering, publication, and lifetime code is unchanged.

The unchanged typed adapters retain concrete image-bank pointer, image index, texture-count, and image-identity checks. The shadow adapter retains attenuation blend, DiffuseCompressed, fog/ATE, GEQUAL, and depth writes. The specular adapter retains its own program/blend and disabled-depth-write checks. The ordinary common converter rejects every special blend through its default switch arm; generic lit conversion cannot bypass that rejection.

The unchanged renderer rejects special blends on the RGB-only entry before drawing. Its source specular validation additionally requires GEQUAL, no depth writes, fog and ATE; shadow requires GEQUAL, depth writes, fog and ATE. Unknown blend values reject. The sealed source-RGBA entry requires an actual owner, fresh complete-frame generation, and source binding-history admission before submission.

Its unchanged byte arithmetic preserves the genuine equations: body specular adds `floor(Cs*Ad/128)` to Cd; shadow adds `floor(-Cd*As/128)` to Cd. Source MODULATE clamps/quantizes before GREATER0 testing, fog affects RGB only, and every surviving fragment replaces stored alpha with the source alpha. A discarded fragment writes neither color, alpha, nor depth. The candidate changes no shader, ordering, barrier, color-mask, sampler, source-owner, or presentation behavior.

## Explicit ordinary-RGB and movement-gate findings

1. **Ordinary admission remains closed.** The generic lit entry calls the private common gate without either authenticated special flag. That keeps `SpecularMapCompressed` outside its admitted programs, and ordinary blended Diffuse still requires ALPHA.A0 rather than the shadow's A2. Neither new identity opens a generic conversion route.
2. **Ordinary span conversion still rejects.** `convertOriginalUiMaterialSpan50KG` supports only opaque, sourceAlpha, and additive, then returns false in the default arm. Destination-alpha additive, FIX255 additive, and shadow attenuation therefore reject. The typed adapters use a local temporary ordinary raster solely to obtain existing sampler/depth checks, then restore the distinct special blend; the submitted output retains that special identity.
3. **RGB-only renderer submission still rejects before drawing.** `submitOriginalUi(native,...)` calls the private implementation with sourceRgba=false. Shadow's explicit sourceRgba check and specular's explicit sourceRgba check reject before draw setup or framebuffer clear (`vehicle_window_gl50ds.cpp:3898-3900,4009-4016`). No candidate renderer edit exists.
4. **Directional movement remains unavailable.** In unchanged `original_ui_rider_session50kg.cpp:444-452`, every successfully planned input0/1 returns `unavailableDestination` before real preflight/selection/transform/audio/deferred work. Planning does not execute a selection. The same file still rejects input4 at line463 and retains the selectedRider19 Back guard at line467. It has no diff between the requested baseline and the inspected active commit and is absent from the candidate. The final patch contains only material.cpp, its header comments, and the exact shadow-dispatch edit; it alters no session, planner, callback, cold bank, or movement gate.

## Boundaries and remaining work

No placeholder images, broad roster/suffix allowlist, new IPUM cache entry, dormant pass admission, or movement enablement is introduced. The two changed executable regions are material admission and exact shadow dispatch. API names remain legacy19 names; the updated declarations describe the exact expanded scope.

This review approves no native activation. A material admission result remains insufficient for cold bank/GPU publication, genuine source material ownership epochs, corrected Rider callback chronology, orientation transfer, audio/deferred consumers, Back/reentry policy, or complete-frame success. Those are separate dependencies documented in the existing next-rider reports. Central verification, when separately authorized, must still exercise both exact20 policies, cross-pair rejection, unsupported identities/passes, preservation of19/window behavior, and RGB-only rejection.
