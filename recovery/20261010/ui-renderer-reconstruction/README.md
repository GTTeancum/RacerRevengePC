# New original-art native renderer reconstruction

This is NEW, UNTESTED code, not exact recovered historical bytes. No build,
syntax check, test, importer, game, GPU execution, PC access, Git write, or
integration was performed. Baseline source was read and copied, never edited.
Root owns integration and the centrally limited validation window.

## Files and dependencies

- Replacement copies of platform/linux/native/vehicle_window_gl50ds.cpp and .h
- New platform/linux/native/original_ui_native_draw50kg.h
- Requires unchanged recovered original_ui_draw50kg.h from
  ui-exact/coordinator/transcript-complete/files/platform/linux/native.
- Uses baseline MaterialTextureGl50EB APIs only: validBorrow, publicationToken,
  textureCount, asset and texture. No texture-bank API changes.
- No archive enums, source owners, screen logic, game transitions or gameplay
  behavior have been reconstructed here.

## Explicit native contract

The new enableOriginalUi flag defaults false. Enabling requires immutable,
matching CPU and GPU banks at cold renderer creation, kept alive through close.
The cold scan verifies bank index/identity and every decoded texture alpha <=128.
It does not cryptographically authenticate archives: authentic decoded bank
provenance remains the original source asset owner's responsibility, just as
in the existing material renderer. Missing banks and textures reject.

submitOriginalUi(OriginalUiNativeDraw50KG, width, height, diagnostic) consumes
ordered spans with exact TL/TR/BR/BL corners. Every quad is covered once, no
sorting, batching regrouping, generated geometry, texture, host font or art.
The draw-only overload is deliberately fail-closed. Old canvas data alone does
not carry source depth or destination-alpha-consumer authority.

Each span has explicit NEVER/ALWAYS/GEQUAL/GREATER depth plus depthWrite.
Vertex Z must contain an integer source packet value in [0,16777215]. A shader
normalizes Z once to [0,1] in D24, clear depth 0. XY uses one exact uniform
640x448 scale with centered letterbox. Scissor encloses fractional canvas bounds
with whole pixels; no source XY rounding, UV flip, or half-pixel modification.

RGB = clamp(texture.rgb * vertex.rgb * 255/128) before blending. Alpha source
coefficient = texture.a * vertex.a * (255/128)^2, and source-alpha or additive
RGB blending is explicit. Vertex raw alpha <=128 is required. Opaque spans keep
the recovered alphaScale=1 field convention, since alpha is not stored.

The new native wrapper requires opaquePresentationAlphaOnly=true. This is an
explicit caller assertion that there is no destination-alpha test/factor or
consumer expecting original framebuffer alpha. The target remains alpha 1:
clear RGBA, optionally blit the retained race, reset just alpha, then RGB-only
writes. Its RGBA readback is presentation data, never recovered source alpha.
If those conditions cannot be proven, reject activation. A complete original
alpha-storage implementation is deliberately absent.

Caller must pass recovered prepareOriginalUiNativeMaterial50KG admission first,
retain its true blend/depth/sampler choices, resolve pass groups and animation,
and reject unsupported state. The renderer cannot infer this proof from the
bank. It does not overwrite published texture sampling. No bit-exact original
integer rasterization, depth precision or retail behavior parity is claimed.

## Resource and transaction ownership

Two new RGBA8+D24 private targets are independent of both race and host-menu
banks. Vertex/index storage is allocated cold and bounded to the recovered
8192 quads / 2048 spans. Submit performs no allocation, IO, source clock update,
texture mutation or index upload. Publication occurs only after successful
span draws and cleanup-state error check. A later window blit failure retains
the privately accepted image and sets the existing presentation diagnostics.
Readback uses the same displayedTarget path. Cleanup owns all new GL names.

A requested retained race image must exist. Its most recent accepted native
race target is copied without re-evaluation. Its aspect ratio is preserved to
whole-pixel blit rounding, independently of the source UI canvas. Source race
publication and gameplay are never modified by UI submission.

## Still blocked / not complete

Real archives and actual decoded material, screen, font, atlas, depth, animation
and pass-group ownership are still required. The older recovered archive enum
has only seven entries and is not presumed compatible with later course/rider/
options composition. The recovered canvas currently writes Z=0; it must not be
promoted to a source depth owner without source evidence. A separate verified
owner must populate original packet Z and native span state. No call site is
changed, so UI stays inactive. This work does not deliver a finished frontend.
All code and GL behavior remain uncompiled and untested.

## Sampler admission amendment

Every native span now supplies an explicit MaterialTextureSampler50EB. It must
have source-admitted equal minification/magnification (nearest or linear) and
explicit repeat/clamp-to-edge for S/T. Before modifying the pending UI target,
the renderer binds sampler zero and verifies the texture's actual immutable
min/mag/wrap/base-level/max-level parameters via glGetTexParameteriv. Levels
must both be zero. Same image with conflicting requested sampler state rejects;
provide separate compatible bank bindings/owners instead. No first-use sampler
inheritance, runtime texture-parameter mutation, or speculative sampler default.
This adds bounded per-submit GL queries (at most six per span); no sampler cache
or source sampler-object allocation is introduced in this reconstruction.

The cold alpha<=128 admission deliberately applies to every bank texture,
including textures referenced only by opaque spans. This bounded limitation
can reject valid opaque-only original art; it is not a claim that the source
required this restriction for opaque materials.
