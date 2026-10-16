# Dud Bolt / Watchtower window evidence route

The opt-in `RR_UNATTENDED_UI_ROUTE=main-dud04-course-ga` navigates the actual original Main, Rider, and Course owners. It does not change a profile or unlock mask and does not supply driving input.

- Start from the authenticated fresh Main selection and Rider19/slot1.
- Issue original Rider Next events until actual rider ID 4 (Dud Bolt), bounded by 26 moves. Each move requires the original deferred refresh and a newer accepted source frame in the retained GPU publication.
- Capture only the settled Dud selection, then issue original Confirm.
- Follow fresh Course slots 1 through 4: TA (course0/planet0), SA (11/4), MA (4/5), GA (8/1). Intermediate course selection does not launch those tracks.
- Capture only settled GA/Watchtower, with original 3-lap and 8-entrant settings. Confirm still requires the real prepared destination and explicit all-original-AI policy.
- Preserve the existing actual framebuffer and GPU-completed baseline FPS capture; menu navigation completion alone is not evidence of a successful race launch.

## Windows evidence, 2026-10-14

Central Windows Release build and visible smoke passed at commit
`68c157d2ca8ac5ea8a4a6c4eb4eb064ba9e85210` (tree
`6b360bc5ed9fdc362b512f38b358033f157d26c0`). The smoke ran from
12:01:23.836095 to 12:02:10.403584 UTC, exited zero, and confirmed cleanup.
The native executable was beside SLUS in the original asset folder; SLUS was
never executed. Executable SHA-256:
`c4839f603a9edbe494badf1787b87c07467b0e31914af9f69da51c0e2d0500f8`.

Fresh Main, Rider (Dud04/slot6), Course (Watchtower/GA), and gameplay captures
were retained. The selected Rider source frame was generation 60 with 8817
source triangles. All eight racers, including the followed Dud, used original
AI, with zero human, keyboard, controller, or host-driver samples.

The GPU-completed Windows swap baseline was 13.5681771972 FPS: 68 swaps over
5.011727 seconds, after 5.0588333 seconds of excluded warmup. Native ticks
advanced 275 to 547. Resolution 640x448, swap interval 0, MSAA 0. This measures
actual rendered gameplay submitted through SDL, not physical monitor scanout.
No new Linux result or full-race completion is claimed.

Dud's character-selection framing is visibly imperfect and remains an open
visual review item. A successful basic smoke does not establish hardware pixel
parity. Dud/GA is a different workload from historical Occo/SA and cannot
isolate the effect of the pending performance changes.

## Preparation and recovery

Authenticated GA intake, visual/texture/sky/ATM preparation, and seven additional
Rider kind2 sources (35 mips) completed inside the D: repository. The additional
catalog was integrated at 68c157d. The earlier 0fe381e build passed but its smoke
stopped at an unprepared Rider neighbor; no gameplay or FPS was produced by
that failed attempt. Source-neighbor admission was preserved; the real missing
resources were prepared rather than bypassing the unavailable selection.

## Pending capture-readiness refinement

Source review found that the capture gate accepted the first published character
model while its authored entrance animation could still be running. The session
snapshot now exposes the actual selected/attached character readiness, and the
capture harness waits for that observation before taking the Rider screenshot.
The receipt records `selected_character_ready`. This changes only evidence
capture timing; original menu input admission and animation playback are unchanged.
It does not establish the cause of the visible framing issue or prove it fixed.
No build, syntax check, or smoke has run for this refinement yet.
