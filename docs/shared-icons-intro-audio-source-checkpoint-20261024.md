# v67 to v69: compiled source and bounded GUI129 regression

This local packet contains exactly 26 source changes: 23 modified files and three additions, with no deletions. Only files/ contents are proposed publication payload. No original game asset bytes, executable, build output, runtime log, screenshot or private local metadata is included in the proposed source payload. The src/assets/ paths are C++ source, not bundled game assets.

## What changed

- Private SaveLoad/MemoryCard groundwork retains the actual Options archive owner, validates the original memcard page, admits first-X/NoTexture resources through existing domain scans, captures construction-family provenance, and extends retirement guards. The Options source-custody handoff and first-icon population entry have declarations and definitions but no call sites in this source snapshot. This does not activate page438, storage, Save/Load behavior, or a complete MemoryCard screen.
- Shared material/texture handling preserves the first loaded icon construction family and the captured texture winner. Existing Garage quad creation skips new missing-PSM selection for already loaded shared icon rows; the existing collector borrows the retained captured texture; canonical submission admits and checks MemoryCard fallback provenance as well as Garage provenance. These are changes to existing active infrastructure, not solely dormant candidate code. The bounded GUI129 Garage cancel/reentry/unchanged-Accept regression exercises existing Garage presentation; it does not exercise the private MemoryCard-origin activation.
- Private TA intro authoring and cold Sound membership preserve original metadata and actual Application aural-manager ownership, with constructor/clear identity checks. The TA constructor has no call site in this source snapshot. This does not activate Tournament/TA playback, intro presentation, an audible stream, or a completed Tournament flow.

## Evidence and limits

The local terminal receipt records a successful v69 build: 496 translation units, 158 fresh, 338 reused, 925.860666 seconds. Binary SHA-256: ca23e45b52c452fa9678695221d34e1c527750ed16b7e1a4d78ba9a54e97d023. No build was rerun for packet preparation.

GUI128 is historical and incomplete: input-blocked timeout, not a pass. GUI129 completed with exit code 0, timeout=false, elapsed 1192.485255 seconds, and generated_root_absent=true. Parent visual testing qualifies only the bounded paths below:

- Main → Tzidik → Citadel; short W5 drive, Pause → Main.
- Controller Normal2 preview cancelled; Normal1 retained on reentry; unchanged Accept. Missing-controller-2 popup dismissed.
- SaveLoad panel Back without storage IO.
- Garage Cancel → Career → Garage reentry with six stats → unchanged Accept → Escape during closing → Career → Back to Rider → Back to Main → Alt-F4.

Read-only log review independently corroborates two frontend closures with cleanup_complete=true, the 300-tick development drive, completed laps=0, marker cursor=0, callbacks=0, and screen activations 13 → 9 → 13 → 9 → 5 → 4 (Garage/Career/Rider/Main mapping from parent context). Final frontend state records 29 input events, 16,117 updates/renders/submissions, zero unavailable destinations, and audio_failed=0. The launcher identifies the same v69 binary.

Qualification caveats: final controls_accepts=0 and controls_saved_presets=[0,0]; logs contain no named Controller Accept or Garage Cancel/Accept event receipts. The exact button outcomes above are parent visual observations, not a claim of controller-persistence counter advancement. An intermediate Garage custody diagnostic reports status=15/source_pc=1927212 before later stages succeed; it is retained as a diagnostic, not hidden or treated as a terminal failure. Audio device status is not_requested and hardware_audio_verified=false. Existing frontend world_ticks=0 is not the race count: the separate drive receipt records 300 ticks.

This is a bounded existing-flow regression pass, not private SaveLoad/MemoryCard or TA activation, storage, purchase, Restart, a completed lap/race, Results, Tournament completion, hardware audio, or a full-game pass. Screenshot files are pinned but were not independently visually reviewed by the packet preparer.

Two new TA files retain stale source comments saying they were not compiled. Those comments describe their earlier proposal status, not this integrated build: race_audio_groups50ic.h includes both the authoring header and the inline implementation. Exact compiled candidate source bytes are preserved without comment edits. Private/unactivated and not runtime-proven remain accurate.

## Publication gate

This packet is locally verified for the qualified bounded regression, not authorization to publish. Parent alone may publish after remote verification. Recheck remote main and the complete base tree, preserve remote-only files and history, reconcile each source blob against the expected v67 baseline, and independently calculate the merged tree. Do not force-update, delete files, or silently rebase over changed main. GUI129 evidence qualifies only the specified existing flows; private SaveLoad/TA functionality must not be called tested by this pass. No external write or upload occurred in packet preparation.
