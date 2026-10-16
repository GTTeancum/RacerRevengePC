# Native static light and coverage continuation

## Same actual RA light owner

Use `original_gc_gameplay_lighting_candidate::Owner` already owned by RA aggregate. Its Storage contains the authenticated scene/zone/light authoring; existing DrawLease is racer-only and stays unchanged. Add a separate private typed native-static publication path, requiring the actual world owner/placement, exact source zone and visual matrix, and an immutable actual SourceCameraSession publication identity. A private bridge can be friended by the aggregate/world and light owner; no caller light matrices, guessed zone or source-ready boolean.

Proposed bounded operation: on the genuine published-camera/idle-bank boundary, prepare native-static light snapshots for the actual black/DiffuseCompressed placements from the same RA authored zone data. Reuse the existing explicitly native authored-zone/top-two selection arithmetic after factoring it away from racer binding checks; retain its negative color handling. Do not duplicate manager/storage, create a fake19C membership or call racer beginDraw. The snapshot owns values and keys them to the real placement, same light owner lifetime and actual camera publication. A terminal invalidation/source change invalidates the snapshot. No new clock/RNG.

Selection is deliberately a native static-world policy rather than retail UseLighting interpretation. Preserve authored UseLighting data separately. Use retained actual visual matrix and original normal quantization with `recoverCompressedNormal`, `prepareDiffuseNormalAxes` and existing host diffuse numeric kernels. A dedicated static-diffuse stream/mode distinguishes generated lighting RGB from authored prelit colors. Do not set racer sourceDiffuseCompressed flag or force white colors. Model ambient/alpha defaults and static output alpha must be explicitly defined from recovered source fields or documented native policy before shading; no invented retail field state.

Concrete files: existing lighting candidate h/cpp for shared numeric selection and typed static lease/publication, world draw-plan include for private placement binding, SourceCamera adapter for authentic publication, vehicle_window_gl50ds for dedicated static mode. Existing racer leases and generic track admission stay unchanged.

## Meaningful per-material multisample coverage

Recommended policy: explicit four-sample native coverage for authored onePassAA1 surfaces; preserve ordinary single-sample coverage behavior for AA0 using controlled multisample state. This is a documented native coverage choice, not GS sample parity. Never silently reduce to fewer samples or ignore AA1 if capability/format/target support fails.

Own multisample RGBA/depth targets at the actual gameplay viewport, verify actual formats/dimensions/sample count, retain original alpha-test/ALPHA/depth-write rules, and resolve through explicit checked framebuffer blit only after the relevant complete scene pass. The final resolved RGBA image must feed existing blur/history/display in unchanged order. Names are stored before error checks; allocation failure and checked cleanup retain retry state. Resolve is not a completed GPU receipt; genuine glFinish/completion remains required before publication/stamps.

Important coupling: current renderer creates single-sample gameplay targets and source-pair specular feedback uses single-sample textures. World alpha surfaces are currently drawn on both sides of racer rendering. An early world-only resolve followed by later crowd drawing does not preserve shared sample/depth state; single-to-multisample depth copying is not an interchangeable continuation. Therefore the coherent implementation must either:

1. Carry the common world/racer/depth transaction in an owned multisample target and adapt source-pair feedback to genuine per-sample storage/read/write, resolving once before history/display; or
2. Deliberately redesign the native world coverage compositing transaction with an explicitly defined, reviewed depth/alpha policy that preserves all surfaces. This is a presentation-policy decision, not an unnoticed shortcut.

Option1 gives the clearest consistent depth/occlusion semantics. It requires checking the actual GL capability set for multisample textures, sample-specific shading/fetch and source-pair feedback handling. Existing GL3.2 loader/GLSL150 is not automatically sufficient for a complete sample-specific feedback implementation. New proc/capability checks must fail closed before publishing the route. No implicit fallback, fixed global light, dropped AA, altered material depth writes, AI/physics changes or partial-world gameplay admission.

First next implementation should be the typed static-light adapter and exact generated diffuse stream under this native policy. The multisample target/feedback transaction should be scoped separately after root chooses the coherent shared-target approach; do not mix broad renderer changes into the cold draw-plan patch.
