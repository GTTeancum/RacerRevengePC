# Source-only dependency, budget and clock review

Reviewed text only; no build, static-analysis tool, parser, ABI probe, tests or runtime execution. Candidate frozen after agreement with mode0 and Rider workers.

## Include and link dependencies

- model_motion49t.cpp includes model_motion49t.h then model_playback50kg.h. The latter includes model_motion49t.h under pragma once and character_mode0_ik50kg.h. This is not a recursive type-definition cycle.
- character_mode0_ik50kg.h includes existing chariot_ik49t.h, which includes scene_graph49p.h, vehicle_visual49r.h and source_acos49t.h. None includes model_playback50kg.h. Vehicle visual's coupled/body headers stay downstream and do not include playback.
- Mode0 worker supplies new character_mode0_ik50kg.h and additions to existing chariot_ik49t.cpp. Playback supplies new model_playback50kg.cpp and changed model_motion49t.cpp/.h. Existing model_motion49t callers do not call new playback functions; the new helper in that translation unit uses existing Eval/scene APIs only, so it introduces no callback implementation link dependency by itself.
- New model_playback50kg.cpp requires existing scene_graph49p, model_motion49t, body_scene48g functions; concrete mode0 callback is provided through a function pointer. Parent must include the new translation unit in target(s) using playback.
- Rider dynamics original_ui_rider_character50kg.{h,cpp} includes playback header and uses current18->motion->duration, time34, all start/capture/transition/advance signatures with separate motion-shared/mode0-shared arguments. Those names/signatures match the frozen header by source inspection.

## Record growth and retained evidence

ModelMotionSharedState49T's original first two uint32 words and two uint8 guards retain their ordering. Appended updateValue433468/default0 and updateGuard3D5DBC/default0 conventionally change a four-byte-aligned layout from12 to20 bytes. This is a source ABI estimate, NOT a measured sizeof result.

Consumers were searched across src/tests/tools; retained reference listing is evidence/shared-consumers.txt.

- VehicleAnimationShared49T embeds motion. VehicleAnimationWorkspace49T and VehicleFrameState49W embed that shared type; their scratch/copy/overlap operations use sizeof and ordinary typed assignment. No fixed shared-record byte extent was found requiring a narrow source correction.
- Found size static_asserts cover VehicleAnimationState49T==19*4, ChariotIkPair49T==40*4, and unrelated visual/frame records. They do not constrain the enlarged shared record.
- Historical49T/49U/49V benchmark loaders and hashes explicitly serialize the original four motion-shared values. Their default-initialized host records leave new playback-only fields zero, and original sampler/vehicle routes do not touch them. Retaining those historical formats is intentional. Do not add nonexistent original playback activity to historical goldens.
- tools/budget/bench_frame49w.cpp RR49WB01 is a raw host-ABI fixture. It explicitly compares serialized sizes to sizeof(Immutable), sizeof(VehicleFrameState49W), sizeof(Expected), then reads those whole typed records. Pre-growth fixture bytes must not be reused or silently reinterpreted. Its current ABI guard should reject them. Regenerate the producer's fixture only during the authorized software check; never loosen the size check or rewrite historical evidence to claim it already passed.
- measure_chariot_animation49t.py, measure_chariot_root49u.py and tools/budget/profile_frame49w.py derive record sizes from freshly compiled sizeof output. Their historical reports become stale for this new build. Re-measure at the authorized check; no constant or budget limit was changed here.

No budget code candidate was justified. Source source-limits and historical evidence remain unchanged. New playback state/capture/workspace and32KiB ShortAxis static tables also need explicit new-host memory accounting, separate from historical phase budgets.

## Clock control-flow review

Source154548..1548CC (nonblending) joins1549DC after sampling; the blending branch joins the same block.1549DC compares old38 with end3C and1549F0/1549F4 clears30/38 when old38>=end3C. The candidate's unconditional post-sample clear is therefore intentional. Initial old38/end3C are both0, so initial clear is harmless and source-correct. Start155800 only resets10/18/30/34; candidate does not invent38/3C/40/44 clearing.

151160 checks whether current time is <= duration; completion means strict > duration.154528 subtracts only one duration for a looping current motion. No fmod/multi-loop correction is introduced, even for large dt. The outgoing old38 clock advances once per blend call and is not independently wrapped. Nonlooping overshoot is sampled before stopped10 and34/38 resets. Interpolation uses independent new34/old38, not a normalized common fraction.

## Exact remaining integration dependencies

1. The separate mode0 candidate must be integrated, including source-initialized immutable chain records, shared lazy state and callback; constructor runs after entry .001/capture.
2. UI manager still owns actual model root/attachment/observer setup, stable descriptors/cold resources, source-loaded/readiness status, and palette/render collection. These are not implied by staging playback.
3. Build target wiring for model_playback50kg.cpp (and Rider new translation units) and the parent's first authorized software check remain pending.
4. All immutable resources/descriptors, graph, state, shared records and scratch are documented disjoint lifetime/serial-access preconditions. Exact overlap guards defend key fixed records; they are not a substitute for that public contract. Callback external mutable state is forbidden; only staged graph and explicit staged lazy records may change.
