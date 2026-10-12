# Native TA authored lighting

Status: integrated candidate; uncompiled and untested. The successful GC observation on 2026-10-12 tested source 0f1c905a, before this change, and does not validate TA or this revision.

Use RR_TRACK=TA and RR_NATIVE_TA_SELECTED_LIGHTS=1 for an explicitly admitted fresh native race. Default is off. GC retains its separate option. Both options together, wrong track, checkpoint restore or Main-only mode reject.

The native owner authenticates TA SCB SHA-256 c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d, owns its nine zones and ten portals, and retains all 26 authored lights: one sun, eight ambient records, and 17 spotlights including 14 negative lights. First-zone ambient initializes every zone before later overrides; zone 9 retains that first ambient. Authored zone 3/4 sun overrides are copied directly, without a second halving.

## Explicit host policy

- Genuine racer visibility membership and scoped draw publication are retained. Each actual component's current world position is read after its real geometry projection. Copied matrices travel with the corresponding geometry; stopped racers retain both together.
- Frozen authored lights belong to the admitted native session. The host scans spotlights whose authored InitialZone matches a current racer membership zone. It retains the two smallest positive attenuation/cone denominators, preserving authored numeric order on ties, and reserves the third light slot for the sun.
- Ambient and sun blend across actual incident visibility portals using the existing source-backed portal-overlap geometry and source radius 500. Selected negative contributions fold into sun and ambient once. Unselected negatives do not darken the whole level.
- The existing native RGB Lambert shader evaluates all three light slots with current host world normals. Existing material, alpha, fog, specular and unlit/prelit/effect exclusions remain.

This is a deliberate native query, not retail LightManager SearchList ordering, physical light-volume registration, cache lifetime, EE/VU arithmetic or original generated-color parity. No guest instructions or hardware emulation are used. Native per-component selection may differ from original cached selection; cross-zone spotlight eligibility is deliberately limited to authored zones. Dynamic light animation and source flare rendering are outside this path.

The recovery recorder accepts --track TA --native-ta-selected-lights and requires the distinct TA owner receipt at actual ready admission. Its previous GC defaults, all-eight-original-AI counters, bounded capture and normal-quit accounting remain. This recorder is Linux-only; it is not Windows verification. Source review found and removed an initially premature receipt check before the startup drain loop. No executable check was run during that correction.

Both platform build manifests already include the modified owner translation unit. All execution remains subject to the centralized rolling-hour slot; this document grants no test authorization.
