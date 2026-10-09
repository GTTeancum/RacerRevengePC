# Actual-renderer static culling comparison 50GD

Status at preparation: **source-only, not compiled or run by the author**.
The coordinating task owns the build and the single bounded comparison run.
This note makes no passing-pixel, draw-reduction, or performance claim.

`tests/native/static_culling50gd_check.cpp` exercises the current 50DS renderer
with identical actual asset inputs, once with `track.enableFrustumCulling=false`
and once with it `true`. It uses three fixed cameras, no driving sequence,
physics advancement, original-word machine, or synthetic diagnostic art.
No renderer, client, frozen 50DH baseline, or build registry is edited here.

## Exact scope

- Actual placed `evidence/visual49G/export-placed-normal/ta-world.rrmesh`.
- The current 50FA/50FC TA material policy from 50DU: pass 1 / slot 1, largest
  retained mip, explicit optional-untextured source spans, original retained
  source texture decoding, and explicit first-frame animation policy.
- The same source RGB exposure 2, linear/repeat sampling, and zero-alpha discard
  for `crowd` and `arrowred` as the live client.
- Actual Pod01 chariot, left engine, right engine, all five original textures,
  and source-derived textured cables. No HUD is needed for this comparison.
- Native-only 50FL cold setup, original authored grid/ground placement and
  source-derived component offsets. 50FB applies the same fresh-model nullcg
  rebasing as startup. The graph remains a frozen authored bind pose: this
  does not pretend that gameplay animation/graphics/physics have advanced.
- One prepared immutable Pod batch, shared texture banks and one surfaceless
  EGL context. Two independent 50DS owners differ only in the culling flag.
- Three camera policies: grid chase, side inspection, and elevated inspection.
  Each uses the existing bounded 50FN pod-framing correction, with fresh
  correction state, before its matrix is shared unchanged by both modes.
- 640 x 480, core desktop GL at least 3.2, RGBA8 minima, depth24, no MSAA,
  GL `[-W,+W]` clip depth, FOV 60 degrees, near 1 and far 3000.

This proves only equivalence for these explicit frozen scenes/cameras if the
run passes. It is not original shader/console parity, a gameplay result, a
collision test, an exhaustive frustum test, or a GPU performance measurement.

## Build and run

Run from the repository root. Keep the same strict C++20 compiler flags,
source list, zlib, `dl`, pthread and asset dependencies as the current 50DU
command. Only replace its main translation unit and output filename. Its
current source list already contains 50DS, 50DC, 50EB/50EA, 50EI, native draw
preparation, 50FB, 50FD, actual asset readers and startup dependencies.
Keeping the unused non-main translation units is intentional: the existing
incremental cache can reuse them without constructing a new source closure.

Example preparation (does not itself compile):

```sh
python3 tools/prepare_interactive_build50dv.py \
  /ABSOLUTE/EXISTING/DIR/culling-build.json \
  /ABSOLUTE/EXISTING/DIR/static_culling50gd_check
python3 - /ABSOLUTE/EXISTING/DIR/culling-build.json <<'PY'
import json
from pathlib import Path
import sys
path = Path(sys.argv[1])
argv = json.loads(path.read_text())
old = str(Path('tests/native/terrain_interactive_drive50du.cpp').resolve())
new = str(Path('tests/native/static_culling50gd_check.cpp').resolve())
assert argv.count(old) == 1
argv[argv.index(old)] = new
path.write_text(json.dumps(argv, indent=2) + '\n')
PY
```

Separately authorized build and one run, preserving their full stdout/stderr:

```sh
python3 tools/build_focused_incremental50ci.py \
  /ABSOLUTE/EXISTING/DIR/culling-build.json \
  /ABSOLUTE/PATH/solo-portal-recovery50fv/playable-cache \
  > /ABSOLUTE/EXISTING/DIR/culling-build.stdout.log \
  2> /ABSOLUTE/EXISTING/DIR/culling-build.stderr.log
# Run only if the build exited 0. The final output directory must NOT exist.
/ABSOLUTE/EXISTING/DIR/static_culling50gd_check \
  /ABSOLUTE/EXISTING/DIR/culling-comparison \
  > /ABSOLUTE/EXISTING/DIR/culling-run.stdout.log \
  2> /ABSOLUTE/EXISTING/DIR/culling-run.stderr.log
```

The code accepts exactly one argument, a fresh output directory whose parent
already exists. It refuses an existing directory instead of overwriting an
earlier result. It does not install packages, fall back to a different host,
spawn a window, access another machine, or select diagnostic substitutes.
The retained TA/POD01 archives and source/cache tree already used by the live
client must be available. No collision corpus argument is needed.

## Evidence and failure criteria

The output includes `result.json` and six RGBA / six PPM files. Raw `.rgba`
files contain every readback byte in original GL bottom-row-first order,
including alpha. The P6 previews flip rows once and omit alpha; comparisons
always use the raw RGBA buffer. Every successfully captured image includes
its SHA-256 in the JSON. The shared prepared Pod vertices also have a hash
and an after-render immutability check.

Each camera records its exact eye/target/matrix, framing receipt, both submit
and readback diagnostics, real checked track draw/index receipts, selection
counters, first differing byte, changed-byte/pixel totals and maximum channel
delta. The comparison requires all bytes equal, nonconstant RGB, accepted
private commits, exact readback extents, no default-framebuffer write, no
culling fallback and consistent selection/draw/index counters. Both modes
must still submit one actual cable draw. Aggregate retained track draw count
must be strictly lower than the unculled count. No pixel tolerance or skipped
first frame hides mismatches.

Pixel or counter failures do not abort the remaining fixed cameras. Their
outputs are retained and the final process result fails. Submit/readback
failures retain exact diagnostics and are not compared as valid pixels.
Successfully written evidence is never removed. JSON is checkpointed before
cold setup, after each mode, and after each pair. Catchable failures and
explicit cleanup receipts appear in the final JSON and stdout. Exit codes
are 0 for all checks and cleanup passing, 1 for failure, and 2 for usage.

The existing cold ground fixture has fatal `exit(1)` paths, and renderer owners
retain their terminate-on-unresolved-teardown contracts. Those can prevent
final JSON completion; initial/partial JSON plus captured process exit and
stderr must be retained as failed/incomplete evidence. A nonzero exit or
missing final `passed: true` is never a pass. Run logs are therefore required,
not optional, and no failed result should be overwritten by a retry.
