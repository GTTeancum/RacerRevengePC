# Shared-renderer headless drive 50EK

Status: source-only. This client and its documentation were written and inspected
without a compiler, build, test, executable, GL/device probe, installation,
desktop session, or original-game execution. No runtime success, pixels, or
allocation result is claimed. The coordinating task owns the next authorized
single-case validation slot, no earlier than 2026-10-08 15:34 UTC.

`tests/native/terrain_headless_drive50ek.cpp` is one bounded cloud-validation
client of the same `VehicleWindowGl50DS` renderer used by
`terrain_interactive_drive50du.cpp`. It retains the actual Pod01 vehicle meshes,
the placed TA visual mesh, and the same explicit material policy. The frozen
`terrain_offscreen_drive50dh.cpp` and its separate 50DD/50DE renderer path are
unchanged. This addition does not expand physics coverage or replace that
baseline.

## Invocation and dependencies

After a separately authorized build, run from the repository root:

```sh
/path/to/terrain_headless_drive50ek \
  evidence/staticType2Producer49X/original-word/native-fixtures.json \
  /path/to/existing/output-directory/final.ppm
```

Exactly two arguments are accepted. There are no frame-count, warm-up,
balanced-control, no-support, interactive, or rerun options. The output file is
opened only after the single successful final readback; its parent directory
must already exist. An existing file at the specified PPM path is overwritten.

Build integration is separate from these two files. Use the retained gameplay,
asset, fixture, strict-floating-point, pthread, zlib and dl dependencies used by
50DU, with these Linux/rendering translation units:

- `platform/linux/native/offscreen_gl_context50dc.cpp`
- `platform/linux/native/vehicle_window_gl50ds.cpp`
- `platform/linux/native/material_texture_gl50eb.cpp`
- `src/assets/material_texture_assets50ea.cpp`
- The existing 50CV draw adapter, 50CZ shader implementation, and native draw
  asset/batch dependencies already used by 50DU

The coordinating task's `tools/prepare_shared_headless_build50el.py` writes the
corresponding compiler argv JSON from two arguments, a command-JSON destination
and future executable path. That generator does not compile or execute it.

The client does not use WindowHost50DQ, keyboard50DR, fixed-step-clock50DT,
50DD, or 50DE. No SDL/window/event/swap path is present. Linux EGL and desktop
OpenGL runtime support for the explicit request are still required. Failure is
reported rather than installing dependencies or selecting another platform.
The retained corpus, source asset tree, animation/physics/effect fixtures, and
cold original-word helper dependencies remain those of 50DU. This is not an
independently packaged game or an oracle-free cold loader; its frame loop makes
no oracle comparisons.

## Exact graphics path

The caller owns an `OffscreenGlContext50DC` requesting MesaSurfaceless, a
320 x 240 pbuffer, desktop core OpenGL at least 3.2, RGBA minima of 8 bits each,
24-bit depth minimum, zero stencil minimum, and no multisampling. The actual
accepted context version and extent are included in the result.

50DS receives `output = VehicleWindowOutput50DS::headless`, 320 x 240 maximum
target bounds, and procedure/currentness callbacks for that exact context.
50EB receives the identical context and identical `isCurrent` function pointer.
Headless50DS commits its ordinary private color/depth target; there is no
default-framebuffer blit, event pump, window, or swap. A successful submission
must report that it did not modify the default framebuffer.

Pod shading is `callerMaterialTextures`, with all 26 original material entries
explicitly requiring native pass 1 / slot 1 and the largest retained mip. The
five exact retained keys, mips and dimensions from the 50ED selection are
checked before the frame loop:

- `01_2`: mip 4, 256 x 256
- `01_3`: mip 0, 128 x 128
- `pod_wndw`: mip 2, 64 x 64
- `01_1`: mip 4, 256 x 256
- `flame`: mip 0, 64 x 64, shared by both authored flame materials

There are exactly five GPU textures and 622,592 RGBA payload bytes. Sampling is
explicit linear minification/magnification and repeat in both axes. No missing
texture or material uses fallback pixels. Full key/provenance details are in
`docs/pod-materials50ed.md` and are pinned in this client's cold checks.

The actual placed track is loaded once from
`evidence/visual49G/export-placed-normal/ta-world.rrmesh`. Its 50DS upload ceilings
are 4 MiB of vertex bytes and 2 MiB of index bytes. Track shading explicitly
remains `viewerNeutralUntextured`; Pod texturing does not imply track textures.
After successful upload, CPU-only track/material staging is released. The
immutable GPU texture bank and retained 50CR Pod owners outlive the renderer.
Original material shaders, multipass/specular behavior and rendering parity are
not claimed.

## One native driving sequence

Cold setup and the native advancement lambda are adapted from current 50DU.
Advanced control preset 4, native48Y held-state ingestion, the existing control
shaping/binding, body integration, main/secondary static response and movement
query remain in use. Packet bytes are the identical differential schedule from
50DH, indexed from zero:

- Frames 0-9: centered axes
- Frames 10-39: packet bytes 5 and 7 are 0, both engines throttle
- Frames 40-79: byte 5 is 70 and byte 7 is 0, differential throttle
- Frames 80-99: centered axes
- Frames 100-159: bytes 5 and 7 are 255, braking

Bytes 4 and 6 remain 128; bytes 2 and 3 remain `0xff`, with all action buttons
released. Any unexpected button transition fails instead of inventing a
dispatcher. Controls enter only through the native controller interface, with
no pose steering or fabricated contacts. Each frame uses `.016f` and must
schedule exactly one native substep. There is no wall-clock pacing, catch-up,
warm-up, skipped first frame, or second scenario.

The 37 retained collision owners, authored nine-zone ownership and source
first-zone global link are unchanged. The broader TA visual mesh does not add
collision owners, race progression, AI, or full-world traversal support. The
same world-aligned diagnostic follow camera is supplied explicitly. End checks
require both real main/secondary contact support, peak drive above 0.9,
peak differential above 0.1, and at least 30 frames of native braking.

## Allocation observations and output

`RenderAllocationStats50EH` is constructed before the loop and stores one
chronological observation for each attempted frame render, including the very
first. The two counter snapshots bracket CPU draw-bank preparation,
`buildVehicleDrawBatch50CO`, and `VehicleWindowGl50DS::submit`. That is the same
logical prepare-and-submit interval as 50DH's old presenter call, but for the
newer textured/track renderer. A preparation/submission failure is recorded
before diagnostic formatting, throwing, or cleanup. Normal observations do not
trigger per-frame logging.

The result's `render_allocations` contains:

- `first_frame`: whether it was observed, validity, and its count; invalid or
  absent counts are JSON null
- `all` and `after_first`: chronological frames, valid/invalid frames, nonzero
  frames, sum, maximum, and a sum-overflow flag
- Capacity overflow, rejected-frame count and rejected-count overflow

Sums and maxima cover valid samples only. `after_first` excludes chronological
frame zero; it is not evidence of completed driver warm-up or steady state.
Counter decreases, capacity rejection and overflow fail validation. Nonzero
render-allocation counts are reported without being suppressed or attributed to
the application or driver. The thread-local C++ counter does not count all C
allocation calls, other threads, bytes or GPU memory. Native/camera calls have
a separate zero-change counter guard. Cold loading, camera, output formatting,
final readback and cleanup are outside the render observation interval.

One JSON result is written to stdout on normal success or caught failure. It
includes partial completed/prepared/submitted-frame counts and allocation
coverage on failure. Domain-specific diagnostics are JSON lines on stderr only
when a relevant operation fails. Exit status is 0 for a completed case, 1 for
failure, or 2 for incorrect arguments. There are no per-frame status lines or
timing/FPS claims.

After 160 accepted submissions, `readbackRgba` reads only the final private
commit into a preallocated 307,200-byte buffer. Returned width, height and byte
extent must match. A failed readback is discarded and never written as a new
PPM. Successful GL bottom-row-first RGBA is written as P6 top-row-first RGB,
flipping rows exactly once and dropping alpha. Submission acceptance alone is
not GPU completion; the final readback may synchronize.

Both normal completion and exceptions explicitly close in this order:
renderer, GPU texture bank, context. Unresolved upper-layer ownership prevents
closing the layer beneath it. The existing owners retain their terminate-on-
unresolved-teardown destructor contracts, so unrecoverable cleanup or process
failure can prevent the final JSON result; emitted cleanup diagnostics are not
silently converted to a successful run.
