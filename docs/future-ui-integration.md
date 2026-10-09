# Future UI integration

UNTESTED active candidate as of 2026-10-09 16:26 UTC. Root combined the full original roster with next-session Audio Options and native race/current-lap readouts in this separate worktree.

The merge retains table-driven eight-racer selection, immutable loaded identities, audio choices applied only on explicit Start/Restart, unchanged menu page IDs, source CB8/CB0 timing, and existing source audio group ownership. Four contextual patch conflicts were resolved by preserving both roster and audio behavior. No builds, syntax checks, runtime tests or asset decoders were run.

The failed menu/Clegg candidate remains preserved in Git. Its headless pause blit correction is included here. This combined candidate awaits the next central menu/Clegg capture test. Other-track assets remain outside this batch.
