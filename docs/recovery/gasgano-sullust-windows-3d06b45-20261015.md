# Windows Gasgano/Sullust smoke: 3d06b45

Fresh Windows results on 2026-10-15. No Linux software test ran.

- Source:3d06b45100dc10bc3a30ebdf23b283629ecdbc53
- Tree:dbd9968476fbe973dc9cbf43ade75057726cfa8d
- EXE SHA256:15738e6e982019e49c2a5e1591fe5be2f45277cbf15abd7b6fdf3409d77cb912
- Build:02:47:59.932997–02:50:28.623044 UTC,358/358,exit0.
- Visible smoke:02:50:55.689419–02:51:51.108048 UTC,natural exit0.
- Both cleanup receipts confirm zero remaining owned processes.
- Native EXE ran from original disc directory beside SLUS; SLUS was not executed.
- Main menu, Gasgano character selection, Soro Suub Facility course selection and actual race captures inspected by coordinator and root.
- Eight original AI racers including followed Gasgano7/slot0. Zero human count, controller samples, keyboard packets and host-driver calls.
-640x448,swap0,original fog/blur/shadows,culling,logical audio with muted output. Existing45-file Gasgano cache unchanged.

## Baseline

46 actual completed swaps in5.0820943seconds =9.051386551406573FPS.
Native ticks160–344, swaps270–316. Capture/readback intervals discarded; warmup excluded.
Previous da7ca23 baseline was8.472151172179252FPS; previous602643e was8.970563454845463FPS. These are separate bounded observations; no causal speedup is established. Performance remains poor.

New optional display components identify a dominant finish-wait inside completion_wait. Final residual component batch has46calls, history-stage365700ns, finish-wait2374187300ns, host-swap4536700ns. This batch is nested and explicitly baseline_aligned=0. Matching call count does not establish matching time interval. Do not add nested component times to their enclosing phase or report them as GPU-only measurements. Preserve glFinish/history ownership until a valid replacement is established.

## Evidence

Library:libfile_d7d78e0e9ad88191aaacd2743569b96a
ZIP1647675bytes,SHA256e054df8debe88e21c3c3d5d794c84cbe68c1a6330266aee465314f6a1f6cb86e.
Four root-delivered PNGs:
- main:libfile_b5480f403bf08191819121c4ffede40f
- rider:libfile_c2c09ec467988191b37d16a3b6e2c2d4
- course:libfile_43029fb3c17881919e17aca5ef77bc07
- track:libfile_76c48c1de6788191bfb151032c0e9b9b

No failure recovery is active. Next routine centralized test cannot start before2026-10-15T03:51:51.108048+00:00. Codex feature helpers remain stopped; remaining Codex budget is reserved for Windows tests. Default-off Clegg/complete-preview preparation passing this route does not establish those disabled features' runtime readiness. The separately researched original TimeStats HUD is not activated by this build.
