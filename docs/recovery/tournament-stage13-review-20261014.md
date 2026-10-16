# Stage 13 source review

Reviewed the supplied `0001-Populate-finite-Tournament-missing-PSM-materials-thr.patch` against root `16190dc06eeee7d802104df50b5976e069e97f98` on 2026-10-14. Source inspection and static byte reads only. No patch application, production edit, test, build, syntax check, import, project script, launch or original executable execution.

## Verdict

No definite blocking C++ API or ownership defect found in this bounded leaf. This is a genuine canonical missing-PSM material population path. It does not establish a complete Tournament session, texture population, renderer readiness, demand traversal or transactional rollback.

## Admission and provenance

* The public enum admits precisely six trophy names, `notch`, and 23 portrait names: career 1–26 excluding 5, 6 and 13. Invalid enum values, nonportrait career arguments and out-of-range object indices fail before reference effects.
* Icon indices 0–12 map to `9A0 + 60*i`; award indices 0–2 map to `E80 + 60*i`; notch indices 0–119 map to `1060 + 60*i`, equivalent to six rows of twenty at stride `780`; portrait is `1000`. Kind/award-slot correspondence, actual profile selection, actual constructed notch counts and constructor chronology remain caller responsibilities. Bounds alone do not prove those observations.
* Acquisition requires the authentic loaded Tournament archive identity, its real PST and standing page, and absence of the corresponding PSM. The existing directory validator accepts only the actual bounded `ui\\textures` spelling (ASCII case folding and one optional trailing backslash). It does not invent a directory or registry row.
* The private table entry receives the archive absence proof and the exact texture-table owner only after successful real `retainScreen14D590`. Conflicting archive instances, changed texture tables, and an already-loaded row without that proof are rejected. Later population rejects a substituted texture table and any supplied original PSM contradicting the retained absence proof.
* Construction rechecks authentic archive identity/absence and calls the existing finite trophy/notch/portrait builders. Their exact archive, PST, page, caller-PC and missing-PSM origin survive in `tournamentFallbackProvenance()`. `source()` remains null. The empty section passed to `singleState` is an attribute-read equivalent for the original NULL-document call, not synthesized authored PSM text or identity.
* This absence is proven inside the authenticated Tournament archive. There is no new mounted-namespace/provider absence receipt here. The root session must still join the actual source provider and reject conflicting/ambiguous mounted inputs. The patch documentation expressly leaves that join open; do not promote this leaf to full resolver/session admission.

## Canonical identity and actual construction

* The existing `screenApplicationMatches` gate checks the same retained APPSTART and exact template backing. Population independently checks the sealed live construction context's actual application/APPSTART/template backing. The fallback requires the actual associated ScreenMT pointer, rather than an equal-looking caller recipe.
* `construct29B550` continues to use the actual canonical row/name. The new fallback creates one real `Storage::Pass`, preserving constructor flags=2, UV selectors=0, group=-1, fill defaults, and empty parameter/animation owners.
* It resolves the associated template microprogram through the real prepared `382B70` table, sets one map, and obtains/retains the actual signed texture token through the same bound `383400` table using the full original material name without the PSM suffix.
* State construction reuses the canonical `singleState`: template/source field provenance, source context default zero, real retained runtime and live Video snapshots, and original sampler defaults. No immutable recipe token is substituted for a program, texture, state or resource owner.
* Static words at source `29B620` through `29B6AC` and `2475C0` through `247694` match the included evidence: missing-file branch, suffix removal, one pass, real program lookup, one map, signed texture token, UV zero, texture retain, NULL attributes to `2476A0`, and state publication. These bytes were read, never executed.

## Lifetimes and failure semantics

* Each ScreenTexture owner retains one real numeric material reference, including duplicate names. It releases exactly once through the existing `release299AC0`/`release14D500` path; no deduplication or fake receipt is introduced.
* Table rows pin the authentic archive and exact texture table, so destroying individual ScreenTexture owners cannot dangle the population proof. Those pins persist until table destruction, even after the last row reference is released. They do not preserve a loaded material: the existing zero-reference release still invalidates collectors and destroys its real object/pass/texture refs.
* Acquisition is **not all-or-nothing**: after preflight it pins owners, marks `started`, and stores the lookup row before retaining. Missing-row or retain failure leaves a started, nonretained reference with explicit diagnostic effects. This follows stage 12's partial constructor contract; destruction does not release an unretained row. The actual numeric retain plus attaching the two shared ownership pins has no subsequent fallible operation.
* Population is **terminal partial construction**, not transaction rollback. It can bind context and acquire a texture reference before a later state/live-context failure. The row remains unpublished, the partial object stays owned by the terminal material table, and existing pass destruction releases the acquired token. Earlier successfully published rows also remain real effects. The root must treat failure as terminal and perform session cleanup rather than retry or claim atomic whole-table success.
* The texture acquire helper writes `-1` on entry and exposes a nonnegative token only on successful retain. Partial-pass cleanup therefore neither releases an unsuccessful retain nor leaks a successful one. The retained templates keep the raw construction-template pointer live, including failure cleanup. The table's shared construction/texture/program owners survive the partial object's lifetime.

## API/build boundary

Source inspection finds the new function declarations, table friendship, builder enum ordering, runtime accessor, diagnostics, default section construction and changed private `construct29B550` call consistent with current definitions. No compilation was performed.

No new translation units or build-list registrations are needed in stage 13. The stage 12 reference CPP is already registered by root. The three modified canonical/reference CPPs are existing build entries.

The authored outline demand entry explicitly rejects fallback references. Missing-PSM demand hooks, unique marker ATM, complete texture source/residency ownership, inherited widget traversal, session generations and renderer retirement remain open integration work. A retained reference or populated material must not be used as a ready-frame receipt.
