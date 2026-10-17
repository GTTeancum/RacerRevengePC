# Reuse checked per-bridge depth and framebuffer state

UNTESTED. Source-only independent review against remote 71aa1b21. No builds,
syntax checks, tests, gameplay or benchmarks. No measured FPS benefit claimed.

Within drawCoveragePairTriangle only, composition no longer repeats depthMask(0).
First-run payload setup writes it before the mandatory setup error gate; later
runs inherit it from the previous checked successful tail. Indexed
rasterization, framebuffer copy, program/VAO/texture/sample changes and error
queries do not change the depth write mask. Rejection skips composition.

Successful composition binds both read and draw framebuffers to scene[bank],
enables depth testing, selects LEQUAL, and retains mask0. Both setup and drawing
are error-checked. When accepted remains true, the tail therefore omits the
repeated scene bind and enableDEPTH/depthFuncLEQUAL/depthMask0 trio. Intervening
texture cleanup, program/uniform, VAO, sample and scissor calls do not alter these
values. The final restoration error gate remains and still controls the return.

On every earlier rejection, the original framebuffer and depth restoration calls
run at their original tail positions. Full sample-policy cleanup still runs on
rejection. Program, VAO, texture, multisample and scissor restoration remain on
success too. No error gate, draw, destination snapshot, or group boundary changes.
Empty regions still return before any of these operations.

This is state reuse inside one checked bridge, not the previously reverted broad
per-group restoration experiment. No cross-run state cache is introduced. Five
redundant native calls are omitted on a successful bridge. Driver behavior and
runtime correctness remain unvalidated; the next owner may validate them.

## Later-run entry reuse

Later nonempty runs in the same synchronous group also omit a repeated
MULTISAMPLE disable and depthMask(0). First-run coverageAa0 disables MULTISAMPLE;
first-run payload setup establishes mask0. Composition enables MULTISAMPLE but
never changes mask0. Every successful tail disables MULTISAMPLE and preserves
mask0, then passes the final error check before firstRun becomes false. This is
a noncircular induction from explicit first-run setup. CPU-only lookahead and
subsequent entry calls do not write either state. Failed bridges or tails abort
before reuse; new groups start firstRun=true. Depth-test disable remains per run,
and all failure cleanup remains unchanged. Two additional calls are omitted per
later nonempty run. Independently source-reviewed, still UNTESTED.

## Same-group source program reuse

Payload entry binds the source program only on firstRun; mode3 uniform remains
unconditional. Every successful tail binds that same source program and mode2
before its final error gate. VehicleGlShader::program() is a pure ready/program
read. Shader creation, linking and release occur outside this synchronous group;
CPU lookahead and entry state calls cannot switch or relink the program. First
nonempty runs remain explicit, all tail binds remain unconditional, and a failed
tail prevents reuse. One additional native call is omitted per later nonempty
run. Independently source-reviewed; no execution validation or measured gain.
