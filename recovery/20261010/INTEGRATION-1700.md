# Native Main UI integration, 2026-10-10

Historical 17:00 integration note. For the later fresh Linux build and failed
Main-only observation at 17:22, see `MAIN-OBSERVATION-1722.md`.
At this note's original checkpoint, none of the changes below had been compiled or run.
The last centralized observation remains Linux commit `17659ca`, started at
16:19:41 UTC; its 139-second GC capture ended at the unfinished original AI
state-5 recovery boundary. It does not validate these later changes.

- `70369e8`: authored GC Gate action joined to opt-in native collider/sphere
  contact and the shared opening visual/collision animation owner.
- `e6b5d7f`: original UI boolean, base-zero numeric and comment parsing.
- `0c3bbf3`: authentic Main logo missing-material fallback and zero-map panels.
- `fab9a0a`: shared Main widget bank, navigation, camera, and correct control
  enable callbacks. Empty widget IDs remain empty; no dummy entities.
- `7072b40`: native Main particle camera projection.
- `56af479`: original Main fire assets, emission, lifetime and packet ownership.
- `13cf969`: actual Main ambient particle lighting.
- `1352a6c`: original Main music/effects transport with dedicated frontend voices.
- `36fdddd`: native Main fire geometry, actual two-state material, alpha test
  and fog submission. Full original texture alpha bytes are retained.
- `41492c2`: original page-button material fallbacks and sixteen-leaf registry
  order. The renderer bounds texture/vertex alpha products rather than rejecting
  authentic texture alpha above 128.
- `11fd32a`: actual initial Main lifecycle and entry-only aperture callbacks.
- `de601f7`: opt-in original Main application entry, genuine video/texture
  ownership, native input and explicit muted opening/idle capture protocol.

The Main entry source still awaits its session, draw collector and final source
fog/camera helper before it can be built or run. Static source inspection found
that scene fog changes the particle camera near/far planes to 1/4000 during
render; the earlier cold far plane of 100000 is not the final particle state.
The first Main observation will be explicitly muted and will not launch a race.
Parsing other page descriptors does not implement those page controllers or
admit their routes. The existing gameplay HUD and host menu remain temporary.

All source is native host CPU/GPU implementation. Original executable bytes
are static research inputs, never executed or interpreted. Native policies for
undefined emission-count behavior and swept Gate geometry are documented in
their owners. Original assets are not part of this source-only Git deliverable.
