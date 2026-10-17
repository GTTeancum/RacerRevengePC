# Race HUD per-view viewport scale

Status: UNTESTED source change. Baseline de99212b, 2026-10-17.

src/game/original_race_hud50kg.cpp now computes renderer/viewport X and Y scale
once per view in advanceAnimations29CDA0, passing the results into its private
advanceText helper. Exact float casts/divisions, X-before-Y order, each text's
scale assignments and all subsequent animation writes remain. Existing all-view
preflight runs before any computation or mutation, so invalid dt or a bad later
view still returns without updating the earlier view.

construct1A8CC4 binds all eleven texts of a view to the same viewport pointer.
There is no subsequent writer to those pointers in the implementation; public
text exposure is const. Renderer dimensions belong to the same retained storage.
advanceText and stop have no allocation, callback, viewport mutation, FP-mode
change or exception-flag read. Under the ordinary serial native FP contract,
repeating identical divisions does not add different sticky exceptions after the
first occurrence. This does not promise equivalence for external FP-trap
instrumentation or unsupported concurrent mutation. Viewport resize is observed
again each invocation; no cross-frame scale cache is introduced.

The active native race loop in tests/native/terrain_interactive_drive50du.cpp
calls advanceAnimations29CDA0(.016f) when sourceRaceHud.loaded() and
playerAdvancing are true, immediately before the physics phase. Source divisions
fall from 22 to 2 per view/update. Compilers may already hoist some computation;
no measured saving or FPS gain is claimed.

Successor validation: build, compare single/two-view animation snapshots and
viewport resize, invalid dt and invalid second-view nonpublication; verify
original text, blink, alpha, scale, rotation and gleam timing before measuring
under an equivalent renderer/workload. No execution checks ran for this change.
