# Distinct native Pool backing and Entity constructor prefix

## Intended implemented boundary

This packet builds on the separately reviewed Pool scalar-allocation entry and adds a distinctly owned, success-only native0x1D0 Pool object plus the supported Pool/scene/Entity constructor prefix. Its endpoint is **before original289420**, after the Entity constructor returns. The outer Pool and scene constructor frames remain outstanding. No scene matrices, type-registry lookup, full Pool constructor return, resource load, Water68 element publication, position transform or rendering/GPU readiness is established.

The original scalar allocator stays stopped before its3059EC arena read. Native backing success and the adapted caller are separate receipts, not original malloc/new return, original heap metadata, NULL/error/exception behavior or source deallocation. Original Config/aggregate/provider30492C, previous array allocation/backing, formatter2B9D0C and signature2B9A80 remain unchanged. The new Pool is not the pointer-array allocation and not the Water provider's existing Entity.

Immediate predecessor: `/tmp/racer-water-pool-allocation-20261105-0527`, final manifest c2826d721376250eef1841a20c1816f6782ee3904f5ba6112a0f5513d1efaba8, patch d3a8e7ec1e07a05a6a3e360eac85b7d1abb9a05da422c5d7e7eba178b8e3697f. Published main remains e566993fc25f628ce5b4aa5def424c6003f53b75. The entire successor chain is unpublished. Inherited process-globals changes still require a narrow reviewed rebase if the unpublished vectors/section checkpoint applies first, preserving its vector-owner changes.

## Distinct owners and native admission

Two one-shot APIs keep the boundaries distinct: `allocateWaterPoolBackingNativeOnly(nativeSuccessBackingOnly)` prepares custody and the adapted caller through2BAE30; `constructWaterPoolEntityNativeOnly(nativeSuccessBackingOnly)` admits the constructor prefix and stops before289420. Repeated attempts expose retained historical results without recapture, reallocation or constructor replay, including after native retirement.

The Pool has a stable, noncopyable native owner with its own exact464-byte payload, semantic whole-object role and canonical typed Entity facet at semantic offset8. Native C++ layout is not a packed EE object layout. Native host pointers are never converted into guessed guest pointer values. The existing poolPointers array/view/epoch supplies the current caller-slot context, but no slot is published at this boundary; every pointer slot remains unknown under its existing preflight.

Fresh payload bytes are unknown. The typed facet's default reference0/unbound/null links are native bookkeeping only, not original Pool byte values. All original byte/bit knowledge must come from reached stores. Original normalized chunk0x1E0 is retained by the allocator-entry history; native object payload size is the original request0x1D0, not that chunk size. Sidecar/authority metadata storage is additional native storage, not part of the guest object request.

Admission uses the same live Owner, provider, Config, Application/load-Application, World invocation and thread, valid current array custody, no borrows, no terminal/revoked/withdrawn domain and no attachment lock. A fresh native-only reentrancy check is independent of historical scalar30570C and formatter captures. Allocation/preparation refusal is terminal for this one-shot native occurrence and retains all reached prior state. It does not take an original source NULL/handler/throw branch or assign a fake source return.

Before any genuine reference acquisition, all fallible native owner/payload/authority capacity preparation and count headroom checks are completed. The same Application authority reserves its metadata, validates exact future Entity identity/key/session and provides its real objectCount423F60. The uninterrupted reference/Entity/count/enrollment segment must have no fallible gap that could leave an untracked reference or count obligation. Native authority enrollment is bookkeeping for this exact counted Entity, not an extra source count or source call.

## Adapted caller and exact constructor stores

Success-only native backing gives a separate adapted caller the nonnull whole Pool role at2B9D14. The original BEQ is not taken, but its delay2B9D18 still supplies a2=1.2B9D1C supplies a1=1. JAL2B9D20 with POR delay2B9D24 passes the same whole role to2BAE30 and retains future return2B9D28. No allocator or malloc frame return is retroactively claimed.

The represented constructor prefix is:

-2BAE30 creates32 bytes and saves ra/s0. JAL2BAE3C and delay2BAE40 retain the same whole object while entering2893A0.
-2893A0 creates another32-byte frame and saves ra/whole argument. SH2893BC establishes whole+00=0; SB2893D4 establishes whole+02=FF. Separate SB2893E8/2893FC/289410 clear only bits0/1/2 of whole+03. Upper5 bits remain unknown.
-JAL289418 with ADDIU delay28941C passes whole+8 to141300. That constructor creates48 bytes and saves ra/s1/s0.141314 with delay141318 invokes1048F0 on that distinct Entity.
-SW1048F8 writes base dispatch3CC980 to Entity+8, whole+10. **SD104900 first establishes reference1 at Entity+0, whole+08.** Only now is a reference-retirement obligation acquired.104904/104908 returns to14131C.
-SW141324 replaces whole+10 dispatch with3CD240. SB14132C writes categoryFF at whole+30. SW141330/34/38/3C zero links at whole+18/+1C/+20/+24. Four distinct flag stores14135C/37C/398/3A8 establish whole+31 low4=3 while preserving unknown upper4. SW1413AC/B0 zero whole+28/+2C mask and descendants.
-LW1413B4, ADDIU1413B8 and **SW1413C0** increment the same actual Application423F60 once. Only this store acquires its count-retirement obligation. The exact facet is enrolled in the same Application authority with its canonical address, current session and key. The native registry's serial/latch effects are retained, not undone during cancellation.
-1413C4/C8/CC restore Entity saves; JR1413D0 plus release delay1413D4 returns to289420. Stop before executing289420. The scene and Pool frames have not returned.

The implementation writes the represented Entity fields directly in source order after admission, following the existing typed component contract without calling its bundled initializer. Typed bookkeeping zeros do not make opaque source upper flag bits known. The source-known masks remain whole+03 mask07/value00 and whole+31 mask0F/value03. Whole+04..07 and every other unreached lane remain unknown. No raw indeterminate C++ byte is read to emulate a source read/modify/write.

## Balanced count floor and cancellation

The local Pool validator must not call the combined World count floor recursively. `sourceWaterProviderCountedNative` deliberately reports **provider contribution plus independently validated Pool contribution**, preserving separate acquisition/retirement facts. The existing combined World floor then includes the child exactly once. A positive global count alone is not enough to justify either decrement.

Before mutation, withdrawal validates exact provider/Pool identities, reference1, authority membership, detached topology, zero masks/descendants, absent native flags/effect borrowers, unknown array slots and all combined count contributions. Unexpected references, attachments or aliases refuse with custody retained.

The existing fallible Config-abort step remains before either count changes. If it refuses, both objects and obligations stay intact. World400/light-pin withdrawal remains explicit. The Pool is retired before the provider: revoke its native views, retire its exact authority entry, decrement only its acquired reference and Application contribution once, then release its actual native payload/role. Backing-only state has no count/reference to decrement. The provider's own authority/reference/count and array payloads are then retired by the existing flow. No original104970/1413E0/scene/Pool destructor, guest free or source NULL store is asserted. Historical one-shot/PC/ownership receipts survive released storage. Future loader/resource/slot aliases require additional cleanup support before admission; this detached base-only slice does not authorize them.

## Evidence, versioning and review

Focused custody report SHA2567322dbe151e41216d05473e79af3e141d708dc43830774a7660a544cca51f6ef and362 exact original instruction words are preserved under evidence/focused. Its source identity records select the actual latest inherited files, not an arbitrary same-name copy. Five missing dependency files were fetched read-only at exact published e566993, with Git blob/SHA256 and verified URLs retained in PUBLISHED-IDENTITIES.json and the focused fetch receipt. These include the component helper, entity authority, combined count-floor and teardown-order sources; they are not source-pristine substitutions.

Original ELF SHA256c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Source/ELF inspection, identity hashing and isolated strict text-patch reconstruction are permitted validation. No compile, build, syntax/type check, tests, probes, implementation/helper/model execution, emulation, runtime or publication. Independent review must bind to final frozen source, patch, identities and evidence; acceptance metadata may later change without altering reviewed source.
