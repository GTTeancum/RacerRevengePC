# Cold material texture upload 50EB

Source-only implementation. No compilation, test, runtime/context probe,
installation, renderer integration, or protected-source change was performed.

`MaterialTextureGl50EB` owns one uploaded texture for each real texture entry in
a loaded `MaterialTextureAssets50EA` bank. Input order and 64-bit caller identities
are retained exactly. It uploads the original RGBA bytes as linear `GL_RGBA8`,
`GL_RGBA`, `GL_UNSIGNED_BYTE`, level zero. It does not reinterpret channels, flip
rows, premultiply alpha, convert color space, generate mipmaps or generate pixels.
No claim of original-game sampling/material parity is made.

## Caller contract and use

1. Load the authentic texture inputs and declared material slots through 50EA.
2. Keep an unshared Linux desktop core GL 3.2-or-newer context current on one thread.
   The host owns its context, drawable and all proc resolution. Copy the ten
   corresponding `gl*` entry points into `MaterialTextureGlFunctions50EB`; no GL
   loader or direct GL link is added by this module.
3. Supply `request.context` and `request.isCurrent`. The predicate must check the
   exact live/current context and intended drawable, if applicable, without GL
   calls. Callbacks must not throw, reenter or change the current context. Their
   storage and context must outlive this GPU owner.
4. Supply one `MaterialTextureSampler50EB` per original bank texture index. Set
   both `minification` and `magnification` explicitly to `nearest` or `linear`;
   set `s` and `t` explicitly to `repeat` or `clampToEdge`. Default/invalid values
   reject. There is no inferred historical sampler policy. This first API has
   no mip filter or generation mode: base/max level are both explicitly zero.
5. Set `maximumGpuBytes` to an explicit ceiling for the whole bank's
   `width * height * 4` total, including real textures not used by any slot.
   Call `create(request, bank, diagnostic)` once. All inputs, byte extents and
   samplers validate before application allocation or any GL call. The current
   context's version/core profile and maximum texture size are checked across
   the entire table before generating any GL texture name. Core profile avoids
   legacy pixel-transfer maps/scales/bias changing the supplied pixel bytes.
6. Use `texture(bankIndex)` to borrow a GLuint without allocation or GL calls.
   `asset(bankIndex)` additionally exposes the exact source identity. A material
   slot already supplies the appropriate index; for example:

   ```cpp
   const auto* slot = bank.slot(slotIndex);
   const std::uint32_t name = slot ? uploaded.texture(slot->textureIndex) : 0;
   ```

   An unresolved optional slot has the 50EA sentinel index and yields zero.
   No placeholder object or checkerboard is created. Zero is a lookup result,
   not permission to choose a renderer fallback. The caller owns that decision.
   The table contains each bank texture once, even if several slots share it.
7. Explicitly call `close(diagnostic)` on the creating thread while the same
   live context is current, before destroying the host. Destruction attempts
   close and terminates if ownership remains unresolved, analogous to 50DS.

The bank and sampler array are borrowed only during the synchronous create
call, even when it fails with incomplete cleanup. Their pointers are never
retained. Keep the CPU bank separately if later material-slot lookups need it.
Borrowed GL names and metadata pointers are valid only while initialized and
until the next close attempt. Bind/sample the names, but never delete, redefine,
change parameters, attach them to framebuffer/image targets, or otherwise mutate them. No shared
context or concurrent/reentrant access is supported. Sampler objects bound by a
future draw caller override texture-object parameters; that caller is responsible
for its explicit sampler, texture-unit and shader bindings.

## Allocation and observable state

The owner performs one bounded cold allocation: 4,096 fixed identity/name slots
(normally 64 KiB) plus small context/function/ownership metadata. It makes no CPU
pixel copies or per-texture allocations. Actual GL texture count and level-zero
payload follow the input table, bounded by 50EA's 4,096 textures, 4,096-pixel
dimensions and 256 MiB RGBA ceiling, and the caller's possibly smaller byte
ceiling. `gpuByteCount()` reports this logical RGBA8 payload. Driver allocation
padding and bookkeeping are not measurable or bounded by that value; GL errors
remain possible under the caller's budget.

`create` accepts an explicitly loaded empty or all-optional-unresolved bank.
It still validates the proc table and exact-current predicate but makes no GL
calls for an empty texture table. Such a publication owns metadata, reports
`initialized()` and `ownsResources()` true, and reports zero count and bytes.
Unloaded banks reject. Repeated create rejects without changing a publication.

For a nonempty upload, this module uses the already active texture unit. It
does not call `glActiveTexture`, change samplers, bind programs/framebuffers,
draw, present, read pixels, wait for completion, or inspect renderer state.
Its permitted state changes are:

- Current active unit's `GL_TEXTURE_BINDING_2D`, set to zero on upload success.
- `GL_PIXEL_UNPACK_BUFFER_BINDING`, set to zero so source addresses are pointers.
- `GL_UNPACK_ALIGNMENT`, set to 1.
- `GL_UNPACK_ROW_LENGTH`, `GL_UNPACK_SKIP_ROWS`, `GL_UNPACK_SKIP_PIXELS`, set to 0.
- Consuming GL errors and deletion's implicit unbinding of owned texture names.

These states are not restored, including on a failed create. A failure may leave
only some of these changes applied. Unpack image-height/skip-images are irrelevant
to a 2D upload; swap-bytes/LSB-first do not affect unsigned-byte RGBA. No other
global state is changed. Owned-texture deletion can clear their bindings on
other texture units as required by GL. Callers must not depend on such bindings
after close.

The uploader checks command errors and the reported level-zero width, height and
internal format. A true create publishes only after all textures upload and the
active unit's upload binding is cleared. It means GL command acceptance, not
GPU completion, readback-proven pixels, visible rendering, or gameplay parity.

## Partial failure and lifetime

Create publishes the whole table or none of it. On any partial upload failure,
it attempts to delete all generated names, retaining a generated name even when
its generation or deletion reported an error. Cleanup checks the creating
thread and current context and performs a bounded GL error drain first. It
stops at a failed deletion, retains unresolved ownership and never allocates.
The host must not generate/reuse texture names between failed close attempts.

If `cleanupIncomplete` is true, keep the GPU owner and its host alive, recover
the required current context/error state, and retry explicit `close`. All name
lookups are zero after a failed close attempt, including a wrong-context attempt.
`ownsResources()` stays true until cleanup completes. No CPU input bank or
sampler lifetime is needed for this retry. Do not destroy the host first.

The diagnostic preserves the first creation/close error, static operation,
original texture index, first GL error, cleanup index/error, and whether the
bounded drain could establish a clean error state. Cleanup can finish yet return
false to report stale GL errors: `cleanupIncomplete == false` then means all
ownership was relinquished. Empty close is idempotent. Neither failures nor
destructors silently abandon owned GL objects.

Only `material_texture_gl50eb.h/.cpp` and this document are introduced. No draw
path, build registry, material association, gameplay code or existing renderer
is modified. A future native target must compile 50EB and link its existing 50EA
and 48B dependencies; no target wiring is part of this change.
