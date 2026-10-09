# 50FF runtime bundle and run instructions

This package contains the supplied native development-client binary and its
source-inspected runtime asset closure. It is a development build, not the full
commercial game. The package manifest identifies the actual binary format and
records every included file's size and SHA-256. Packaging does not establish
platform compatibility, successful gameplay, or a frame-rate result. Consult
any included `validation/` receipts for what was actually tested. Supplied JPEG
captures are under `validation/screenshots/` so they can be opened independently
of chat image previews.

## Run the supplied client

1. Extract the complete archive into a new directory. Preserve the directory
   names and capitalization. Do not copy just the executable elsewhere.
2. Open a terminal in the extracted `RacerRevenge50FF` directory.
3. Verify the files without launching: `python3 run50ff.py --verify-only`.
4. On a compatible host with its runtime dependencies, run `python3 run50ff.py`.
   The launcher verifies hashes, selects the supplied executable and corpus, and
   sets the correct working directory. If a ZIP extractor lost execute permission
   on Unix, use `chmod +x bin/<actual-binary-name>` before trying again.

The current 50DU host source is Linux-specific. Its window mode dynamically
loads SDL2 (`libSDL2-2.0.so.0`, or `libSDL2.so`) and requires a graphical session
with desktop OpenGL 3.2+ core support. The binary also needs its compiler/OS
runtime and zlib dependencies. None of those system libraries is bundled or
installed by these tools. The release acceptance target is the coordinating
task's Linux VM. Other environments require independent verification. A Linux ELF
binary does not run directly as a Windows program.

### Keyboard controls

| Control | Action |
|---|---|
| Hold W | Left-engine throttle |
| Hold Up arrow | Right-engine throttle |
| Hold W + Up | Both engines forward |
| Hold S | Left-engine brake |
| Hold Down arrow | Right-engine brake |
| Hold S + Down | Both-engine braking |
| Unequal left/right throttle or braking | Differential steering |
| Hold Space | Native boost, subject to drive, repair and heat conditions |
| Hold Left Shift | Native powerslide action (source conditions still apply) |
| Hold R | Repair, subject to native damage/repair conditions |
| Backspace | Reset to the current genuine checkpoint, keeping navigation and damage |
| F5 | Restart the complete development session |
| Escape or close window | Quit |

These are explicit development-host bindings. Letter keys use SDL physical
scancodes. Opposing throttle/brake keys for one engine center that engine's axis.
Focus loss or minimization releases held keys and pauses the simulation. The
initial window is 640 × 480; oversized drawables beyond 1280 × 720 pause.

Space maps to the selected native preset's boost action. Start driving with
W + Up, then hold Space to boost; pressing Space at rest without positive engine
drive does not initially activate boost. The native controller manages heating,
cooling, boost duration and overheat. Holding R suppresses the boost request.
Release Space to stop requesting boost and let heat cool; overheating can damage
the engines. This development topology has no boost audio/control owner.

For the separately selected terminal-driven EGL mode:

`python3 run50ff.py --headless-live capture.ppm`

Keep stdin open. Commands include `down W`, `down Up`, `up W`, `up Up`, `down S`,
`down Down`, `down Space`, `down Shift`, `up Space`, `down R`, corresponding `up` commands,
`release`, `capture`, `checkpoint`, `restart`, and `quit`. Commands are newline-delimited; EOF quits. A capture is a
committed renderer image. This mode has no native window and is not evidence
that desktop keyboard input or window presentation worked. Its host needs EGL
and a compatible desktop OpenGL implementation. Captures are written relative
to the package root unless an absolute output path is supplied.

### Live HUD and checkpoint recovery

The small development HUD shows live HEAT, left/right engine damage and
IDLE/BOOST/OVERHEAT state inside the committed game frame. It is host-authored,
not a recreation of the original game's HUD.

The host supports two automatic checkpoint recovery paths:

- When the native undertrack guard is reached after falling below the track,
  the host rebuilds the vehicle session at a source-derived checkpoint, preserving
  navigation progress and the existing damage values.
- When the native damage decision reaches a supported, validated wreck or
  detachment transition, the host rebuilds the session with a fresh, undamaged
  vehicle at a source-derived checkpoint, preserving navigation progress.

Both paths require an actual terrain query to establish a grounded checkpoint
before recovery proceeds. They are development-host recovery policies: the
interrupted native frame is not resumed, native guards remain intact, and the
original destruction/detachment visuals are not complete. Other unsupported
native paths are not covered by these recovery policies.

If a checkpoint cannot be prepared or grounded, the client pauses on its last
committed image. Press F5 (or send `restart`) to return to the authored starting
grid, or quit. F5 always starts a complete new session; it does not restore the
current checkpoint.

## What is included and why

The original asset selection and export report remain byte-for-byte intact,
including their existing references to unneeded objects. The packager includes
only the file-opening closure for this client, rather than every object mentioned
by those provenance documents:

- The retained collision corpus with the two selected authored placement cases.
  It is preserved whole, without rewriting oracle data.
- The original `POD01.RES` and `TA.RES`, needed for actual cold scene, material,
  collision, spline and source-texture-member reads.
- The Pod01 physics/tuning files, 12 PSG/VUT/MOT inputs, three cold trigonometric
  tables, the source pod shape, and six effect authoring files.
- The 3 Pod and 228 TA scene records/geometry files, material records and actual
  pass-1/slot-1 largest-retained texture mips used by the current readers.
- Eight exact 50FC source-decoded texture files, their pinned hashes, and the
  original conversion manifest/proof. The two ATM resources use original frame
  zero; the package does not supply an animation controller for them.
- The 50FD cable owner extracts its material and PST from the included Pod01
  archive. The source-derived 50FC cable decoder converts those bytes in memory;
  it requires no additional loose asset or downloaded codec. This adds one
  runtime-decoded cable texture to the 34 packaged Pod/track texture files.
- Exact retained upstream JSON/zlib notices and supplied validation receipts.

No original executable-memory image, unrelated historical checkpoint archive,
driver, emulator, system library, or asset downloader is included. Asset files
remain subject to their original owners' rights. This private handoff grants no
new right to redistribute game assets. Upstream notices apply to their respective
components; they are not a license for the game's assets or the whole project.

The current native physics scope and full-game limitations are unchanged by
packaging. A complete asset inventory is not proof that every gameplay path is
implemented or that every machine can sustain a particular frame rate.

## Reproduce the package without building or launching

From the project root, using Python 3.8+ and only its standard library:

```sh
# Inventory and authenticate the selected runtime files. Output must be new.
python3 tools/package_playable50ff.py --plan /tmp/runtime50ff-plan.json

# After the coordinating task supplies a completed binary:
python3 tools/package_playable50ff.py \
  --executable /absolute/path/to/completed-client \
  --stage /absolute/path/to/new-runtime50ff \
  --evidence /absolute/path/to/build-receipt.json \
  --evidence /absolute/path/to/validation-receipt.json \
  --screenshot /absolute/path/to/start.jpg \
  --screenshot /absolute/path/to/driving.jpg \
  --screenshot /absolute/path/to/braking-repair.jpg \
  --zip /absolute/path/to/new-runtime50ff.zip

# Static integrity check only; this never runs the client.
python3 tools/package_playable50ff.py --verify /absolute/path/to/new-runtime50ff
```

`--evidence`, `--screenshot` and `--zip` are optional. Evidence and screenshots are
copied exactly, never interpreted as a successful test or re-encoded. Screenshots
must be regular, non-symlinked `.jpg`/`.jpeg` files with JPEG start/end markers;
the coordinating task remains responsible for visual validation of the captures.
The tool refuses existing output directories/archives,
symlink inputs, missing assets, changed pinned assets, unsafe paths, and excluded
checkpoint inputs. No files are deleted. A failed copy can leave an incomplete
new stage directory; retain it for diagnosis and choose a new output path.

Identical input bytes, executable name, receipts and packager version produce
the same manifest and same ZIP with a fixed member timestamp/order/permissions
under the same Python/zlib compression implementation. Compression bytes can
differ across zlib versions. Source reader hashes record the inspected revision;
they do not prove that an arbitrary supplied binary was built from those sources.
Pair the binary with its actual build receipt. The packager rejects non-ELF/non-PE
inputs to catch accidental source/log files; header recognition is not a runtime
compatibility test.

## Required release check

Before describing this as a self-contained runnable handoff, the coordinating
task must start the final executable with the staged directory as its working
directory, using only its included files, and record the result. The packager
does not perform that launch and does not use syscall tracing. The source-derived
closure should be reviewed again if any cold reader or selected display policy
changes. A sealed 49W source handoff is neither replaced nor modified by staging;
it is not required by the intended runtime closure.

### Working-directory and source-path audit

The cable-enabled 50DU quoted-include closure was source-inspected (245 files),
along with native asset/game/Linux host sources. No `__FILE__`, `__BASE_FILE__`,
`__builtin_FILE`, `chdir` or `current_path` based asset lookup was found. The cold
49W fixture constructs 49T assets and its cold source machine with `"."` as root.
Its PSG/VUT/MOT and trigonometric files consequently resolve beneath the process
working directory. The other active cold readers use the relative `evidence/`
and `work/` paths inventoried in the manifest. `std::filesystem::canonical` in
50ED/50FA canonicalizes the supplied relative `evidence/visual49G` directory; it
does not discover or fall back to the source checkout. `native_asset48b.cpp`
opens the filename its caller supplied without a secondary lookup.

The launcher changes the working directory and `PWD` to the extracted package
root, supplies its fixed relative corpus path, and rejects symlinked files or
parent directories within the asset tree before executing. It does not search
the original checkout or use a remembered build path. This is a source audit,
not an OS filesystem sandbox or syscall trace. Normal system/graphics libraries
remain external runtime dependencies. A changed binary or reader must be audited
again; source debug strings in a binary alone are not proof of a file dependency.

The optional HOST/NEXT arrow uses the live native map target to show the direction of the next gate. It is a host driving aid and does not steer the pod.
