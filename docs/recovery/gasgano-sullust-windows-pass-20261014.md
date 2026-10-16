# Gasgano / Sullust Windows bounded smoke

Tested commit602643e80fa69f57272e6b45bf88b69e98588a90; tree9c484e6504e828ea2e1fb2b24440c4747216450d. Windows build358/358 exit0 (2026-10-15T00:36:42.664639Z–00:39:10.265637Z). Visible smoke natural exit0, finished00:40:39.021831Z; build and game supervisors confirmed zero remaining processes. Native exe beside SLUS in the original asset cwd; SLUS was not executed.

Root reviewed Main, Gasgano7/slot10, Soro Suub Facility course11/planet4/slot2 and actual gameplay screenshots. All eight racers used original AI, including followed Gasgano; human count, controller samples, keyboard packets and host-driver calls were zero. The disclosed process-local test profile makes Gasgano available and initially remembered; this is not an earned unlock or saved progress. Complete locked-roster preview navigation is unfinished.

Baseline45 GPU-completed actual window images /5.0164073 seconds =8.970563454845463FPS, after5.0078987 seconds excluded warmup. Muted output and existing trace settings retained. This is poor performance and not a performance acceptance or full-race validation. No fresh Linux test occurred. Historical Mon Calamari/Ody955c12d was34.048703FPS on a different course/racer and is not a controlled before/after comparison.

Evidence Library libfile_f9fbb78790fc81918484ec4f0c5fadcf; ZIP SHA2567e8e2dece71183997e00edbf212d0483d98a4e7cab58c823d08838f6c3f7c30d. EXE SHA256fc92dc517abb56c090078b401864f38094ddcd09e86df13bec73e7bfb2113e75.

Failure recovery ended after this pass. Next ordinary centralized test is no earlier than2026-10-15T01:40:39.021831Z.

## Timing coverage follow-up (source only)

The final telemetry interval has44 render calls and176 physics calls; its window differs from the45-swap baseline. Existing physics/render/audio counters leave about2.4 seconds unassigned. Source review found the normal source-camera completeGameplayDisplay call outside phase scopes. It includes retained history staging, existing glFinish, SDL swap and publication. completion_wait=0 therefore does not prove absence of waiting; the residual also includes other unmeasured orchestration and telemetry. The added CompletionWait scopes measure existing completion calls only and close before FPS receipt work. They add no synchronization and have not been executed yet. No GPU-only bottleneck or speedup is claimed.


## Subsequent untested changes

- `b3badd5`: scopes around existing source-camera display completion and initial underlay completion. No extra synchronization calls.
- `14b2efb`: seven integer uniforms cache only within one uninterrupted track pass. The first span emits all values. Source UV callback spans emit all values and invalidate reuse for the following span. All draws, texture bindings, floating-point uploads and GL error checks remain present.
- `9e5e7dc`: a disjoint `post_world_service` timing phase covers existing FIFO/event dispatch and opponent audio updates, ending before shared mixing.
- `bafd936`: the existing completed-results audio mix/service now contributes to `audio_commit`.

Physics and AudioCommit scopes are temporally disjoint. Physics does include embedded audio callbacks, while AudioCommit includes mixing plus music/announcer world service. These phase names must not be interpreted as pure subsystem cost. PostWorldService closes another previously unmeasured gap without moving work. None of these changes establishes a speedup or a new Windows pass. The next central check must retain Gasgano, Sullust, the same display profile and all eight original AI racers to make a useful comparison.
