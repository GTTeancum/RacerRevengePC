# Water startup alpha and complete current vector copies

## Exact predecessor and scope

Isolated successor to accepted unpublished ShaderName packet `/tmp/racer-water-shader-20261105-0120`: patch `44a53b333a7677640b4a77bafc94cc6f64d7fe0e69010415ece5baa426c382d6`, final manifest `f8566be41854afdaac27150534e15f3037cdb6b3cf38164ed8879da15b976d39`. Its exact unpublished flags/color/scalar/opacity/getter/section chain remains required. Published main remains `e566993fc25f628ce5b4aa5def424c6003f53b75`; publication is paused. No checkout/predecessor edits.

This packet joins genuine explicit startup producers for the previously unowned alpha and vector-tail words, then continues the actual original Water caller through remaining color copies and two full current vector copies. It stops at the SECOND157D00 entry, before the Wake Froth section-selection body. It does not reuse the General Information selection as a new result.

No compile, syntax/type check, build, tests, probes, target/helper/model execution, emulation or runtime. Static source/ELF inspection, identity hashes and strict text-patch reconstruction only. No GPU, renderer, gameplay or loader readiness. Success-only native allocation backing still does not establish original EE allocation addresses, heap/free-list, failure, errno or availability. Inherited process-globals Water additions still require a narrow reviewed rebase preserving pending vector-owner changes if the paused vectors/section checkpoint integrates first.

## Genuine startup publication, not zero substitution

Original ELF SLUS_202.68 SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Static bytes establish:

- Entry1000A4 calls239C40, whose239C54 calls2EADE0.
-2EADE0 supplies constructor-table interval[3CC450,3CC650) and jumps2E91F0. That loop loads each entry at2E9210 and calls it at2E9214.
- Table3CC5F4 contains3CB790. This entry is inside the actual range; the initializer runs before ordinary Application construction.
-3CB790 has a straight-line sequence through return3CB8B0 and final delay-slot store3CB8B4.

The six newly joined words are:

-443EAC = raw INTEGER32, SW3CB84C, from ADDIU a0,zero,32 at3CB844.
-443EBC = raw INTEGER32, SW3CB870, same unchanged a0.
-443EC4 = raw0, SW3CB7D0.
-443EC8 = raw0x448AE000, SW3CB880, from LUI/ORI3CB874/878.
-443ECC = raw0xC0000000, SW3CB88C, from LUI3CB884.
-443EDC = raw0x3F000000, SW3CB8B4, from LUI3CB8A4. This is the initializer return delay-slot store.

Although entry BSS clearing includes these addresses, its zeros are superseded by this explicit initializer. Neither object alpha0 nor RGB results nor file-backed/BSS guesses supply process alpha. Both process alphas are32, not float32,255 or0.

## Same-Application lifetime and current mutable lanes

The existing native Application is the already accepted fresh process domain. Its SourceWaterProcessScalars now includes six additional lanes24..29 for the words above, initialized once with exact source address/store-PC/value metadata and native publication1. This is a bounded projection of source startup publications, not a claim the host executed the guest constructor, observed retained retail process memory, or performed a new source allocation.

No reset/replay entry point is added. World/provider creation, native retirement, retries and section changes cannot restore these words. The same App native thread, constructed/released lifetime and whole-owner unknown poison govern reads and any future joined writes. A later legitimately joined writer changes the same word/publication; a future unsupported alias must poison this owner, never recreate its startup values.

The existing mutable words are deliberately NOT duplicated or reset:

- Word0 is current443EC0, last joined by original constructor2B850C.
- Word1 is current443ED0, last joined by constructor2B8528.
- Word5 is current443ED4, written by loader2B9414.
- Word6 is current443ED8, written by loader2B9430 after original SUB.S.

Although3CB790 also initializes those words and RGB, this packet projects only the six newly needed startup words. Existing scalar ownership/publications stay authoritative. Original constructor and config writes continue overwriting their same current lanes. No frozen full startup vector cache is created.

Each full LQ admission requires all four current words and publications on this SAME owner, then takes one snapshot without a fallible partial output.443EC0 vector maps to lanes[0,26,27,28];443ED0 vector maps to[1,5,6,29]. An unknown/poisoned/missing word refuses the entire source LQ before a register or object copy is claimed. Reading a known word does not require capacity for another publication; counters at their maximum remain readable. All snapshots keep per-word historical versions, allowing later legitimate overwrites without falsely claiming current values after poisoning.

## Actual continuation and destination ownership

The preceding packet already copied Env R/G/B from current443EA0/A4/A8. This packet adds these individual live loads/stores in order:

- LW2B9768 reads443EAC; SW2B976C writes Water+2EC.
- LW2B9774 reads443EB0; SW2B9778 writes Water+2F0.
- LW2B9780 reads443EB4; SW2B9784 writes Water+2F4.
- LW2B978C reads443EB8; SW2B9790 writes Water+2F8.
- LW2B9798 reads443EBC; SW2B979C writes Water+2FC.

The same SourceWaterProviderMembers::integerArrays lanes initialized by2B8590 are updated. Separate integerConstructorZeros retain exact constructor history. Optional zero and32 values remain known source words, not absence. Each refusal at a live read leaves previous writes intact.

Then LQ2B97A4 takes all16 current bytes at443EC0 and SQ2B97A8 writes the same constructor-owned Water+1F0 vector. LQ2B97B0 takes all16 current bytes at443ED0. JAL2B97B4 issues the second157D00 call, and its delay-slot SQ2B97B8 writes Water+200 BEFORE the callee body. The existing object vectors[1]/[2] are overwritten, with all original constructor vector stores separately preserved in constructorVectors. Neither vector is manufactured from a scalar first-word receipt alone.

The resulting frontier is157D00 entry. The same Config and Wake Froth literal were prepared by the original caller. The call-issued history records dispatch/delay completion only; General Information remains selected in the Config. No teardown/free, new section allocation, selected row set, missing/present result, new getters, source loader return or completion is fabricated. Any later section extension must implement the genuine teardown and fresh lookup under the still-explicit allocator limits.

## Audit boundaries and validation

Frozen broad static low16/address-candidate scans find only startup direct alpha writes. Other matched alpha accesses are reads at2B8B20/2B9768/2BA334 and2B8B50/2B9798/2BA364. No exact alpha-address formation or aligned raw pointer443EAC/443EBC was found. RGB escaped pointers target only the separately represented single-word getters. EC4/EC8/ECC/EDC likewise have only startup direct writers in this audit; global quadword operations found here are reads, not writers. Alternate-loader first-word/opacity writers remain outside this represented continuation and cannot silently mutate or reset the owner.

This is a bounded direct/address-candidate audit, NOT universal whole-program indirect-alias proof. Arbitrary unrepresented guest execution does not inherit known currentness. Shared poisoning, private custody and same-owner/thread/lifetime checks remain required.

Frozen evidence includes source3CB790, startup dispatch/table bytes, caller/writers/consumers, ELF metadata/hash and candidate scans. The packet preserves exact predecessor hashes, source preimages, inherited dependencies and SHA256/Git-blob identities. Independent source review and exact text reconstruction do not substitute for unperformed build/runtime verification.
