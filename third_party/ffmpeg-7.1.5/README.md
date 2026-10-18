# FFmpeg public headers for the original-IPU adapter

These are 28 unmodified public headers from the official FFmpeg repository,
tag `n7.1.5`, retrieved on 2026-10-17. `UPSTREAM.json` records the canonical URL
and Git blob SHA-1 of every header. Every local header was verified against its
upstream Git blob hash after materialization. No FFmpeg implementation source,
executables, shared libraries or game-image pixels are included here.

- Upstream: https://github.com/FFmpeg/FFmpeg/tree/n7.1.5
- Upstream license: `LICENSE.md` and `COPYING.LGPLv2.1`, copied unchanged
- Public ABI: libavcodec 61.19.101, libavutil 59.39.100
- Header bytes excluding generated configuration: 478,699

`include/libavutil/avconfig.h` is the only locally supplied companion. Upstream
generates this file during configure; it is not present in the source tree.
The upstream `HAVE_LIST_PUB` has exactly `bigendian` and `fast_unaligned`.
The companion derives the former from compiler target endianness on Linux and
explicit known little-endian Windows x86/x86-64/ARM/ARM64 targets. Unknown
Windows architectures are rejected. It defines fast unaligned access for
x86/x86-64/AArch64 and conservatively disables it for 32-bit ARM. These affect public inline helpers, not copied
or guessed ABI layouts. Official headers and declarations remain unmodified.

## Build integration

Compile `src/assets/original_ipu_decoder.cpp` into targets that use the adapter.
Add this directory's `include` subdirectory to their include path. Link the
platform dynamic-loader library (`${CMAKE_DL_LIBS}` in CMake; `-ldl` on older
Linux toolchains) and ordinary C++ thread support. There is no compile-time
FFmpeg library link and no dependency on libavformat, libswscale or the FFmpeg
command-line program.

The Linux adapter dynamically opens `libavutil.so.59` and `libavcodec.so.61`
using the system loader. It verifies the full public version integers before
using any structure. Missing libraries, an absent IPU decoder, wrong versions,
decode errors and malformed frame extents produce a clear failure; there is
no Python, CLI, generated-cache or alternative-asset fallback.

The Windows adapter uses the same official public headers, exact version
checks and decoding pipeline. It loads `avutil-59.dll` and `avcodec-61.dll`
from absolute paths beside the running executable, obtained using
`GetModuleFileNameW`. `LoadLibraryExW` restricts dependency search to that DLL
directory and System32 using `LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR` plus
`LOAD_LIBRARY_SEARCH_SYSTEM32`. It checks safe-search support first and never
falls back to the current directory, PATH, or a process-wide search-path change.
Symbols use `GetProcAddress`; owned references are released with `FreeLibrary`.
Kernel32 is the only Windows loader dependency. Other operating systems compile
a clear unsupported diagnostic. No Windows DLLs have been bundled.

Windows loader references:
- https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryexw
- https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew

The complete 198-image Linux parity corpus passed byte-for-byte before the
Windows-loader addition (144 rider, 48 sky, six crowd). Windows compilation and
runtime parity remain unverified until tested with actual matching DLL builds.

## Decoding and provenance

`rr::assets::decodeOriginalIpuRgba` accepts a complete original IPUM and returns
RGBA only. Its caller still authenticates original archive/member bytes and
the decoded output against the existing image pins before adoption. Calling
the format decoder alone establishes no authenticity or resource lease.

Supported inputs are one image, zero frame flags, power-of-two dimensions
16..1024, at most 8 MiB of encoded data, and exact B0/B1 framing. An earlier
B0 is rejected. The 16-byte IPUM header and final four-byte B1 are excluded
from the one decoder packet, matching the official demuxer/parser and the
existing `tools/assets/decode_source_textures50fc.py` command-line conversion.
Packet allocation supplies FFmpeg's required zeroed input padding.

Settings match the reference: CPU flags zero, bitexact flag, simple scalar
IDCT, one thread, explode-on-error, YUV420P. Output must be exactly one clean
frame of the requested dimensions with bounded positive plane strides. The
adapter handles padded YUV plane rows directly. Integer conversion retains
each Python floor division, both RGB clamps, the fixed 4x4 IPU RGB16 dither,
high-five-bit RGB expansion and TEXA255/AEM1 alpha. It does not flip rows.

FFmpeg's CPU override is process-wide. Adapter calls are serialized and the
prior effective CPU flags are restored on success or failure. Use this API
only during the cold-load stage without unrelated concurrent FFmpeg work.
The decoder loop is bounded by the authenticated dimensions and packet length.
At the maximum dimension the returned RGBA is 4 MiB, plus FFmpeg frame/context
storage and at most 8 MiB packet storage. The adapter creates no disk cache.

Relevant official implementation references are in `UPSTREAM.json`:
`libavformat/ipudec.c`, `libavcodec/ipu_parser.c`,
`libavcodec/mpeg12dec.c`, `libavcodec/idctdsp.c`, and
`libavcodec/x86/idctdsp_init.c`. No emulator decoder or color-conversion
implementation was imported. The CSC reproduces the project's existing
independently expressed integer reference. PS2 hardware IDCT pixel identity
remains unmeasured; parity targets the existing pinned converted images.

## Runtime dependency and distribution status

The development system has FFmpeg 7.1.5-0+deb13u1:

| Existing library | Bytes |
| --- | ---: |
| libavcodec.so.61.19.101 | 16,895,056 |
| libavutil.so.59.39.100 | 1,148,960 |
| Total, before transitive dependencies | 18,044,016 |

These system libraries were inspected, not copied into the project. The
Debian configuration enables GPL features and many external codec libraries.
Do not treat that binary set as an LGPL-only redistributable dependency or
bundle it without completing the applicable license/distribution work.
Dynamic loading does not itself remove license obligations.

A release can instead use a deliberately minimal official FFmpeg build with
only IPU decoding, shared libavcodec/libavutil, no GPL/nonfree/version3 features,
no external autodetection, programs, network, demuxers or filters. For example,
the following is an initial **unexecuted and unvalidated** configuration recipe
for the same n7.1.5 source, not a completed shipping package:

```sh
./configure --disable-autodetect --disable-everything \
  --disable-programs --disable-doc --disable-network \
  --disable-avdevice --disable-avfilter --disable-avformat \
  --disable-swresample --disable-swscale --disable-postproc \
  --enable-decoder=ipu --enable-shared --disable-static \
  --disable-gpl --disable-nonfree --disable-version3 --disable-x86asm
```

Before distribution, build and audit that configuration, check the resulting
dependency graph and license, rerun every pixel-parity test against those exact
libraries, and provide the required notices/source/relinking or replacement
materials. Library sizes for such a minimal build are not yet measured. Windows
also needs matching native DLL builds and platform compilation/runtime parity
validation; having a loader is not evidence of those checks. This
patch establishes an in-process development path and removes generated-image
runtime data requirements; it does not certify a finished redistributable game.
