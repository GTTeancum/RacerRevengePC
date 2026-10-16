# PositionStats native cold preload

Source-only integration; no compilation, tests, syntax checks, sanitizers or gameplay were run.

`tests/native/original_position_stats_prestage50la.h` owns four genuine canonical ScreenMT references, authenticated OVERLAYS backing, the real Application font entry, and the actual construction context/domain. It has no PositionStats Entity or display identity and no GPU capability. It does not relax any material table or display guards. The table grants access solely to this concrete owner for the existing guarded retain/release operations.

## Exact 50DU integration

1. Include `original_position_stats_prestage50la.h`.
2. BEFORE `auto raRosterOwners=...` (and therefore before `nativeRosterOwners`, `raWorld`, `racerBank`, renderer, full PositionStats resources), declare:

   ```cpp
   original_position_stats_prestage50LA::Owner sourcePositionStatsPrestage;
   original_position_stats_prestage50LA::Diagnostic sourcePositionStatsPrestageDiagnostic;
   ```

   This declaration order is essential. Destructor release must occur after both the RA Bank and aggregate retire. Do not close this owner inside the current closeWindow while the aggregate still retains a collector.

3. In the RA cold block, after `stageTimeStatsNativeCold50LA(..., true)` and BEFORE `mountRetainedRuntimeRosterArchives` / `raSession->construct`, insert:

   ```cpp
   const bool positionPreloaded=sourcePositionStatsPrestage.stage(originalMainApplication,
       originalMainApplication.assetRoot,raMaterialContext,sourcePositionStatsPrestageDiagnostic);
   require50DU(positionPreloaded,
       sourcePositionStatsPrestageDiagnostic.failure ? sourcePositionStatsPrestageDiagnostic.failure :
       "RA PositionStats native source preload rejected");
   ```


4. After RA construct's final canonical population, `verify(diagnostic)` is a read-only check of the actual current collector, retained rows, original source bytes and font provenance. It never rebuilds a collector.
5. Stage full `Application::stagePositionStatsResources50LA` ONLY after the actual renderer has been created, through its unchanged original fixed-resource and primitive leases. Before that call, copy the domain's collector receipt and provider revision. After the call require that this exact copied receipt remains current and the provider remains sealed at that same revision. Also call preload `verify`. Do not call `domain.populate` at this point to hide a changed/stale RA receipt.
6. Bind the HUD only after full stage completes and preservation checks pass. On failure retain the actual full/partial owner for its original drain/retirement path. Preload destruction is not a substitute for any of those drains.

## Non-RA use

If activating the same full HUD on other tracks, stage this owner after Speed/Boost staging but BEFORE the non-deferred TimeStats stage that produces the final shared collector. Supply the actual Video-minted construction context. Do not stage a second time within a session. No non-RA root hook is included in this patch.

## Why the final RA receipt can survive

- `OriginalStandardArchiveProvider50LA::commit` checks matching archive name, digest, byte count and every member name/offset/byte. An identical mount returns before changing revision or sealed state.
- `appendOriginalSources` shares identical material sources; conflicting bytes fail. Identical texture source append preserves existing objects.
- `retainScreen14D590` on an already loaded row changes only real refcount/template association. It does not invalidate the collector.
- `populate14D370` invalidates on a newly published pass object. All four required objects are populated by this preload before RA's final combined pass. Later original per-texture population calls find them loaded.
- Existing font entry loading preserves its actual handle and owner. Full PositionStats can reuse the same real font13 entry.

## Failure and cleanup

Each stage is single-attempt. Mount/source append or population effects are never rolled back by fabricating old revisions. Acquired rows remain held on partial failure. Startup must abort rather than advance RA/HUD after a failed stage. The destructor performs reverse-order guarded `release14D500`; it terminates rather than discard a remaining pinned reference on failure. Explicit `close` is retryable and leaves remaining references intact after rejection, but must be ordered after all full-HUD drains, renderer borrowers and RA collector consumers. Since the root owner precedes all those locals, normal/exception destruction gives that order, including failures before closeWindow is installed. The process-owned canonical source corpus and font registry remain intact for serial sessions; native GL ownership is never carried over.

## Central recovery correction (2026-10-16 12:31 UTC)

The first combined smoke exposed premature collector publication: RA archive,
roster and particle cold construction correctly reject an already current final
collector. The preload now authenticates/appends/retains/loads font references
only. RA's existing final whole-table population resolves these positive-ref
HUD rows together with world and racer rows. Root verifies afterward, then later
constructs full PositionStats against the real created renderer. Earlier prose
above describing a separate preload population is superseded by this ordering.
No current-collector guard is removed and no stale receipt is rebuilt to hide it.
