# Reuse UVs for adjacent identical pixel-metric glyphs

UNTESTED. No build, syntax check, test, smoke, benchmark or gameplay executed.

Within layoutOriginalUiTextDimensions50LA, the first pixel-metric glyph and every
changed ASCII still execute each original UV multiply and division in the same
order. An adjacent equal ASCII copies u0/v0 from the prior successful quad's
vertex 0, u1 from vertex 1, and v1 from vertex 2. No new cache storage is required.
The prior quad exists only after its common finite and geometry checks succeeded.
The const font and atlas dimensions are unchanged throughout the fixed-storage,
serial callback-free loop. Float copies retain signed-zero results.

Every current glyph lookup, mixed-metric check, extent check, pen update, position,
measurement and common finite/geometry check remains. No glyph corners are
translated or inferred; positions always run the original arithmetic. The legacy
UV-only branch is unchanged. First/empty/invalid runs preserve their failure and
transactional output behavior. This relies on the ordinary serial host FP contract,
not externally installed per-operation trap instrumentation.

Reachability evidence: TimeStats time189000 emits repeated '--' pairs for sentinel
times and '00' minute digits early in a race. The owner binds the authenticated
eurostileboldbold13ingame font and passes its metrics to layout. Historical raw-CEL
metadata in time-stats-native-joins-20261015.md records 46 glyphs and 46 metric records,
including these characters. This is source/asset evidence, not a new runtime test.

Two independent source reviews accepted the design. A repeat avoids four UV
multiply/divides but adds an ASCII comparison and float loads. Actual frequency,
compiler effects and FPS are unmeasured. Successor should compare exact UV/member
values for repeated/different glyphs, sparse fonts, both metric modes, signed-zero
UVs and rejection cases before timing comparable HUD updates.
