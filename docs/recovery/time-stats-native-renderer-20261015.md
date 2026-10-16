# Isolated TimeStats renderer candidate

Source-only candidate. No compile, syntax check, tests, GL invocation, conversion, Windows operation or central edit was performed.

## Integration API

- `VehicleWindowGl50DS::bindOriginalTimeStats(std::shared_ptr<game::OriginalTimeStats50LA>, VehicleWindowDiagnostic50DS&) noexcept`
- `VehicleWindowFrame50DS::originalTimeStats`: raw pointer matching the strongly retained bound owner; null means no TimeStats this frame.
- Diagnostics: `originalTimeStatsQuads`, `originalTimeStatsLeaves`, `originalTimeStatsRejected`.
- Include `original_time_stats_render.inl` after the existing lap HUD include. It deliberately reuses existing `hudName` and strict `hudSampler` helpers, shared UI shader/program/vertex/index buffers, native texture upload owner, framebuffer capability checks and checked deletion helpers.
- Root owns terrain activation, coherent bank capture, source numeric update/display, and teardown order. No activation is in this patch.

## Admission and submission

Cold binding requires actual owner CPU-resource completion and exact immutable two-image bridge. Both actual material passes go through `prepareOriginalUiNativeMaterial50KG` against their own decoded atlas. Only source-alpha, ScreenMT reverse-Z GEQUAL/write, no fog/alpha-test, matching min/mag, supported clamp/repeat and one mip are admitted. The authored stats `unique` and font `none` are preserved. No substitute pass or registry rank is fabricated.

Frame preparation copies owner-prepared geometry and packets, checking bounds, finite coordinates/UVs, packet coverage, exact proven Z, packet alpha ceiling, actual viewport/generation and monotonic actual collector-leaf order. It preserves packet order instead of doing another sort. A requested invalid frame rejects submit before pending target writes. Quads are copied into renderer-owned bounded storage; no pointer into mutable owner geometry is retained across GL work.

Drawing uses the existing shared source-byte UI composition program into a private native-resolution color/depth target. Both passes use actual source-alpha equation and independent stored-alpha replacement. Coherent fetch is reused if already compiled; otherwise the proven texture-barrier triangle path is required. The only gameplay RGB write is a checked private-target blit; existing frame transaction handles failure. Native presentation alpha is restored opaque. The patch places TimeStats after existing lap HUD and before existing gameplay blur: this is explicitly native host scheduling, not proof of complete retail cross-HUD collector ordering or parent traversal.

## Lifetime and failure

The renderer holds a strong canonical TimeStats owner through all GPU work and cleanup. It releases this owner only after acknowledged `glFinish`, image/target/depth cleanup. Incomplete cleanup retains storage and owner. SourceCamera viewport remains a synchronous external borrow: root must close the renderer before destroying its camera session or actual Application/Video host. Root already owns that teardown ordering.

Shared UI partial compilation failure blocks both HUD late-bind APIs from accidentally reusing partial shared names. Cold failure keeps real allocated names for existing renderer close if cleanup cannot complete. Resource presence participates in `usesTextures`, `hasGlResources`, unit-one unbinding, and close's completion requirement.

## Remaining validation boundary

This is untested source. No performance or visible parity claim. Native GL texture sampling/rasterization and existing aspect-preserving 640x448-to-drawable mapping are reused, not PS2 raster emulation. The two source-alpha policies must actually pass strict native admission; rejection must remain visible rather than broadening policy. Root should review its latest central owner API and integration conflict resolution before applying this focused patch.

## Root integration

The native ALL8 original-camera path stages the owner after original track
material loading and before canonical racer visual/GPU publication. It binds
actual timing handles and uploads the two atlases after successful camera cold
preparation. No human path or AI controller identity is modified.

Original animation state advances inside the existing accepted-world phase.
The display snapshot is taken after original manager gaps and camera publication;
uninitialized manager rank state does not fabricate a snapshot. Once admitted,
requested CPU/GPU failures reject the frame instead of silently claiming success.
Completed-window logging occurs only after the actual gameplay display boundary.

Cleanup closes the renderer and its strong owner first, then releases the root
TimeStats result before remaining canonical model refs and the actual Video host.
The Application font registry remains its existing process owner across sessions.

This source integration still awaits the one centralized Windows build and
visible Gasgano/Sullust smoke. No Linux test or independent helper check was run.
