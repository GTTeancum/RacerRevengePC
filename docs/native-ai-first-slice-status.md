# Native original-AI first slice: bounded runtime result

Guarded source `93201627` passed its one authorized direct-GC observation on 2026-10-10. It compiled and linked, then ran one 12-second ready window including countdown with clean exit. Clegg in slot 1 was the only original-AI racer; all eight actual racers advanced their native counters while the remaining racers retained HOST controls.

Actual telemetry observed state 4 at CB8 1.232 followed by state 2 at CB8 5.45599, ten brake commands and ten native-bound simultaneous brake/steer events. Source boundary remained zero. Five-second sampling does not prove the exact transition instant, cold state 0 or completion of each bound command's entire physics frame. See [current validation](current-validation.md) for source/binary identity and artifact limits.

The guard requires every requisite racer active and Ready before observations/manager mutation, actual contacts enabled, and one cold session. Failure-trigger and restart paths were not exercised by this successful prefix. Actual AI dynamic contact, eligible combat, new route choice, reset, lap/finish and unsupported owner domains still stop explicitly. No lap or full original-AI claim follows.

The next requested scope is all eight racers on original AI. That is future work, not the tested one-racer behavior. Separate dynamic-contact and lighting candidates remain untested. The original frozen source states and subsequent guard correction remain in Git history. Original assets and raw evidence are excluded.
