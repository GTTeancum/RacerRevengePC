# Completed 47W reference experiment

Reviewed 2026-10-03; original execution timestamps remain 2026-09-30. This records
completion of work that was still running when checkpoint 47W was frozen. It
does not modify that immutable checkpoint or promote the compatibility runtime
to the native game architecture.

Both full runs stopped at VBlank 7000 with exit 0. The `7000-0-state.json` and
`7000-1-state.json` reports in `evidence/47W` report equivalent EE/IOP RAM,
terminal state, stop and cards. Their explicit PCM and final-image checks also
pass. The frozen executable SHA256 is
`f6da2e716339a2e3a7bfb2563f389c6efa9b9c97c12d803da966ef84c4eb6724`.

| Mode | Run under work/ |
|---|---|
| Diagnostic cache off | `native-run47w-0-1-1-1-1-0-1-1-1-1-20260930-180935-820` |
| Diagnostic cache on | `native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483` |

The timing report requests VBlanks 6000–6900, but the on-mode's last sampled
capture is VBlank 6877. Its `window` result is absent. A successful stop at 7000
does not supply a time-stamped picture bracketing 6900. Therefore there is **no
accepted matched-window speed ratio for 47W**. The report's
`game_behavior_equivalence_verified: false` means that timing report alone does
not establish equivalence; the separate saved-state and media reports do.
Do not extrapolate the missing endpoint or present total execution time as a
matched race FPS measurement.

The CPU/IO observers ended with process exit 0. Their files are preserved as raw
evidence, not a new cost attribution claim. No native benchmark or compiler
process was active during the 2026-10-03 planning review. No tests were rerun.

Rechecked checkpoint 47W manifest records:

- Source archive SHA256: `77a3b36a2c101728c7665fa8a60227b4a1b6508e9e3c8f14e9cff4580baaeb62`.
- Evidence archive SHA256: `2ed33081ba95d62307bb779d976e3dbf0203a9d1563fee2ca5904b685e5d652b`.
- Recorded member verification completed at `2026-09-30T22:12:38.697279+00:00`.

The native-PC planning checkpoint adds the completed full-run evidence to the
recoverable chain. Existing 47W remains the source/build/startup recovery base.
No completed lap, native game performance or Xbox gameplay acceptance follows
from these reference results.
