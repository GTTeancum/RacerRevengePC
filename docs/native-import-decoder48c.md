# Native in-process asset decoder 48C

2026-10-03. This component moves the frozen 48B resource/mesh conversion subset
into portable C++ for the PC import workflow. It does not broaden geometry or
material acceptance, implement game simulation, or execute PS2 instructions.
The original game data remains input; the C++ importer needs neither Python
nor the reference PS2Recomp/PCSX2 runtime in production.

## Interface and ownership

`src/assets/import_decode48c.h` defines the API in `rr::assets`:

- `decodeResource48C(input, archive, error, hooks, limits)` reads RES version 3,
  codec 0x340, flags 0 into an entry table, compressed-block provenance and one
  contiguous decoded byte vector.
- `extractResMember48C(archive, name, output, error, maxBytes)` selects exactly
  one ASCII case-insensitive member name and copies its bounded extent.
- `decodeMesh48C(input, mesh, error, hooks, limits)` decodes only the frozen
  static PSG3 subset into `RRMESH01` bytes and validation metadata. Metadata
  includes the original world/local flag, object/material names, vertex,
  triangle, strip and group counts, quantization, bounds and normal agreement.

All outputs are transactional: failures and cancellation preserve the previous
output. `DecodeError48C` separates malformed, unsupported, limit and cancelled
results, with a stable code, diagnostic byte offset and detail. It is cleared
on success. Unknown formats never silently fall back into a PS2 interpreter.

The decoder owns no files, Windows handles, UI, cache, threads or filesystem
paths. The caller provides already bounded byte vectors and owns asset hashing,
source selection, cache staging/commit, persistence and progress presentation.
Resource names are metadata and are not sanitized output paths. The cache layer
must not write them directly without its own path policy.

`DecodeHooks48C` has a context pointer, `cancelled(context)` and
`progress(context, stage, completed, total)`. Calls are synchronous on the
decoding thread. The callbacks must not throw, reenter decoding or mutate its
inputs/outputs. Stage names are `res_blocks` and `psg_vertices`; cancellation is
checked before processing, between blocks/groups and before committing output.
A final progress notification therefore does not itself establish success.

## Bounds and accepted subset

Hard limits match the earlier verifier: 64 MiB compressed RES, 128 MiB decoded
storage, 10,000 index entries, 1,024 bytes per ASCII name, and 8 MiB per extracted
member/PSG. Caller limits may reduce these ceilings. These are PC import limits,
not Xbox residency claims. Import peak memory also includes the caller's input,
the temporary decoded output, an existing output retained transactionally, and
temporary conversion arrays. Cancellation does not expose partial output.

RES validation checks the compressed-size table and zero terminator, complete
index consumption, ASCII names, every member extent, 0xff block separators and
complete container consumption. Every compressed stream must independently
finish with exactly 24,576 decoded bytes and no unused compressed input. A
24,577-byte output buffer detects oversize blocks without unbounded inflation.
All arithmetic is bounded before allocation; member offsets use widened sums.

PSG validation retains the exact 48B rules: version 3, one identity-transformed
root object, one material, known single-LOD/surface prefix, finite bounds and
quantization, and observed packed attribute groups. Only the original
`-16,-16,+16...` strip convention is accepted. Source vertices, RGBA bytes,
UVs and degenerate triangles remain intact. Strip winding restarts locally;
the decoded bounds must agree within one position quantum and every triangle
must agree with its stored normal. Local-space meshes with a nonzero origin
remain unsupported.

A decoded vertex/header mismatch is malformed (`psg.header_vertex_mismatch`).
Different source-strip versus packed-group strip counts are separately named
unsupported (`psg.strip_count_variant`): later scene inspection found original
assets whose long source strips split into several upload groups. This frozen
converter skips that variant rather than silently changing its topology rule
or calling an otherwise original asset corrupt.

Positions use binary64 multiplication and addition before the final binary32
serialization, matching `tools/assets/decode_static48b.py`. Build the decoder
with precise, non-contracting arithmetic (`/fp:strict` in the native MSVC
integration). Serialized integers/floats are little-endian explicit-width
values; no host struct, pointer, original instruction stream or GPU register
state appears in the native mesh output.

The component does not yet decode PST textures or material programs, general
hierarchies, multiple surfaces/LODs, skinned meshes, placements, collision or
animation. A successful conversion remains a structurally checked candidate;
UV/color arithmetic and the original material/culling conventions still need
independent validation. Later 48C Python scene research does not silently
change this frozen baseline.

## Compression dependency and attribution

The project-local upstream zlib 1.3.1 source was available under
`toolchains/nxdk/lib/zlib/zlib`. The parent integration vendors the needed source
without taking Xbox object files or an x64-only prebuilt library:

- C files: `adler32.c`, `crc32.c`, `inflate.c`, `inffast.c`, `inftrees.c`, `zutil.c`.
- Headers: `zlib.h`, `zconf.h`, `zutil.h`, `crc32.h`, `inflate.h`, `inffast.h`,
  `inftrees.h`, `inffixed.h`, and `gzguts.h`.
- Preserve `LICENSE` and `README`, and upstream copyright notices.

The native build links this as `native_zlib` from `third_party/native-zlib`.
zlib is by Jean-loup Gailly and Mark Adler and retains its upstream license.
The new C++ container and mesh logic is derived from this project's preserved
`validate_ui_resources07.py` and `decode_static48b.py`; it does not copy the
CarsTools source. The implementation uses zlib's public inflate API with a
fresh stream for each block and ends every initialized stream through RAII.

## Verification contract

`tests/native/import_decode48c.cpp` takes the repository root as its only
argument. Its actual-data checks decode original `TRACKS/TA.RES`, expect 383
entries / 533 blocks / 13,099,008 decoded bytes, extract the ground and rock
members, and compare both against the frozen 48B original PSG and native mesh
files byte for byte. The expected native mesh counts are 53 vertices / 25
triangles for ground and 73 / 41 for rock.

Independent synthetic RES fixtures use a manually constructed stored-DEFLATE
stream and Adler-32 trailer, avoiding a test that merely calls the paired
compressor. Malformed tests cover headers, overflow-sized metadata, entry/name
limits, compression checksum and consumption, short/oversize blocks, every
ground-PSG truncation, unsupported layout variants, bounds/normal disagreement,
member ambiguity, cancellation, progress totals and unchanged output on failure.

Acceptance requires the parent build to execute these checks for both x64 and
Win32, preserve logs and exact binary/source hashes, and keep the original
48B samples unchanged. A build or successful conversion alone is not native
gameplay, a complete asset import, or an FPS result. Test results are recorded
in the parent build evidence rather than assumed by this design document.

The first x64 run in `evidence/nativePc48C-build-20261003-125934-964` matched
both real mesh outputs but failed one test-specific error assertion. The test
incorrectly treated the ground's 53 vertices as one normal array; its four
groups actually contain 15, 16, 16 and 6 vertices. The corrupt fixture therefore
changed unrelated packed commands. The repaired test flips only the recorded
normal ranges and retains the specific normal-winding rejection requirement.
This was a test fixture defect, not evidence of a decoder normal-check failure.
