# Windows recovery: 1107caa failure, bdb9d80 pass

1107caa failed its Release build at 2026-10-15 10:52:12.317420–10:52:27.897711 UTC, exit 1. MSVC C2082 reported that the private QueryBridge parameter world conflicted with the existing local world alias. The smoke was not launched. Cleanup confirmed zero owned processes.

Root fixed only the parameter and its SceneInfo call to sourceWorld. The existing local alias and constructor effects remain unchanged.

Recovery source: bdb9d80da8657fe0b4ce2c30ce2b2bdaf8047e2a
Tree: 18baa591aec6f67b09bc48487bc3c91bb93e4934
Immediate extra build/smoke was centrally assigned under the user's failure-recovery exception.

Build: 10:57:08.073614–10:59:41.131971 UTC, exit 0.
Visible Gasgano/Sullust smoke: 11:00:01.277446–11:00:54.723865 UTC, natural exit 0.
Both cleanup receipts confirm zero owned processes. Native EXE ran from the original asset directory beside SLUS; SLUS was never executed.

Actual baseline: 45 GPU-completed swaps / 5.0961407 seconds = 8.83021145785869 FPS.
All eight racers used original AI; zero human/controller/keyboard/host-driver activity.
Root independently inspected the four actual Main/Rider/Course/Track screenshots, both original TimeStats/SpeedBoost completion logs, source/build/smoke receipts and evidence ZIP hash. The screens and original HUDs are intact without overlap.

RA was not invoked and its crowd cache was not used. The new inactive RA code compiled, but no RA construction or gameplay success is claimed. Performance remains low. No Linux game/build/smoke test occurred.

EXE SHA256: b331ddd865592b538953ef576eeb31b9148d6605729cbbf19a2a4ce87b0bb406
Evidence Library ID: libfile_4edee2ed79548191a01b37b243fdb36a
Evidence bytes: 1606372
Evidence SHA256: 3ec174ef812a6cd9e55574b8afbb30c42a654f23bead2bcb451f3a9fc97529d0
Failure evidence Library ID: libfile_2ca3994863e88191a2db20fef1adf423

Recovery cleared only after both build and real-window smoke passed.
Next routine centralized test eligibility: 2026-10-15 12:00:54.723865 UTC.
