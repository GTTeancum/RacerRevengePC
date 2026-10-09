# Source-only offscreen build command 50DO

`tools/prepare_offscreen_build50do.py` writes compiler arguments for the existing
`tests/native/terrain_offscreen_drive50dh.cpp` harness. It does not invoke a
compiler, test, executable, package manager, EGL loader, or device probe.

From the repository root, with an existing output directory:

```sh
python3 tools/prepare_offscreen_build50do.py \
  /tmp/offscreen-build.json /tmp/terrain_offscreen_drive50dh
```

The first argument names the output JSON file, which contains an argv array for
a later authorized `subprocess.run(command, shell=False, check=True)` call.
The second is the future executable path; generation creates only the JSON file.
Paths are resolved from
this checkout and the invocation's working directory, with no dependency on old
objects, executables, or workspace paths. Regenerate after moving the checkout.
The output directory must already exist; an existing command JSON is replaced.
`CXX` optionally selects a compiler argv prefix, such as `clang++` or
`ccache g++`; the default is `g++`. Shell substitutions are never evaluated.

The source set follows `prepare_submit_build50cw.py`: replace the registered
terrain harness with 50DH, retain its gameplay/draw dependencies, and include
all six Linux units: 50CV draw adapter, 50CZ shader, 50DC context, 50DD renderer,
50DE presenter, and 50DG camera. Ordered deduplication avoids repeated sources.
The command retains C++20 and strict floating-point options, links zlib, and adds
`-pthread` for compile/link thread support and `-ldl` for runtime symbol loading.
The EGL/GL code uses runtime-loaded APIs, so no `-lEGL`, `-lGL`, or GL SDK headers
are required by these units. Runtime availability remains a separate question.

Only the coordinating task may execute the generated command or harness under
the global one-test-per-hour limit. The harness's future runtime arguments are
`CORPUS_JSON OUTPUT_PPM`; run it with the repository root as working directory
because retained assets use relative paths. The corpus used by the baseline is
`evidence/staticType2Producer49X/original-word/native-fixtures.json`.
Generation grants no build or runtime authorization and performs no fallback.

Status: source inspection and writing only. This generator, command, and harness
have not been executed for this addition. No compilation, EGL capability,
rendered-pixel, presentation, or frame-rate result is established.
