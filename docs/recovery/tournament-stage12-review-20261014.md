# Tournament stage 12 canonical outline reference review

Date: 2026-10-14. Reviewed supplied patch `8950b2d15275f2eb7410c6b6137fe00ba1511b51` against current canonical recovery source. Root described the integrated baseline as ec5d7f4; the read-only HEAD observation during review was `528876d5953e284ef6374a258c5dc794ec395652`. This reviewer did not alter production source, apply a patch, commit, build, test, run syntax checks/imports/scripts, or launch project/guest executables. Original SLUS was read as static bytes only. This report is outside the repository.

## Verdict

No definite source/API compile blocker or ownership defect found. This patch creates a real, narrowly scoped canonical outline-reference and demand-write path, beyond stage 10's immutable traversal recipe. It does not finish the full Tournament material-update traversal or session/renderer integration. The private gates preserve same-table/template identity, source bytes, real numeric reference counts, and the direct source floor10 operation. Runtime and compilation are unverified.

## Exact registry identity and private boundary

- New public `OriginalUiScreenMaterialReference50KG` admits only authenticated Tournament outline branch1EF884, outline ordinal0..12, actual sampled directory `ui\\textures` with case folding and at most one final backslash, real authored medlgout PSM/PST, prepared application owner, material table, and prepared expected texture table. Empty/unknown directory, slash-normalized guesses, unsupported outline, absent authored material, wrong archive, and terminal population/demand owners reject.
- `screenApplicationMatches` checks the SAME retained APPSTART shared-pointer identity and the exact `OriginalUiMaterialTemplates50KG` object address in the supplied prepared application. The canonical table's existing application prepareFresh overload aliases that exact application member. Equal-looking template recipes from another application cannot satisfy the gate.
- Original actual directory casing is retained in lookup construction; canonical `find298C80` is case-insensitive but preserves separators. The code appends the original `medlgout` basename and one backslash only if needed, matching the admitted original299BE0 segment. No suffix stripping is needed for this literal basename.
- Public acquisition cannot supply a row number. It resolves the real numeric canonical row via private retained table ownership, then calls private `retainScreen14D590`. Public demand can only use its privately stored row and fixed authenticated medlgout members. No public mutable material/texture pointer, floor setter, arbitrary material member, or texture callback is added.
- Table demand rechecks population/demand/collector phases and terminals, row range, positive numeric refs, exact current ScreenMT association, actual table-bound texture-owner pointer equality, and real nonnull material object. It guards demandActive during the private object call. Pending or foreign rows reject before mutation.
- The existing canonical populate14D370 path binds actual programs, texture table, and sealed construction context consistently. It already verifies context.application APPSTART and exact template backing against the table. The new leaf does not broaden that constructor population domain or substitute a fake context/program.
- Material-object demand rechecks its actual construction context is live, SAME expected texture table, exactly one constructed pass, exact retained source PSM bytes, exactly one nonnegative map token, and no animation object. It resolves only that actual private first-pass token through the retained texture table, rejects pending/null resource, and compares actual resource source bytes to the authenticated original medlgout PST before the sole write.
- Existing texture-table resolve298E60 already rejects active or terminal texture population. Its mutable output stays private and friend-gated to the canonical material object. Stage 12's new demand method is likewise private and only reached by that material consumer.

Bytes are authenticated at the public outline-owner boundary and compared again to the canonical objects before writing. Material name/token resolution remains that of the original canonical registries; decoded UI image indexes and stage10 recipe handles are not used as substitutes.

## Numeric duplicate references and release paths

- Each successfully acquired noncopyable outline owner invokes actual retainScreen14D590 once, even when thirteen outlines alias the same material row. That canonical method increments references08, rejects signed32 overflow and active/terminal registry phases, and stores the actual ScreenMT association. No deduplication collapses these source numeric references.
- This owner is neither copyable nor implicitly movable, preventing accidental duplicated releases. Shared-pointer copies pin manager lifetimes without incrementing the original numeric row refs, correctly distinguishing host lifetime from source reference ownership.
- Acquisition constructs the requested string before publishing its retained input pins. Once it stores material00/started, a later missing-name/retain failure is deliberately a started partial reference segment rather than an automatically retryable complete ScreenTexture. `retained_` remains false until actual numeric retention succeeds, so failure cleanup does not release an unacquired ref. Diagnostics distinguish one stored slot from one retained slot and preserve sourceEffectsCommitted.
- `release299AC0()` checks retained_, performs private canonical release14D500 exactly once, then clears retained_. Repeated release or later destructor becomes a no-op. An unexpected release invariant failure terminates, consistent with existing canonical reference owners rather than silently dropping a live ref.
- Actual release14D500 decrements references08; on final zero it invalidates collector borrowing and destroys the material/pass objects, whose canonical cleanup releases actual texture tokens/resources. It leaves source association30 intact, as the original does. Positive remaining duplicate refs preserve shared material ownership.
- Explicit release leaves archive/application/material/texture shared-pointer pins for inspection until native owner destruction. The destructor invokes the same release before member destruction, and the canonical material table retains its own texture/context owners while needed. Caller/session must not destroy/release these references before actual renderer retirement. This leaf contains no retirement receipt and does not override that requirement.

## Source-specific direct floor10 semantics

Static original words at `0x1F2508..2534` show first the original2DF8A0 call for outline object4C0, load of actual TextureResource+10, float10 constant, C.LE comparison, and direct conditional store at1F2530. The prior static2DF8A0 evidence shows material table382B38 -> first pass4C -> signed texture token1A -> texture table383400. Stage12 follows that identity chain.

- New private `demandTournamentOutline1F2530` requires constructed/live texture ownership and positive actual resource refs. It admits only supported host numeric mode and finite ordinary nonnegative existing demand.
- If existing demand is below10, it writes exact float10 and reports written=true. Equal10 or higher demand is preserved exactly and reports written=false. It never resets demand or alters mip/resident selection.
- It does not call104A00, whose projected-size conversion/capping/threshold arithmetic would yield a different value. This new source-specific primitive is therefore necessary rather than a generic use of the existing model-surface demand API.
- Ordinary zero and negative-zero are admitted; both floor to10 as the finite source comparison requires. Subnormal, negative, NaN/infinite, unsupported rounding/fast-math host modes fail before mutation. The source comparison can store10 for unsupported special values; rejecting those values is an explicit narrow native safety domain, not an all-input EE parity claim.
- The material object preflights source identity, registry resources, and supported numeric state before this sole write. No later fallible step follows the texture mutation. On success, diagnostics report one map visited/texture demanded and whether an actual demand write/source effect occurred. A no-op higher demand correctly reports success without claiming mutation.
- Failure does not reset shared demand or manufacture a texture/resource. Source24E670/24E4A0 resident-level selection and GPU residency remain separately owned and unimplemented by this leaf.

## Fail-closed domains and diagnostics

No arbitrary missing-PSM fallback, unique ATM construction, texture animation, extra pass/map, guessed image, host-clock advancement, profile mutation, or source-frame seal is admitted. The only new allowed operation is authored static medlgout first-map floor10 through its live canonical owners.

Acquisition may happen before actual material/texture population, mirroring the material-reference segment; demand then rejects until the SAME canonical tables publish the required material/pass/texture. Wrong expected texture owner at acquisition does not create a demand authority: exact pointer equality with the bound canonical owner is rechecked at demand. The caller remains responsible for correct actual population order and original directory observation.

Profile generation, source page phase, outline-instance uniqueness, complete ScreenTexture construction/flags/geometry, and renderer retirement are caller/session requirements explicitly left outside this leaf. An ordinal alone is not proof of a live page object, and started/retained is not full ScreenTexture or session readiness. No public success boolean opens Tournament routing.

The private helper declarations and friend relationships match existing class ownership. Diagnostic enum values, pass/map counters, texture diagnostics, construction live checks, source byte accessors, template/app accessors, row/reference fields, and existing find/retain/release signatures all exist with the used types. The new memcmp include is supplied. No prior `.c_str()`-type mismatch or missing-origin whitelist issue is introduced in this patch by inspection.

## Integration requirements

Root must add `src/assets/original_ui_screen_material_reference50kg.cpp` to both actual native compilation lists:

1. `platform/linux/native/CMakeLists.txt`, using `${RR_ROOT}/src/assets/original_ui_screen_material_reference50kg.cpp`.
2. `tools/prepare_interactive_build50dv.py`, using `src/assets/original_ui_screen_material_reference50kg.cpp`.

The altered canonical material-object/table and texture-resource CPPs are already registered in both lists. No separate header compilation entry is needed.

This provides an actual callable canonical registry consumer for the outline portion only. It still needs authentic application source/context population joined to Tournament, the remaining static/missing-PSM/unique-ATM ScreenTexture branches, inherited141C30 widget traversal, complete source update ordering, full renderer submission/retirement, and session ownership. The stage10 request lease is still not itself applied demand; connecting its thirteen outline calls to these genuine owners is future root/session integration. The full update and frontend should remain closed until those boundaries are established.

No definite blocker found, but no build/test/runtime success is claimed.
