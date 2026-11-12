# RacerRevengePC: governing design rule

Applies to every agent, plan and doc in this repository. Where an older doc or plan says
otherwise, this file wins.

## One common path: every course, every pod

The original game runs one race-load and race-frame path for every course and every pod.
Courses and pods differ only in the **data** they load (track files, zones, water, PHY,
models, materials), never in which code runs. The port must do the same.

- **No per-course code paths.** No `isRaTrack`-style branches, no "RA route" versus
  "common-course route", no course-by-course bring-up switches (`RR_..._ORIGINAL=TA,SA,...`),
  no "RA keeps its own providers". A course-dependent branch is allowed only where the
  original code itself branches, and then it must branch on the same data the original
  reads (for example "this track has water"), not on a course name.
- **No per-pod code paths.** All 8 pods (player and AI) load through the same pod load
  (2129C0) and the same wreck-piece chain (165E90). Pod differences come from that pod's
  files (piece counts 7 or 9, PHY values such as the explosive-wreck speed at +50,
  materials). No fix may be keyed to one pod or one pilot (for example the hand-written
  pod-load exceptions for pods 02, 09, 12, 18, 19, 20, 22 and 25 listed in
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\branch-inventory\PLAN.md`); if one pod
  exposes a gap, fix the general rule the original uses for every pod. Where the original
  itself keys on a racer (for example the AI steering boost for racers 4, 8, 18 and 23),
  the port does the same.
- **Human versus AI** differences are allowed only where the original branches on them
  (for example the human manager tail 1AE120, the human-only limp, vibration).
- **Plans are written once, for all courses and all pods.** A course or pod may be used to
  *test first*, but the code is written as the common path from the start, and verification
  covers every course with rotated pods before a build is handed over.
- **Existing per-course or per-pod branches are port stand-ins** to retire into the common
  path, not designs to copy. Flag any you touch.

The other standing rules (faithful only, flag stand-ins, hooks never write memory something
else owns, bounds-check every write, muted logs-only tests, no screenshots in tests, no push,
scratch under `D:\Programming\GitHub\RacerRevengePC\work\scratch`) remain in force; see
`D:\Programming\GitHub\RacerRevengePC\docs\HANDOFF-20261106.md`.
