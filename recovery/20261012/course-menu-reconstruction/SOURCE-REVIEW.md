# Independent Course source review

2026-10-12 02:42 UTC. Reviewed reconstructed candidate `/workspace/shared/course-recovery-20261012` against retained original word evidence and native owners in `/workspace/shared/rr-recovery-git`. Main HEAD observed at the final read: `dbdea5209c603f015463919d2b61dbc92623a248`.

## Result

No remaining definite source integration/state-lifetime blocker found in the bounded muted, mode0, one-human, Rider19 → Course6/1 → fresh Rider19 Back route after the fixes below. This is a static review, not runtime admission. Both candidate admission constants remain false. No build, test, syntax/compiler probe, sanitizer, renderer, gameplay or project script was run. No repository or shared candidate source was edited by this reviewer.

The earlier interrupted review is superseded for the findings covered here; its unresolved popup wording must not be treated as the current conclusion.

## Closed findings

1. Reconstructed hologram state/reset drift was corrected: models start hidden, the root includes source bit08, and reset changes only the root pose rather than rebuilding the full hierarchy. Original2A8B74..84 and2A8C90..D8 support the correction. Public apply stages storage plus shared motion/IK and commits only on success, repairing copied playback descriptor pointers.
2. The collector's image-bank comparison now compares the borrowed pointer with `images().get()`. Bank publication is unavailable after incomplete preparation, and the final GPU bank checks preserve texture identities, publication and sampler policy.
3. Popup68..76 are actual hidden manager-owned templates, not supposed reparented children. Source2037D0 copies them into popup-owned arrays. The candidate now retains genuine clone/font/image ownership, actual disabled bits, hidden templates and rejection of control mutations to those templates. The session and compositor check this real inactive owner. This closes the fresh no-card inactive path only, not active popup rendering.
4. Voice choice initially used nearest host int-to-float arithmetic, which could manufacture choice5 and a terminal failure. The final bounded implementation discards excess low bits of the low31-bit draw, then evaluates `(uint64(q)*5)>>31`. For draw0..2^31−1, at most seven bits are discarded, the product is below2^34 and the choice is0..4. Power-of-two scaling is exact; positive toward-zero binary32 reduction cannot cross an integer floor because integers0..5 are representable. No clamp, modulo, extra draw or global FP-mode mutation is used. This is the fixed-domain native formula, not a general hardware ALU parity claim.

## Transaction and ownership review

- The retained profile is initialized once at application startup. Course uses authentic control indices and constructor/reset values, including the nonsequential course order. It does not fabricate a page-local profile.
- Confirm has a separate typed proposal and rechecks generation, source session, serial, current lifecycle, selected control, real widget53 model attachment/selection/visibility and real animation-transition preflight. The preflight copies playback and graph without clock/RNG mutation.
- Cold destination owner/bank/GPU preparation precedes the private profile commit. Rejected preparation/admission/commit leaves profile writes unperformed. The accepted three Confirm writes are followed by the actual route request, fade, voice seed/draw/choice, muted voice output, actual `_sel`, then muted effect1. No Back bypass or fabricated readiness is applied; closing waits for real character readiness and aperture endpoints.
- Voice lazy guards/cache and the raw RNG word are application-owned. Startup, materials and voice retain the same host clock epoch. Seed cache zero retains the source later-reseed behavior. A clock failure retains the initialized lazy guard; later failures preserve the already completed seed/draw prefix. This is not a blanket rollback after accepted source effects.
- Course entry checks the post-Confirm generation, then performs the actual planet-profile commit. Initial requestedPage90 is safe because the transition owner publishes current/requested before emitting initial notifications.
- Back cold-builds genuine fresh Rider owners using actual Course predecessor materials. Its exact four remembered-profile writes are generation-checked. Source exit/render endpoint completion and actual submitted-work completion precede renderer bank rebinding, light-manager handover and outgoing resource release.
- Callback contexts remain stable manager objects. Application clock/shared noise owners survive page destruction. Course bank ownership is immutable across publication, and each draw verifies the genuine live bank/publication.

## Explicit limits to retain

Native resource allocation/presentation policies do not prove historical PS2 allocation or GS raster fidelity. Current two-light Course rows are a named native policy, not per-model14A0E8 cache membership. Background sampling/alpha and material-pass registration policies stay explicit. Ordinary widget copies precede page palette changes, so initial copied packets can retain prior/default palette until the next frame; aperture alpha does not establish concealment. Native cold preparation contributes actual elapsed wall time before accepted voice sampling. Race launch, other Course selections, career/save/card paths and active popup rendering remain unavailable.

## Original arithmetic sources

Source1EA6E0 contains `46000024` (CVT.W.S). The final signed conversion truncates independently of the host rounding mode. Sony's EE instruction manual p356 defines this conversion; the EE user's manual p164 makes FCR31 low bits hardware-fixed01. The latter also warns about generic low-bit arithmetic differences, which are not claimed resolved here.

- https://docs.alexrp.com/mips/ee_insns.pdf#page=356
- https://docs.alexrp.com/mips/ee.pdf#page=164

## Final hash pins

Paths below are relative to the candidate root. Hashing was a source identity read only.

| Source | SHA-256 |
|---|---|
| core/platform/linux/native/original_ui_rider_profile50kg.cpp | 9b080f01f1720de08b5849518f5076000b3f2164351a135a6f7526e92bef2b0c |
| core/src/game/original_ui_course_selection50kg.cpp | 2575005ab875f6bfae49b066e90228e99d031f61b2e8cbb9188d3c4149451527 |
| session/platform/linux/native/original_ui_course_session50kg.cpp | 77fd75b4023a8486541c9f9cb88bf4467f4316833b7d5fea82b83b41cde8d217 |
| aperture/src/game/original_ui_aperture_owner50kg.cpp | b1575d26b347031b363316ab5fa69a587785435c3a18c9a3a001d567553425c3 |
| frontend/platform/linux/native/original_ui_frontend50kg.cpp | c24f1d7e51a3ef33ef377bb156f085f09f619fa784c3116daba723489805a05c |
| frontend/platform/linux/native/original_ui_frontend50kg.h | 5e1b35a250a1e49702ed7f8c8144a71eefdc002ec709edffa055a98aeff59ae4 |
| frontend/RUNTIME-COURSE-THREADING.patch | 3d9c1bcf5e2f2b754b6c5897fc160b434ed126b2410cd75e9720d0754d9e9c72 |
| rider/platform/linux/native/original_ui_rider_session50kg.cpp | 91a82727cc1f24609eba2d47a669b383d06929d2c9fb7238a20f46645d4836a9 |
| rider/src/game/original_ui_rider_voice_choice50kg.cpp | 18e2fc7d853fc7dcfba655fdae79a7b008bf96a94f2b06ccebbb692851d5cd6c |
| widgets/src/game/original_ui_course_widgets50kg.cpp | 6ad104c2180132d26b28b95e54e610fcc9b651cd694f12b85de05e4b4b41c874 |
| widgets/src/game/original_ui_course_widget_controller50kg.cpp | adc4d7dc54bf13e44774a563aa55ab7ce9e4d89d98b36fff7645a2a82253d677 |
| widgets/platform/linux/native/original_ui_course_draw_resources50kg.cpp | 3169155a9ef24ac7e3aeaf2221e5a1ccc5a2f6f621b4b8060b1ded98b47bf0ef |
| page/src/game/original_ui_course_page_draw50kg.cpp | b0fd28579c69b8f0faf31df560b7891d114620ae1b3882c81f3b198e8a2ac5c5 |
| scene/platform/linux/native/original_ui_course_draw_contribution50kg.cpp | 2842e212484117cfd3f9338ff21efae2a27bb1db8ea3f568ca836ea51632231c |
| scene/src/game/original_ui_course_hologram_scene50kg.cpp | 5586cb111cc953e842c76bde6c24fc16e2dc019c9317495c0837a3939d86f322 |

The complete frozen scene22 inventory is `scene/SHA256SUMS.txt`, hash `df319a3484b77c098650ab99b07e333545937d0d3c04f68f0763db75ca27b835`. The final Rider11 inventory is `rider/NEW-RECONSTRUCTION-SHA256.txt`, hash `5c58960d596567455ab8de9d093a316025d7dd6ea08a01c430574224e5d90bdb`.
