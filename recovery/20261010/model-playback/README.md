# Native shared model playback candidate

Staged source only, UNTESTED. No active repository edits, builds, tests, syntax checks, project-parser execution, sanitizers, gameplay, or guest execution. Static source inspection and bounded independent original-data reads only. Parent owns integration and all software checks.

## Files and API

New `src/game/model_playback50kg.h/.cpp`; full replacement `src/game/model_motion49t.cpp/.h` with exact preimages. Existing49T Eval, key readers, original quaternion interpolation, matrix conversion, scene graph/setter/cache owners are reused. No duplicate MOT parser, render owner, palette owner, guest instruction engine or VU emulator.

`ModelPlaybackMotion50KG` is a stable retained MOT descriptor with original resource B4 looping metadata. MOT header duration is authoritative. `ModelPlayback50KG` retains current18/previous1C, stopped10/blending30, current34/old38/end3C/start40/duration44, captured per-binding quaternion/position, and original155730 saved local matrices. UI polling reads `state.time34/state.current18->motion->duration` before update; no fabricated readiness timer exists.

Lifecycle functions:
- `startModelPlayback50KG`: original155800, only stopped/current/blending/current-time stores.
- `captureModelPlaybackMatrices50KG`: original155730 local-matrix snapshots in binding order. UI invokes after original entry .001 update.
- `transitionModelPlayback50KG`: original155830, preservePhase0 only. Nonzero blend retains old motion/time unless already blending or no old motion; those branches capture actual scene quaternions/positions instead. Zero duration takes the immediate start path.
- `bindModelPlaybackIk50KG`: original156620 endpoint registration order and sorted unique ancestor binding rows. It does not manufacture solver coefficients. Attach the actual mode0 solver and immutable coefficients from the separate `character_mode0_ik50kg` owner.
- `advanceModelPlayback50KG`: direct clock/update owner, staged graph/playback/motion-shared/mode0-shared transaction. Source entry lazy433468/guard3D5DBC is now part of existing `ModelMotionSharedState49T`, with default zero new fields; original interpolation/pair fields remain intact. This grows that shared record and must be included in the parent's memory/budget accounting.
- `sampleModelMotionTransition50KG`: implemented in existing49T translation unit to reuse its private Eval. Two independent direct times; previous MOT or captured pose; no fractional pair shortcut. Original fractional49T functions are unchanged.

`advanceModelPlayback50KG` arguments, in order: dt, bindings, graph, IK config, playback, motion shared, mode0 IK shared, fixed workspace, diagnostic. All descriptors/input records remain alive and disjoint. Persistent shared lazy records belong to the process, not individual characters.

## Original clocks and arithmetic

1544F8 increments current34.151160 reports completion strictly when time exceeds duration. Looping subtracts exactly one duration; no modulo is invented. Nonlooping still samples the overshoot frame, then154A00 stops and clears34/38. The direct sampler retains last-key rotation and source Bezier endpoint extrapolation.

During a blend1548D4 increments old38, then computes `((old38-start40)-dt)/duration44`, clamps and applies `(3-2u)*(u*u)` with separate binary32 operations. The subtraction of dt is deliberate: first-blend weight uses the prior elapsed time.1549DC clears the blend only after that frame when old38>=end3C. Both actual motion clocks and stopped state commit only if the whole operation succeeds.

154A40 samples new time34 and old time38 without separately wrapping old time. Rotation interpolation is existing source14FD00 with old then new arguments and new weight. Positions are `new + (old-new)*(1-weight)` in source154D84..154DE0 order. This differs in rounding order from historical fractional pair49T, so the existing fractional helper was not repurposed.154FB0 samples the new resource and reads old quaternions/positions by original binding row from the captured pose. Capture uses original28A890-equivalent `matrixToQuaternion48G` and copy-only28B180 positions.

## The so-called scale path is endpoint IK

Source evidence corrects earlier terminology:156620 creates actual two-bone endpoint chains;156960 samples a complete pose, obtains endpoint world positions through28B1A0, and accumulates weighted targets.28F580 is a two-bone IK solver dispatch, not scale multiplication.

On blended frames with model204 set:
1. Zero target XYZ accumulators.
2. Sample every new MOT channel, refresh actual endpoint world positions, accumulate weight*worldPosition.
3. Sample every old MOT channel, or restore captured quaternion/position for the sorted156620 ancestor rows; refresh endpoints and accumulate (1-weight)*worldPosition.
4. Execute the actual channel crossfade using154A40/154FB0.
5. Invoke the solver in original endpoint registration order for each accumulated world target. No endpoint-orientation assignment is added.

The prepasses still occur with204=true/count208=0, matching source calls; count0 skips only endpoint loops. The pose prepasses and final writes reuse scene cache/dirty owners. Missing enabled nonzero solver rejects before any live clock/graph/shared mutation, with diagnostic source28FFD0. It is never treated as successful animation.

The callback receives immutable source-initialized chain context, index, actual target, staged `ModelMode0IkShared50KG`, and staged graph. It may mutate only those staged objects; this preserves atomicity if a later endpoint fails. `character_mode0_ik50kg.h` is supplied by `/workspace/shared/recovery-20261010/character-mode0-ik-reconstruction`, not duplicated here.

## Rider19 activation evidence

Initial entry sampling is nonblending, so154538 follows154544 and does not call the IK solver. UI load1D2478 advances entry by0.001, captures155730 at1D24A8, registers existing l_ik then r_ik via156620 with absent axis, and1D251C sets model204=1. Absent-axis156920 sets chain mode10=0. An actual nonzero-duration155830 transition sets blending30=1;154994..1549D0 invokes28F580 once per208 without any weight gate, including the first weight-zero blended frame.28F5B8 dispatches mode0 to28FFD0. It is therefore mandatory on real transitions, not on the initial entry-only frame.

Independent authenticated19c PSG byte reading shows intentionally cross-named parent chains:
- l_ik node12 -> r_calf11 -> r_thigh6 -> pelvis0
- r_ik node10 -> l_calf8 -> l_thigh3 -> pelvis0

Do not reorder or repair these using name intuition. Full bindings remain supplied by existing original VUT/PSG owners. The helper rejects ancestry outside its admitted bound graph rather than inventing unbound row indexes.

## Remaining integration and checks

- Integrate the mode0 chain constructor/callback candidate, call its constructor at the exact entry .001/capture boundary, then install immutable context and callback. Keep setup source libm and process shared lazy owners persistent as required by that owner.
- Rider owner joins exact looping metadata, six stable MOT descriptors, original scene root/observer lifecycle, RNG choice, and actual palette/render collection. This candidate does not advertise screen availability or UI readiness.
- Parent must include the new playback translation unit and its mode0 dependency in intended build targets; no build list was edited here.
- Only parent may perform the next authorized software check. Required coverage: legacy49T preservation; two independent clocks; strict duration boundary and nonloop overshoot; single-subtract loops; first/final blend frames; interrupted captured-pose transition; original matrices/caches; enabled204 count0 prepasses; missing solver atomic rejection; authentic endpoints/real mode0 callback; shared guard persistence; no allocation/bounds.
- Source semantics are recovered but not runtime-validated. ShortAxis host-double trig precision policy from the prior candidate remains explicit; no bit-parity claim is made for those reconstructed tables.
