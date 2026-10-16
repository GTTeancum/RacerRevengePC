# Native race HUD countdown presentation domain

Source-only correction based on `1c7eb1d2c450914c3c08a631de5a01753c844928`.
No build, test, lint, syntax check, sanitizer, guest execution, runtime decoding,
or gameplay was performed for this correction.

## Existing Windows observation

The root supplied the read-only receipt for the October 13, 2026 run at
04:49:35–04:49:56 UTC, following the successful 04:49:00–04:49:22 build. It
followed actual Main → Rider19 → Course → TA in the native window, with all eight
racers using original AI, including the followed racer. The run exited 1 at
`native race HUD counters/status outside admitted bounds`, with GL=0,
cleanup=0 and component=5. The failing HUD payload was not logged, so the exact
predicate responsible for that specific failure remains unobserved.

Earlier receipt lines establish that the first race state was valid:

- Line 2416: phase=1, countdown=3, seconds=0, player_place=8/8,
  presentation_valid=1, and eight standings rows.
- Line 2417: player_finished=0, CD8=0, native_ticks=1, sealed_laps=0,
  host_results_ready=0, and all racers had zero live laps.
- Line 2406: 219 render commits and window swaps; track_draws=78/78/0,
  hud_draws=0. Thus the renderer had already drawn the intro track. The later
  failure does not establish that no track image ever rendered.

No settled-track screenshot or baseline was accepted. The new correction has
not been exercised; the existing test-window restriction remains in force.

## Source and lifecycle evidence

`beginOriginalRaceCountdown1AFD80` in
`src/game/original_camera_start_effects.cpp` preserves the ordinary application
branch's binary32 constant `0x40A66666`, approximately 5.2 seconds. The alternate
application branch starts at approximately 0.2 seconds. This matches the
previously authenticated bounded original-source inspection of 1AFD94–1AFDB8.
The inspected source ELF identity was SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`;
the retained countdown evidence identity is
`484ad62a2eae6d4af57a8687b44c13ca09621b2e9c73b705b0bce2791541131f`.
No original instruction bytes are included here.

`Session::advanceExit` in `tests/native/original_camera_session50kx.h` copies
that actual duration into the existing, sole advancing race countdown when the
intro exit completes. It does not start a second clock. `advanceNativeStartGate`
then advances that owner and publishes GO through the original manager flags.
`race_session50HM::countdownNumber` returns the ceiling of its remaining time;
`terrain_interactive_drive50du.cpp` copies this value unchanged into the HUD.
The ordinary 5.2-second start therefore legitimately yields a display value of
6, followed by 5 and 4 before entering the older admitted 3..0 range.

The source UI visibility flag withholds the host HUD during the intro; the
renderer receives it when `nativeHudVisible()` allows it. This explains why
valid countdown=3 diagnostics and actual intro track draws can precede the
later domain mismatch. The source-derived mismatch is established statically;
its identification as the exact recorded failure is a strong inference.

Rank is not repaired or substituted. Cold activation already calls
`rankOriginalAiFleet` before `publishOriginalAiStandings`, and the initial
receipt reports the genuine place 8. No evidence supports a zero-rank fix.

## Bounded correction and rendering safety

Only `platform/linux/native/vehicle_window_gl50ds.cpp` changes. The existing
countdown presentation upper bound becomes 6, derived from ceil of the
ordinary source duration. All lap, rank, field-count, status-exclusion, timing,
standings and result guards remain in place. No countdown is shortened,
clamped, fabricated, disabled or fed back into original state.

`HostHudGeometry::build` prints `START IN %u` into a 32-byte buffer. The existing
bitmap glyph switch supports all digits, including 4, 5 and 6. Its text path
uses bounded geometry emission; there is no three-element countdown table.
These are still single-digit labels of the same width. `drawHostHud` uploads
only the accepted bounded geometry. This remains the declared native host
status presentation, not a reconstruction of original numeric countdown art.

A failure-only stderr record now includes lap, target, place, racer count,
countdown, GO, finish and retirement fields before returning the unchanged
public diagnostic error. No public struct, header, or diagnostic ABI changes.

## Rebuild scope and remaining verification

The Windows source inventory contains this translation unit at zero-based
index 176, producing `obj/0176-vehicle_window_gl50ds.obj` under the documented
builder. This correction requires that single object and relinking, subject
to normal source/input identity checks; it introduces no header fanout.

The next authorized Windows run must confirm the intro-to-countdown transition,
actual track capture/baseline, and absence of a new source boundary. A passing
build alone would not establish those outcomes. No new run is authorized by
this document.
