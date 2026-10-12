# Course central integration: independent source review

2026-10-12 03:47 UTC. Reviewed the staged integration in `rr-recovery-git` against base `bfe1552d0fddbae5a1a0da7900e73782926992e4`, the reconstructed candidate inventory, and the preceding source reviews. This report supersedes the earlier reports' disabled-gate and private-selector-ownership descriptions.

## Result and scope

No remaining definite source integration, state-lifetime or obvious declaration/call-site mismatch was found in the bounded muted mode0, one-human, Rider19 Confirm → fixed Course6/1 → fresh Rider19 Back route. The two paired admission constants are now enabled; their actual owner, archive, profile, selection and lifecycle predicates remain in place. This is source-review acceptance only. The integration has not been compiled or exercised by this reviewer, and full UI/gameplay acceptance remains unexecuted.

Only source/text reads, diffs and identity hashing were used. No build, test, syntax/compiler probe, sanitizer, project script, renderer or gameplay execution occurred. The reviewer changed no repository or candidate source.

## Findings

1. **Missing-archive startup regression closed.** The first integration made Course loading mandatory for muted Rider-cache startup. The final `original_ui_main_entry50kg.h` checks the exact compiled Course catalog path and leaves `cold.course` null when absent. Main/Options/Rider Back remain available; genuine `courseInputsReady()` prevents Confirm before profile or voice mutation. Supplied malformed bytes still go through normal authentication and fail; filesystem errors propagate. There is no alternate search path or readiness fallback.
2. **Confirm, entry and Back joins remain bounded.** Cold destination preparation precedes the generation-checked private profile commit. Confirm retains actual Rider19 widget53/model-transition preflight and the accepted route/fade/voice/selection/effect sequence. Course entry consumes the post-Confirm generation before its planet write. Back constructs real fresh Rider owners using Course predecessor resources and retains the remembered-profile transaction. Renderer submission/completion and bank/light-owner handover precede outgoing resource retirement.
3. **Clock and selector lifetime is explicit.** Runtime and menu Application share one `HostMonotonicClock50ID` owner and epoch. The stateless voice selector borrows Application's `SharedRandomOwner50ID`; the same raw RNG, single draw, fixed-domain integer choice and already-completed mutation prefixes remain. Menu-to-race launch is still unavailable, so cross-scope application/race selector continuity is not claimed.
4. **Live GC behavior is preserved by this join.** Existing Dax, contact, GC animation and lap calls retain their one cold-race `sessionRandom` owner and typed subobject borrowing. No Course integration changes were found in the shared RNG header, GC animation fixture, GC lighting owner or vehicle renderer/shader. The runtime diff preserves the GC fog and lighting policy branches. Previously documented GC failure staging and cold-race reset policy remain unchanged.
5. **Build/API joins are coherent on reading.** Linux CMake, the native build recipe and Windows source manifest each add the same 16 translation units. Runtime/Application constructor and clock publication changes match; Course session, collector, resources, material and scene calls match their declared APIs. Windows provenance records the current recipe hash and accurately identifies the base revision plus this added integration. This does not establish compile/link success.

## Limits that remain explicit

Race launch, other Course selections, career/save/card routes and active popup rendering remain unavailable. Initial copied widget packets can retain prior/default palette until the following frame; aperture opacity does not prove concealment. Native cold preparation contributes real elapsed time before voice sampling. Native global Course light rows, background decoding/sampling and material/resource policies do not prove original per-model cache membership, PS2 allocation history or exact GS raster fidelity. These are bounded native-policy limitations, not hidden prerequisites claimed complete.

## Final source identities

All paths below are repository-relative. Read at 03:47 UTC; SHA-256:

| Path | SHA-256 |
|---|---|
| platform/linux/native/original_ui_frontend50kg.cpp | 06252fd175d1e9b9d92c99471f7ce3c5688df8e8db749cb281aa2544063b32d6 |
| platform/linux/native/original_ui_frontend50kg.h | 3b84edce2aa7076dbfc7d5c3ca3c02b12f11034d9dcd78da5a2fc369e72ae337 |
| platform/linux/native/original_ui_rider_session50kg.cpp | 93db60b6e28870af9d2e54e894e808b07d42f51bc92eecdf0c705790ae2aa8de |
| platform/linux/native/original_ui_main_session50kg.h | 3ad2a17957dfb52d8da26f6e7be05fc7cfe42d40fd7d32af4b156fe56b0633fd |
| src/game/original_ui_rider_voice_choice50kg.h | 226345eceeb203234039da88fca747e87e50e3bd852ad7169edc8a8f285d373c |
| src/game/original_ui_rider_voice_choice50kg.cpp | 7985815fcf15737c0c879f85ef056c72dc96a293dd1900b654517ae916611220 |
| tests/native/original_ui_main_entry50kg.h | 7183336a7adf81ef68423e846383df97667865c0277e85926db17702d006f210 |
| tests/native/terrain_interactive_drive50du.cpp | 730aadde705ee80168281674cc0380c1553834032be49995902b1db545efea5f |
| platform/linux/native/CMakeLists.txt | 10cc0379b0aa1f35d0b4ae47f466dbb4ebaf58cf19bbf23a0f1ef1e8aa108a64 |
| tools/prepare_interactive_build50dv.py | 3d3e099239224e5d25c6d543bb53589e82de1189ee6d79e6d264026a3224877a |
| platform/windows/native/interactive50du.sources.json | 5cdb8f5fcf7ab45e48adc4a99e7e4c2f54424a5d26813345eab317ff2664e32e |

The 53-file candidate inventory `COMPLETE-SOURCE-INVENTORY.json` has SHA-256 `77e24d80494404a855b6385457c4627a3b06d158b99b1d914f1be6ca7bf5456f`. The current pins above cover central integration edits that supersede candidate identities.
