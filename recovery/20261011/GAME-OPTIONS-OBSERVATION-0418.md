# Original Game Options Linux observation — 2026-10-11

Centralized focused check started 04:18:42.970824 UTC against local commit4801bf52d0a4e5f5954265c0e27451dc7a02409d and finished 04:21:03 UTC. Next permitted check is no earlier than05:19:42.970824 UTC.

## Fresh observed result

- Native build passed:33 newly compiled sources,244 dependency/object-hash verified reused objects,277 total; link exit0.
- Executable SHA256:80da8674a09fadf633623b901ab5216328dd946abc81ac702fc4003826726532.
- Actual original Main→Options→Game Options→Options→Main flow passed. All20 key presses and separate release barriers,21 open checkpoints, requested closing/opening transitions and stable final Main tail were observed.
- Cancel discarded edited sound mode/SFX volume on re-entry. Accept committed source sound mode2 and SFX40, preserved all other settings, advanced the actual application sound owner from revision1 to2, and those values remained on re-entry and return to Main.
- Real enabled original UI mixer/output produced596870 stereo16-bit frames at48kHz, matching all source/output/captured sample receipts. Finalized WAV SHA256:4a68f6c1357d195f761a7094818df140e4d1dcb8895f8227a21d966b3d91e539.
- Native process exited0 through explicit quit; cleanup complete; no forced termination, pending capture, protocol/storage errors or physical audio device use. World ticks remained0. No driving, Rider activation or gameplay check occurred.

## Capture and presentation

327 real640×448 frames were retained across12.097293232 seconds of capture acknowledgments:26.948177Hz measured, maximum gap112.356ms. This is neither sustained30fps proof nor a native-display frame-rate claim. Native renderer reported663 frames over the complete observation.

Delivered video contains all327 captured frames with original recorded UI audio. It uses each frame's actual captured PCM sample count for timestamps, trimming audio before the first captured image (sample14920). Final sample596870. Video duration12.123854s; audio12.124s. No interpolation, synthetic frame generation, or inserted gameplay. Source-clock timing differs from wall acknowledgment timing and does not include excluded cold-load intervals as simulated time. Encoding and inspection used retained media only, without another native run.

Visual inspection of the retained Game Options image shows original background, game typography, setting values and Defaults/Cancel/Accept prompts. This focused observation does not establish pixel-perfect original hardware parity, audible quality, disk-save persistence, external rumble/split-screen consumers, other menus, all racers, Rider rendering or completed gameplay.

Historical Windows results are separate from this fresh Linux result. The03:17 compile failure remains recorded in GAME-OPTIONS-BUILD-0317.md and was not relabeled as a pass.
