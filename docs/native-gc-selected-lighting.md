# Smaller visible improvement: selected GC lights in the existing native shader

## Recommendation

Keep the genuine GC registry/visibility candidate independent, then add an explicitly named `nativeGcSelectedLights` rendering policy. Replace only the currently hard-coded scalar light in the existing racer shader with RGB illumination from the current selected GC matrices. This is an honest, bounded native rendering improvement. It does not need original alpha/fog/clipping/specular parity before it can be useful, because it does not claim to execute the original material program or replace its entire composition.

The reviewed policy now defaults on for an admitted fresh GC race, per the user's 2026-10-12 direction. RR_NATIVE_GC_SELECTED_LIGHTS=0 is an explicit diagnostic opt-out. Main/checkpoint sessions do not implicitly enable it. The first integrated Linux build and bounded original-AI observation passed on 2026-10-12; see the validation receipt below. This does not establish Windows behavior or full rendering parity.

## What this policy means

The source-owned input is the effective `LightMatrices` selected from authenticated GC registration plus the real racer's7F0 membership under the separately reviewed native per-publication reevaluation policy. The native output is continuous per-fragment Lambert lighting using the renderer's existing current world-normal stream.

For each fragment under the existing host-lit opaque-textured admission:

    illumination.rgb = selected.diffuse[3].rgb / 128
    for slot in 0..2:
        illumination.rgb += selected.diffuse[slot].rgb / 128
                            * max(dot(hostWorldNormal, selected.directionColumn[slot]),0)
    existingBaseRgb *= illumination.rgb

Use the selected columns exactly, with their constructed sign/orientation. Do not replace their direction with a camera vector. For GC they are the negative constructed source look vector, with one actual directional slot and two zero slots. Preserve the existing world normal normalization and geometric fallback as explicitly native policy; do not call this StripPreamble normal math. Use source RGB/128 as a documented native coefficient convention corresponding to source byte MODULATE neutral128, with no screenshot-tuned exposure constant. A later original source-generated-color path must remain separate.

For unchanged GC, ambient coefficient is(0.3125,0.3125,0.3515625); directional coefficient is(0.6640625,0.6640625,0.625). A surface facing directly toward the source light reaches(0.9765625,0.9765625,0.9765625); away-facing/perpendicular surfaces retain the cooler actual ambient. The source light direction is toward positiveY/negativeZ, unlike the current fixed positiveZ direction. These are analytic coefficient expectations, not rendered results.

## Exact smallest integration

1. Keep the source owner lifetime implemented in original_gc_gameplay_lighting_candidate.h/.cpp. Wire its real initial attach and immediate postgraphics updates, then select actual current matrices under each racer's draw lease. Do not reset/reuse the UI light manager lifetime. A failed owner/update/select stops publication of a requested-policy frame instead of silently claiming the old generic light is current source light.
2. Add an optional, typed native-light packet to `VehicleWindowFrame50DS` for the player and to each `VehicleWindowOpponent50DS`, containing three component selections, actual roster slot/graphics/draw publication identities, and the explicit rendering policy. Keep values copied, not borrowed owner/lease pointers. The synchronous frame preflight copies and validates them before any GL call, alongside each corresponding real geometry batch. Both player and opponents need independent packets even though this particular GC result is numerically equal.
3. Add shader uniforms to `VehicleGlShaderUniforms50CZ`/`VehicleGlShader50CZ`: explicit native policy selector, RGB ambient, three RGB directional colors and three direction vectors. Existing uniform4f calls can upload paddedXYZ vectors without new GL capability requirements. Resolve and validate uniform locations during normal cold shader creation. Uniform setter/admission failures abort the existing unpublished frame transaction.
4. In `vehicle_shader_gl50cz.cpp`, replace lines75..78 only when the explicit native policy is selected. Leave the existing normal stream/fallback and texture/vertex-color base calculation intact. The current fixed hemisphere and fixed(0.35,0.85,0.30) light must not be multiplied in again. Keep current shader behavior for policy-off draws, with no claim of authentic source shading there.
5. Bind the packet for the actual player component in `vehicle_window_gl50ds.cpp::bindMaterial`, and for the actual opponent/component in `drawOpaqueOpponent` before its real span draw. `applyPodMaterial` must only opt in when current policy already allows host lighting, blend is opaque, a real texture is bound, and the material is neither unlit nor sourcePrelit. All effect branches keep native policy off. A requested-policy opaque host-lit untextured span rejects in preflight; policy-off diagnostic rendering remains unchanged. `begin`, `resetPodState`, transparent pass, cable, track, dynamic-world, sky, menu and UI transitions must explicitly disable/reset the policy to prevent GL state leakage.
6. Preserve current textures, source vertex colors, native alpha/blend/depth, UV animation, shader interpolation and sampler behavior. The policy can apply to the existing opaque base rendering of SpecularMapDual materials as a native base-light improvement, while their missing original specular/second-pass behavior remains unsupported and documented. It does not certify those programs. No source-program-name gate is necessary to replace a pre-existing host lighting decision, but actual material policy identity and its real texture binding must remain validated.

This route does not need to modify `buildVehicleDraw50CM`, regenerate vertex RGB, consume original raw normal quantization in the shader, alter the mesh ABI, implement original model1B3/1F4 overrides, or change alpha/fog/clipping. Those remain required if claiming original material-program parity, not prerequisites for this explicitly native host-light policy.

## Scope of the claim

The bounded integrated observation supports this claim for the captured GC slice: “GC racers now use the level's actual registered ambient and directional light colors/direction, selected with their real visibility ownership, in the native renderer.”

Do not claim full original DiffuseCompressed rendering, original cache behavior, original normal arithmetic/quantization, generated RGBA/alpha override, PS2 rasterization, original fog, specular or TA lighting. Do not call the packet `sourcePrelit`: the shader is evaluating native lighting and retains existing material composition.

Only GC is admitted. TA's26-light spatial selection remains unsupported. Untextured neutral-gray diagnostics are not promoted to authored materials. Other tracks require their own authenticated registry/selection support; the GC packet must not become a universal per-level constant.

## Bounded acceptance for root

Source/owner acceptance:
- Exact GC SCB only; unknown/TA source rejected.
- Actual separate4C0 memberships, correct second-method registration, real initialC30 and cached694 radius; consecutive graphics updates and transient draw identity checked.
- Wrong racer/component, stale sequence, invalidated session, failed update and lease mismatch cannot publish a requested native-light frame.
- Per-racer packets are copied alongside that racer's actual batch, never from the preceding bank slot.

Native math/shader acceptance:
- Policy-off preserves old behavior exactly.
- Policy-on removes fixed hemisphere and fixed direction completely.
- A white opaque textured sample with an away/perpendicular normal gets GC ambient coefficients; a light-facing sample gets ambient+directional coefficients above. No additional exposure/tint constant enters.
- Actual current animated normals continue to follow the real per-node palette. Inverse-transpose/normalization remains explicit native behavior; degenerate-normal fallback is reported.
- Unlit/sourcePrelit, transparent/additive effects, cables, world, sky and UI paths are unaffected; policy-off untextured diagnostics are unchanged, while requested-policy lit opaque untextured spans reject. No stale uniform leakage.
- Missing/malformed requested packet fails before private framebuffer publication. Earlier accepted image remains intact under the renderer's existing transaction.

Visual acceptance, later and only by root:
- Same authenticated GC racer/position/camera/frame before/after, clearly labeled native policy.
- Check both player and at least one real opponent, chariot and both engines; no geometry/UV/alpha regression.
- Verify cooler ambient and authored sun orientation rather than claiming dramatic color from a level whose full-sun RGB is nearly neutral.
- Report remaining material, fog, specular and TA limitations with the result. The integrated observation captured real gameplay frames, but no matched-position policy-off/policy-on comparison was run; visual improvement or full parity is not certified.


## Integrated validation: 2026-10-12 UTC

- Tested source: `0f1c905a8100ec0701dc0cc2326ff55edc2902c0`.
- Linux native client build: 282 C++ translation units (86 compiled, 196 reused), exit 0, completed 01:25:46 UTC.
- Executable SHA-256: `04dd578dc35a144f2aeb6cd7663a0b08d7a8a1e3ff94bbde6367874842f96e7c`.
- One 30-second wall-ready GC observation with this option enabled, completed 01:26:22 UTC. Original in-game AI controlled all eight racers, including camera slot 0. All eight dispatcher/tick counters advanced; all observed host-driver, controller-sample and keyboard-packet counters were zero.
- Recorder acknowledged 126 frames, with no pending capture, reported boundary, partial fleet or recorder/protocol problem. Quit and process exit were normal, exit 0, with no forced termination. The actual selected-light owner startup receipt was observed.
- Root inspected the first, middle and final captured frames. These show the starting grid and racing on GC terrain, including the camera racer and opponents. The world remains incomplete. These views do not independently certify numerical lighting correctness, matched before/after appearance, absence of every visual defect, or a complete lap.
- Capture sampling was capped at 5 frames/s; it is evidence of the bounded gameplay slice, not a smooth-video or performance benchmark. The separate source WAV is 22.528 seconds; it must not be stretched to claim wall-time audio synchronization.
- Native Gate and authored fog options were off. Full original material/alpha, specular, fog, other-track lighting, complete-track traversal and Windows UI remain outside this pass.

Historical evidence set names: `gc-lighting-validation-01` and `gc-lighting-validation-20261012-01`. Their local raw receipts, PPM frames and WAV were lost in the 2026-10-12 workspace rollback. The previously delivered silent gameplay video remains saved separately in Library; do not claim the raw take is still available locally. No second game execution or software check was performed to prepare that presentation artifact.

## Next default-on fog observation

Runtime assets were regenerated successfully on 2026-10-12 at 02:31:05.599140 UTC; see `recovery/20261012/cold-asset-preparation-receipt.json`. That was cold preparation only. The recorder now requires the actual authored native fog initialization line, finite ordered start/end values, and its existing actual selected-light startup receipt before accepting Ready when those policies are enabled. The new recorder guard is unexecuted. It proves initialization when observed, not visible fog, a green-hue reference match or original shader parity. No default-on fog gameplay result is claimed yet.
