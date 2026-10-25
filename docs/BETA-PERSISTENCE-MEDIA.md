# Beta persistence, Options and authored TA introduction batch

Status: requested code batch integrated and frozen before compilation. No build or runtime verification of this batch yet.

## Implemented scope

- Native versioned profile and independently accepted settings files, with bounded validation, atomic replacement, process locks and explicit failure/durability status. Original disc assets and original memory-card files are not modified.
- Normal Memory Card page exposes explicitly named native beta profile Save and confirmed Load; unsaved state is retained on failure or Cancel. Accepted settings restore at startup; staged/Cancel settings do not persist.
- Actual Tournament launch arms the genuine manager; an actual ended-manager transaction applies the recovered result rules exactly once and saves according to the live Auto-save flag. Restart/Main teardown cancels only the matching pending manager, never earned profile data. No synthetic results or finish tests.
- Resumed careers use real progression, remaining prizes, medals and credits rather than fresh-zero gates. Unknown non-TA PAR annotations are omitted, not invented. Completed-career presentation and bonus rider-award lifecycle are not added.
- Earned cash no longer prevents unchanged Garage viewing/Back/Accept. Purchases remain explicitly unavailable; funded upgrades do not falsely report insufficient funds or mutate unowned transactions.
- Accepted vibration setting feeds the existing physical-controller feedback backend.
- Authenticated three-cut TA Career introduction, genuine same-Application race/world/controller and audio ownership, original cue/music data, skip/natural completion/fade and ordered retirement. This is not MPEG/FMV playback or a claim of full original World-loader parity.
- Audio effect preflight preserves prior playback when a replacement cannot be admitted; Game Options Cancel restores its actual source audio operation.

## Deliberate verification boundary

Complete and integrate the full source batch before compilation and one basic human-navigation smoke. No per-patch runtime checks, racer/track combination sweep, lap completion, finish or Results testing. Progression earnings and broader functionality remain tester-verified. Same-run selected-racer and in-race evidence are required for every GUI run.

See NATIVE-BETA-PROFILE.md for file locations, controls and failure behavior. Previous build100 evidence applies only to its unchanged source, not this batch. GitHub main remains ccc051a099919a67b0af1e35bb8b234be2d5918c until a later explicit publication receipt.
