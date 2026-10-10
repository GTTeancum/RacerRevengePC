# Native static-world fog reconstruction (2026-10-10)

This is new code against recovery baseline 32df461. It is not a byte recovery,
not asserted identical to the lost implementation, and not retail shader parity.
No guest execution, PS2/VU emulation, game-state changes or substitute assets.

## Provenance and gate

The authenticated sky fixture (`tests/native/sky50gw_fixture.h`, Source table and
Owner::load) corroborates the exact SCB Fog.ColorRGB values:

- TA: 175,182,207, member offset 9928; scene SHA-256
  c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d.
- GC: 151,160,177, member offset 27716; scene SHA-256
  d47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d.

The cold fixture verifies both archive and SCB hashes and the exact color field.
This reconstruction consumes its already-published normalized color. It does not
alter sky images, sky geometry, the sky shader, or the viewport clear color.
There is no blanket green filter or screen-space grading.

The parent's retained pre-loss design provides minimum 1333.300048828125,
maximum 4000, haziness 0.5, TA visibility 1, GC visibility 0.5, and:

    end = minimum + visibility * (maximum - minimum)
    start = (end - 64) * (1 - haziness)

This gives TA start 1968 / end 4000 and approximately GC start 1301.325 /
end 2666.65 in host float arithmetic. Available current source corroborates RGB
but does not independently establish these range constants or formula. They
remain historical-design evidence, not independently verified retail provenance.
Consequently RR_AUTHORED_WORLD_FOG=1 is required to enable this reconstruction;
unset or 0 disables it, other values fail clearly. The historical default-on
behavior is deliberately not restored while this evidence gap remains. Enabling
logs the range and provenance limitation. No implicit inference from track tint.

## Native rendering path

A cold descriptor carries the explicit fog RGB/range; submit requires and copies
three finite world-camera coordinates when enabled. Existing positional frame
initializers remain valid because the new camera field is appended. The entry
point supplies the same current camera.eye that produces its native view.

The native world vertex shader computes clamp((length(worldPosition-camera)-start)
/(end-start),0,1). No view-Z, clip-W or extra perspective-distance multiplier is
used. This per-vertex scalar uses ordinary GLSL smooth interpolation; it is not
recomputed from interpolated fragment positions. The fragment shader mixes RGB
only after the existing texture/light/shadow calculations. Source alpha is
unchanged. Disabled fog skips the RGB operation entirely.

Eligibility is cold metadata, never an added frame-time allocation:

1. Single-pass surfaces must be opaque and depth-writing.
2. A two-pass surface must be a complete validated projected pair: projection
   slots 1/2, both required textures and unlit, opaque depth-writing base,
   source-alpha overlay, no fixed additive alpha and no zero-alpha discards.
   Both passes receive the same eligibility and vertex factor. Existing track
   validation already requires adjacent passes repeating exactly the same
   index/vertex surface; source fixture validation admits ProjectedPrelit only
   for GC two-pass materials and validates every source program/pass.
3. Other blended/additive surfaces, incomplete/unsupported projected policies,
   sky, racers, electrical bands, UI and dynamic-world owners are excluded.

For source-alpha overlay a and common fog amount t/color F:

    a*((1-t)*layer+t*F) + (1-a)*((1-t)*base+t*F)
      = (1-t)*(a*layer+(1-a)*base) + t*F.

Fogging only one pass would violate this identity. Existing normalized-target
clamping/quantization can affect exact numerical equality; framebuffer format,
source alpha scaling and material policy are unchanged. This is host blend
algebra, not a retail multipass fidelity claim.

Every track bind starts disabled and every span explicitly selects eligibility.
Dynamic-world binding explicitly disables fog. resetPodState clears the world
program's enable uniform before selecting the pod/sky path. The separate UI
shader and its existing inactive-by-default renderer remain unchanged.

## Review and outstanding verification

Only source reading/editing, diff inspection and SHA-256 recording were performed.
No build, syntax check, test, importer, gameplay execution, emulator, PC action or
Git action was run. Root owns the central verification slot.

Recommended central verification: shader uniform/link checks; disabled baseline;
TA/GC enabled captures with actual camera positions near/start/end distances;
paired ProjectedPrelit surfaces and unchanged alpha; blended/electrical/sky/UI
exclusion; camera validation rejecting null/nonfinite before GL work; switching
between fogged spans, excluded spans and frames without state leakage; prior UI
checks. Restore independent source evidence before considering default-on.
