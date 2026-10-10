# Shared caller-context vehicle and optional track renderer 50DS

Status: the explicit source cable rendering join and optional host-authored
live heat/damage HUD and next-gate route cue are source edits awaiting integration validation by the
live-client owner. Their author did
not compile or launch the game. No install or protected-source change was
performed. A build alone does not establish readable in-game pixels, hardware
acceleration, gameplay completeness, or a measured frame rate.

## Purpose

`VehicleWindowGl50DS` renders actual `VehicleDrawBatch50CO` geometry from one
loaded `VehicleDrawAssets50CR` owner on the caller's current desktop OpenGL
context. It reuses the `VehicleGlBackend50CV` draw adapter and
`VehicleGlShader50CZ` shader. An optional actual world-space `Mesh48B` visual
track is uploaded once during creation and drawn ahead of the vehicle. It owns
GPU objects only: no EGL, SDL, window,
context, event loop, input, camera policy, simulation, or buffer swap.

Cold creation owns one vehicle shader program, six vehicle VBOs, six vehicle VAOs, three
retained vehicle EBOs, and two private RGBA8/depth24 framebuffer targets. A track
adds one static VBO, one static EBO, and one VAO. The explicit source-color track
mode also owns one additional program and its two shader objects. All storage is allocated once.
The targets preserve the adapter's unpublished-frame transaction. The compatible
default output, `VehicleWindowOutput50DS::defaultBackBuffer`, separately blits a
successful private submission to default framebuffer 0's `GL_BACK` buffer. The
window host, such as `WindowHost50DQ`, owns the subsequent swap. Explicit
`VehicleWindowOutput50DS::headless` stops at that same private commit, without
requiring a default drawable or making a default-framebuffer copy. Both modes
can read the last committed viewport into bounded caller-owned RGBA storage.
No synthetic world mesh, fake window, second rendering path, or command-recorder
fallback exists.

## Optional host-authored live heat/damage HUD

The renderer now accepts a deliberately labeled `HOST HUD` overlay. It is a
compact native display policy for supplied gameplay values, with no claim of
original-game HUD layout, font, assets, source threshold recovery, or parity.
The original world/pod/cable geometry and material paths are unchanged. This
overlay is native screen-space glyph/rectangle geometry, not substituted world
assets, a generated image, or a postprocessed screenshot.

Cold opt-in and per-frame input are appended to the existing structs:

```cpp
request.enableHostHud = true;
VehicleWindowHostHud50DS hud{};
hud.heat01 = liveHeatNormalized;
hud.leftDamage01 = liveLeftDamageNormalized;
hud.rightDamage01 = liveRightDamageNormalized;
hud.boostActive = liveBoostActive;
hud.overheated = liveOverheated;
frame.hostHud = &hud;
```

The three values must be finite `[0,1]`: heat zero means cool and one means the
caller-selected heat limit; damage zero means pristine and one fully damaged.
The caller owns conversion from source units, source thresholds, and the left/
right field mapping. Values are not silently clamped. Invalid values or a
non-null frame input without cold opt-in fail with an explicit `frame` error
before any frame GL call. Input is copied and consumed synchronously without
retaining the caller's pointer. Null `frame.hostHud` hides the HUD, including
when its cold resources are enabled. Default requests create no HUD resources.

The lower-left panel shows `HOST HUD`, `HEAT`, `L DMG`, and `R DMG`. Bars grow
with supplied heat/damage. Status is `OVERHEAT` when that supplied flag is true,
otherwise `BOOST` when boost is active, otherwise `IDLE`; idle does not infer
that boosting is available. Overheat takes precedence when both flags are set.
Five-by-seven host-authored glyphs are built from contiguous horizontal pixel
runs. The panel uses 2-pixel glyph cells at viewports at least 960x540 and
1-pixel cells below that, shrinking only to fit very small viewports. At 1280x720
it is a 320x112-pixel opaque panel with a 16-pixel left/bottom margin.

### Optional next-gate bearing

The caller can additionally set `hud.routeValid = true` and supply `routeX`
(screen right) and `routeY` (ahead, drawn upward). Both default to zero with
the cue disabled. Enabled components must be finite in `[-1,1]`, with squared
length within `0.001` of one; malformed enabled bearings fail with a `frame`
error before frame GL work. Disabled coordinates are ignored. The renderer
does not select a navigation target, derive a heading, or normalize bad input.
The live caller supplies its actual next track gate and current camera heading.

The cue is a 44x60-cell top-right panel labeled `HOST` and `NEXT`, with an
eight-cell top/right margin. At 1280x720 it is 88x120 pixels with a 16-pixel
margin. A cyan shaft and triangular arrowhead rotate with the supplied bearing,
including backward bearings. This is host-authored navigation assistance,
without an original-HUD claim, physics movement, extra scene render, or image
substitution. It is appended to the same bounded HUD geometry, upload, draw,
private commit, and normal readback. The combined heat/damage/status and route
geometry stays below 400 quads, within the existing 512-quad capacity; no buffer
size, shader, draw-state cleanup, or per-frame allocation changes are needed.
This route-cue edit has not itself been compiled or run; integration owns the
live check.

Cold opt-in owns one simple color shader program with two shader objects, one
VAO, a 48 KiB dynamic VBO, and a 12 KiB immutable quad EBO. Reusable CPU vertex
storage is 48 KiB plus a small fixed header. These capacities are independent
of viewport and source values. Every frame constructs at most 512 quads, makes
one bounded buffer upload and one indexed triangle draw, with no application
heap allocation or texture access. Capacity overflow is an explicit `limits`
failure. Buffer sizes, index bytes, and the VAO layout are cold-verified, and
all added GL names use the existing recoverable cleanup ownership rules.

The overlay draws after the pod and optional cables into the same unpublished
color target. Depth testing/writes are disabled just for the overlay and its
state restoration re-enables normal depth writes/testing, restores the vehicle
program, and unbinds HUD VAO/array-buffer state, including on a checked draw
failure. The next frame still establishes the full normal world state. Any
HUD upload/draw/restoration failure aborts private publication; the previous
committed image remains the current publication. Window copying and readback
therefore see the same integrated frame, without a separate presentation path.

`hostHudDraws`, `hostHudVertices`, and `hostHudIndices` are checked-command
receipts in the normal diagnostic. Require `batchAccepted` for publication;
a successful draw receipt alone is not a committed image, visible-pixel proof,
or timing measurement. This edit has not itself been compiled, launched, or
visually inspected in the running client; those checks belong to integration.

The original explicitly selected
`VehicleWindowMode50DS::viewerNeutralUntextured` mode uses the 50CZ viewer's solid
`(0.68, 0.72, 0.78, 1)` branch, while retaining and validating original scene
material indices. Missing textures never select this mode automatically. This
is diagnostic neutral shading for authentic visual geometry, not original game
texture/material parity. Optional `callerMaterialTextures` mode instead accepts
explicit per-material mappings into an already uploaded `MaterialTextureGl50EB`
bank. It uses the existing 50CZ texture branch without changing the shader.
Track shading requires its own neutral, source-color, or caller-texture opt-in. No
collision geometry, fallback track, placement transform, or material association
is invented.

## Live track visibility correction

The live 50DU captures at ticks 292 and 1131 showed the real pod against an
almost uniform light-gray course. The renderer was correctly drawing track and
pod into the same depth target, but the original neutral branch discards every
track vertex color and returns the same RGB for road, walls and scenery. This
is a display-policy limitation rather than evidence of a missing track draw.

The retained TA world mesh has 110,196 vertices, 209,514 indices and 11,142
distinct vertex RGBA values. Its actual RGB channels range from 0 through 127;
alpha is 63. A vertical triangle intersection at the recorded start position
`(2795.92, 5924.79)` in X/Z reaches visual terrain at Y `1272.993636`, close to
the captured body Y `1274.94`. These checks support retaining the exact track
placement and buffers, rather than replacing its geometry or changing depth.

The optional `sourceVertexColors` mode displays those existing RGB values and
actual triangle orientation. It adds no texture, geometry, topology, invented
material mapping, or original-shader claim. Its host display lighting is a
two-sided directional term: `0.45 + 0.55 * abs(dot(normal, lightDirection))`,
with `lightDirection = normalize(0.35, 0.85, 0.30)`. The geometric plane normal
comes from derivatives of interpolated world position; no normal map or guessed
source normal is used. Degenerate fragment planes keep the ambient term.
Source RGB is normalized by the existing vertex attribute, multiplied by an
explicit caller exposure and this illumination, and written with alpha one.

For the TA live client, opt in with:

```cpp
renderRequest.track.mode = rr::platform::VehicleWindowMode50DS::sourceVertexColors;
renderRequest.track.vertexColorScale = 2.f;
```

The scale is an explicit host display choice for the retained 0..127 bytes,
not a recovered material equation. Valid scales are finite `(0,8]`. The default
is one; neutral mode and an absent track reject any nondefault scale. Vehicle
materials, sampling, vertex colors, shader and draw policy remain unchanged.
The new program is bound only around the actual track draw; 50CZ is restored
before the pod draws. Both use the same complete matrix, viewport, target and
writable depth. The source-color program participates in cold allocation,
readiness, failure cleanup and retained-resource retry ownership.

The old live camera additionally kept a world-aligned offset of `(-45,+25,-65)`
instead of turning with the pod. The client owner can use actual current
vehicle forward X/Z for a host chase view, with eye 64 units behind and 12
above, a target 38 ahead and 3 above, a 60-degree vertical FOV and near/far
distances of 1/3000. This remains caller camera policy and is separate from
original game-camera recovery. Validate both changes using actual live-client
captures at rest and while driving/turning; an isolated diagnostic render does
not establish playable course readability.

## Source-ordered real track materials

Track `callerMaterialTextures` mode adds explicit source surface ranges and a
borrowed real `MaterialTextureGl50EB` publication. It does not change pod
materials. The track bank can be separate from the pod bank, but both must
retain the renderer's exact context and `isCurrent` function pointer and stay
published until the renderer closes.

Cold wiring after uploading the track bank:

```cpp
std::vector<rr::platform::VehicleWindowTrackSpan50DS> trackBindings;
for (const auto& source : *trackAssets.spans()) {
    rr::platform::VehicleWindowTrackSpan50DS span{};
    span.indexStart = source.indexStart;
    span.indexCount = source.indexCount;
    span.vertexStart = source.vertexStart;
    span.vertexCount = source.vertexCount;
    span.material.textureIndex = source.textureIndex;
    span.material.mode = source.textureIndex == rr::assets::unresolvedMaterialTexture50EA
        ? rr::platform::VehicleWindowMaterialMode50DS::untextured
        : rr::platform::VehicleWindowMaterialMode50DS::requiredTexture;
    trackBindings.push_back(span);
}
renderRequest.track.mode = rr::platform::VehicleWindowMode50DS::callerMaterialTextures;
renderRequest.track.vertexColorScale = 2.f;
renderRequest.track.textures = &trackGpuBank;
renderRequest.track.spans = trackBindings.data();
renderRequest.track.spanCount = static_cast<std::uint32_t>(trackBindings.size());
```

The asset loader must resolve original material/pass/map identity; the renderer
never chooses one by filename, texture size, geometry shape or visual similarity.
Each span explicitly selects a required published texture or untextured source
colors with the unresolved sentinel. A missing required texture rejects creation
or submission, rather than switching to a replacement. The separate bank's
immutable sampler settings are used unchanged on unit zero, without a sampler
object override.

The table must contain 1..65536 nonempty triangle spans and cover the complete
index and vertex arrays contiguously in source order. Every index is checked
against its span's declared vertex range. Existing mesh/count/byte bounds still
apply; each draw fits a signed GL count and its index byte offset fits the
already-validated EBO allocation. All validation occurs before renderer
allocation or GL calls. Creation copies only index ranges, resolved bank
index/GL name and the explicit cutout policy into a bounded 20-byte-per-span
table on this native layout; a 353-span table is 7,060
bytes. The source span array and CPU mesh are no longer borrowed afterward.

Every frame traverses that exact order without texture sorting, rebasing,
merging or geometry replacement. A draw uses `indexStart * sizeof(uint32_t)`
as its EBO offset, `indexCount` as its count, and zero base vertex. Source UVs
at Vertex48B offset 12 reach the track sampler unchanged. A textured surface
multiplies real texture RGB by the same source vertex RGB, explicit exposure
and geometric host lighting used by the source-color mode. An explicitly
untextured surface binds no image and uses source RGB. Fragments remain opaque,
with one explicit per-span optional policy: `span.discardZeroAlpha = true`
discards a sampled real texture texel only when its alpha is zero. This preserves
the actual decoder's alpha-zero crowd/arrow pixels instead of drawing black
rectangles. It is a native display cutout choice, not recovered original GS
alpha-test, alpha blending, multipass effects or original shader equations.
The default is false, and untextured spans reject the option. Source-color-only
whole-mesh and pod shader behavior remain unchanged.

The compatible neutral and source-color whole-mesh modes remain available and
reject texture/span inputs. The optional track program is restored to the
unchanged vehicle shader before pod draws. All surfaces share the same private
transaction, writable depth and camera matrix. No application allocation or
track geometry upload is added to submission.

Per-submit diagnostics provide `trackDraws`, `trackTexturedDraws`,
`trackUntexturedDraws` and `trackIndices`, incremented only after the corresponding
real GL draw passes its error check. These are command receipts, not GPU timings
or visible-fragment counts; a later failure may discard the private frame.
Require `batchAccepted` for commitment and use normal committed readback for
actual pixels. `trackSpan` and `texture` identify material/range failures. For
the initial supported TA material corpus, expected successful receipts are
353 draws, 279 textured, 74 explicitly untextured and 209,514 indices; additional
authentic decoded materials can reduce the untextured count without changing
this interface or surface order.

## Actual per-frame source cables

The optional cable join consumes the current `PodCableMesh50FD` generated by the
admitted original attachment/Hermite/ribbon path. It does not invent a line
between engine roots or reconstruct cable geometry inside the renderer.

Cold-select the real uploaded cable texture:

```cpp
renderRequest.cables.textures = &cableGpuBank;
renderRequest.cables.textureIndex = 0;
```

The separate 50EB bank must have the renderer's exact context and `isCurrent`
callback and remain published until renderer close. Its caller-authored sampler
must repeat U/V for the source cable UVs. Missing required textures reject;
there is no substitute color or texture. Default-null cable configuration
allocates no cable objects and preserves the earlier track/pod-only path.

For every opted-in frame, provide the actual current output:

```cpp
rr::platform::VehicleWindowFrame50DS frame{width, height, matrix, clearRgba};
frame.cables = &currentCableMesh;
renderer.submit(vehicleBatch, frame, diagnostic);
```

The pointer is synchronous-only and cleared before `submit` returns. Frame
presence must match cold opt-in. Preflight checks all 36 source Vertex48B
positions/UVs for finiteness and every one of the 96 indices against canonical
50FD topology. The exact per-cable eight triangle pairs are `(v,v+1,v+2)` and
`(v+2,v+1,v+3)`, with `v = side*18 + sample*2`. Invalid geometry or an absent
mesh rejects the whole frame before its GL work.

Cold creation allocates one 864-byte dynamic VBO, one 384-byte immutable EBO
and one VAO. It checks allocations, exact GPU index bytes and Vertex48B layout.
The source generator's topology is immutable; submission uploads only the 864
current vertex bytes without heap allocation. World placement, source UVs and
explicit neutral vertex modulation are preserved. This does not run the cable
generator, modify game scenes, or change physics.

After all pod draws, the renderer draws both cable strips in one indexed call
through the unchanged 50CZ texture program using the real cable bank texture.
The same matrix, private framebuffer and writable LESS depth remain selected;
the host policy is two-sided/opaque and does not apply track cutout or lighting.
The cable draw and its error checks occur before private publication, so a
failure discards the pending combined image and retains the previous committed
image. The single cable VBO is safe under same-context GL command ordering;
previous published images are renderbuffer pixels and no longer depend on
its bytes.

Successful submit diagnostics report `cableDraws=1`, `cableVertices=36`,
`cableIndices=96`, in addition to the unchanged track receipts. These are checked
draw commands; require `batchAccepted` for commitment, normal readback for pixels,
and the explicit completion barrier for completed-render timing. The renderer
never closes or mutates the borrowed cable texture bank. Close the renderer
before closing any borrowed pod, track or cable bank and before the context.

## Window host integration

1. Load authentic data into a persistent, nonmovable `VehicleDrawAssets50CR`.
   Preserve that owner and its prepared meshes until the renderer is closed.
2. Create and make current a desktop GL 3.2-or-newer, nonstereo, double-buffered,
   nonmultisampled window context. For 50DQ, request `samples = 0`. The host and
   its callback storage must outlive the renderer.
3. Supply `VehicleWindowRequest50DS` with a resolver and exact-current-context
   predicate. The predicate must verify the intended context and window drawable
   on the creating thread, before any GL entry point is used. It must not itself
   issue GL calls. A 50DQ integration can use:

   ```cpp
   request.context = &host;
   request.procAddress = [](void* context, const char* name) noexcept
       -> rr::platform::VehicleWindowGlProc50DS {
       return static_cast<rr::platform::WindowHost50DQ*>(context)->procAddress(name);
   };
   request.isCurrent = [](void* context) noexcept {
       return static_cast<rr::platform::WindowHost50DQ*>(context)->current();
   };
   request.maximumWidth = maximumDrawableWidth;
   request.maximumHeight = maximumDrawableHeight;
   request.mode = rr::platform::VehicleWindowMode50DS::viewerNeutralUntextured;
   request.output = rr::platform::VehicleWindowOutput50DS::defaultBackBuffer;
   ```

4. Call `renderer.create(request, assets, diagnostic)`. Both target dimensions
   are required positive cold allocation bounds. They are checked against GL
   renderbuffer and viewport limits. No dimension, window format, or shading
   fallback is inferred. Allow roughly 16 bytes per maximum pixel for both
   color/depth pairs, plus geometry and implementation-dependent driver overhead.
   Excessive maximum sizes can fail allocation; they are not a resize policy.
5. Build a successful actual 50CO batch using those exact 50CR prepared meshes.
   Supply `VehicleWindowFrame50DS` with the actual drawable-pixel width/height,
   a 16-float finite world-to-GL-clip matrix, and four finite `[0,1]` clear values.
   Call `renderer.submit(batch, frame, diagnostic)`.
6. Call `host.swap(...)` only after `submit` returns true. A true return means
   GL command acceptance, not GPU completion or visible presentation. There is
   no `glFinish`, CPU pixel readback, or application heap allocation in submit.
   Explicit readback is a separate operation and can synchronize with GPU work.
7. Explicitly call `renderer.close(diagnostic)` while its exact context and
   drawable are still current, before `host.close(...)`. Declare/own the host
   before the renderer so ordinary reverse destruction also respects lifetimes.

50DG's matrix output can be passed directly to the frame. 50CZ evaluates
`vec4(worldPosition, 1) * worldViewProjection`; storage is column-major and the
upload uses `transpose = false`. Positions are already world-space. The caller
owns the camera, aspect ratio, near/far planes and complete GL clip-depth
conversion. No additional vehicle/root/model transform is applied here.

## Headless host integration

Use the exact same `VehicleWindowGl50DS`, asset owner, optional actual track,
material bindings, 50CO batch, matrix, and frame input. The sole output selection
is:

```cpp
request.output = rr::platform::VehicleWindowOutput50DS::headless;
```

The caller creates and owns an existing desktop GL 3.2-or-newer context, for
example `OffscreenGlContext50DC`. 50DS neither creates a replacement context nor
introduces a window. With an already-created, current 50DC host:

```cpp
request.context = &offscreenHost;
request.procAddress = [](void* context, const char* name) noexcept
    -> rr::platform::VehicleWindowGlProc50DS {
    return static_cast<rr::platform::OffscreenGlContext50DC*>(context)->procAddress(name);
};
request.isCurrent = [](void* context) noexcept {
    return static_cast<rr::platform::OffscreenGlContext50DC*>(context)->current();
};
request.maximumWidth = maximumRenderWidth;
request.maximumHeight = maximumRenderHeight;
request.output = rr::platform::VehicleWindowOutput50DS::headless;
```

Shading still requires its explicit neutral or caller-material selection. For
50EB texture upload, reuse this exact `context` value and `isCurrent` function
pointer. Preserve all host, asset, texture, thread, and unshared-context lifetimes
described below. Close 50DS, then the borrowed texture bank, then the context.
The caller's 50DC configuration still has its own explicit platform, profile,
format, and pbuffer requirements; 50DS does not select or relax those policies.

Headless creation does not select framebuffer 0's draw/read buffers, check its
completeness, inspect its double-buffer/stereo/multisample format, or resolve
`glBlitFramebuffer`. Submission never copies to framebuffer 0. Cleanup can unbind
private FBOs by binding zero, which does not require that default framebuffer to
be complete or drawable. The private targets remain single-sample RGBA8/depth24
with their usual bounded allocation and completeness checks. A headless frame's
positive width and height select a viewport within those cold bounds and are
independent of host pbuffer size. There is no swap step.

`submit` returning true in headless mode means the real combined track/vehicle
private transaction was accepted. It sets `batchAccepted = true` and leaves
`defaultFramebufferMayBeModified = false`. It does not establish CPU pixel
availability, hardware acceleration, a visible frame, or timing evidence.

## Bounded committed-viewport readback

For an explicit completion barrier without readback, use
`renderer.finishSubmittedWork(completionDiagnostic)` after successful submission.
This requires an existing privately committed image, the creating thread and
the exact current context. It checks prior GL errors, calls the cold-resolved
`glFinish`, checks errors again, and sets `submittedWorkCompleted` only on success.
It performs no allocation, readback, swap or publication change. The caller can
measure its whole submit-plus-finish interval to distinguish completed rendering
from queued command throughput. The method itself records no duration and
cannot establish hardware acceleration or a presented frame. It resets its
diagnostic, so use a separate variable if preserving submit's track draw receipts.
Normal submission and window presentation never invoke the barrier implicitly.

In either output mode, call:

```cpp
renderer.readbackRgba(rgbaBuffer, capacityBytes, diagnostic);
```

The destination must be nonnull, live, writable, and disjoint from renderer input
and diagnostic storage, with at least `capacityBytes` accessible bytes. The
capacity must cover the last privately committed viewport's `width * height * 4`
bytes. Cold target limits already bound this product within `size_t`. No CPU
allocation occurs, and no destination pointer is retained after the synchronous
call. The returned bytes are `GL_RGBA` / `GL_UNSIGNED_BYTE`, tightly packed,
bottom row first, with no scale, padding, Y flip, or conversion to a file format.
Only the committed viewport rectangle is returned, not the whole maximum-sized
target. Any bytes beyond that rectangle remain untouched.

The diagnostic supplies `readbackWidth`, `readbackHeight`, and `readbackBytes`
after checking initialization, current thread/context, and image existence.
These describe the committed viewport and required byte extent, not a count of
successfully written bytes. A null/undersized destination then rejects before
GL calls or output modification, while still reporting that extent. Before the
first private commit, readback rejects with `noImage`; cold target allocation
alone does not publish an initialized image. The other structural/read errors
use `readback`, apart from ordinary initialization/thread/currentness errors.

Readback binds the committed private FBO for reading and color attachment zero,
unbinds `GL_PIXEL_PACK_BUFFER`, and explicitly sets pack alignment to one and
row length, row/pixel skips, byte swapping, bit order, image height, and image
skips to zero. It checks GL errors before readback, after that setup, after
read-FBO completeness, and after `glReadPixels`. No GL state is restored; the
read FBO and pack state remain selected. Both output modes resolve the core
`glPixelStorei` and `glReadPixels` entry points during cold creation.

`readbackBufferMayBeModified` becomes true immediately before `glReadPixels`.
If the call fails with that flag false, the destination was not written. If it
fails with the flag true, any subset of the required bytes may have changed;
discard the whole readback result. The diagnostic does not claim a valid prefix
or atomic CPU output. Readback never changes private publication, so a later
retry can read the same committed image when the context permits. It can block
for GPU work, though it adds no explicit `glFinish`.

The committed dimensions are captured with the private bank commit. A rejected
later frame, including one requesting a different viewport, cannot replace them.
A window-copy failure after private commit still publishes that newer private
image, so readback targets it even though the caller must not swap the failed
window frame. Context/device loss can invalidate any stored image and is not
recoverable through CPU bookkeeping alone.

## Optional caller-material texture input

Leave `request.materials` entirely defaulted for the original neutral path.
To select actual supplied texture data:

1. Load the caller-resolved actual RRTEX001 bytes and explicit slot identities
   into `MaterialTextureAssets50EA`. Upload that bank once with
   `MaterialTextureGl50EB`, supplying all explicit filter/wrap decisions and the
   GPU byte ceiling there. 50DS never resolves a filename, uploads pixels, chooses
   a material pass/slot, or invents a sampler policy.
2. Use the exact same `context` value and `isCurrent` function pointer in the
   50EB and 50DS requests. For example, assign
   `textureRequest.isCurrent = request.isCurrent`; separately written equivalent
   lambda functions need not have equal function pointers. That common callback
   must verify the exact context and host surface/currentness contract, including
   the window drawable for window output, without issuing GL calls.
3. Construct one `VehicleWindowMaterialBinding50DS` entry for every original
   scene material, including unused materials, in each component's original
   material-index order. Each entry must explicitly choose:
   - `requiredTexture` with a valid original 50EB/50EA bank index; or
   - `untextured` with `assets::unresolvedMaterialTexture50EA` as its index.
   `unspecified`, out-of-range indexes, missing required textures, and an
   untextured entry naming a texture all reject cold creation. An unresolved
   optional 50EA slot is not automatically converted to untextured rendering.
   The caller must explicitly author that decision.
4. Supply the already published bank and complete arrays:

   ```cpp
   request.mode = rr::platform::VehicleWindowMode50DS::callerMaterialTextures;
   request.materials.textures = &uploadedTextures;
   request.materials.component[0] = {chariotMaterials.data(),
       static_cast<std::uint32_t>(chariotMaterials.size())};
   request.materials.component[1] = {leftEngineMaterials.data(),
       static_cast<std::uint32_t>(leftEngineMaterials.size())};
   request.materials.component[2] = {rightEngineMaterials.data(),
       static_cast<std::uint32_t>(rightEngineMaterials.size())};
   ```

All material counts, modes, required bank entries, and the borrowed bank's exact
current context/thread/publication are validated before allocating the renderer
or issuing its first GL call. Counts must match the 50CR scenes and fit the
RRSCENE1 limit of 4096 materials per component. Cold creation makes one exact-sized
resolved table, bounded to 12,288 entries of two 32-bit words. Mapping arrays are
borrowed only during synchronous `create`; no pointer into them is retained,
including on a failed creation with pending GL cleanup. Neutral mode makes no
material-table allocation and rejects nondefault material inputs rather than
silently ignoring them. All-untextured mappings may use an explicitly published
empty bank.

The 50EB owner must outlive this renderer and keep the same publication until
the renderer closes. Its nonzero `publicationToken()` changes on every successful
create, including close/recreate on the same owner. 50DS snapshots that token and
uses `validBorrow(context, isCurrent, token)` to verify exact callback/context
identity, creating thread, currentness, and publication without GL calls. Closing
or recreating the bank invalidates a later submit before any renderer GL work;
reusing the same numeric texture names cannot make an old token valid. Destroying
the owner early is still an invalid dangling-reference lifetime violation, not
something a token can diagnose. Renderer cleanup does not need to dereference the
bank, so an invalidated publication does not prevent cleanup of renderer resources.

Each required material binds its retained texture on `GL_TEXTURE0` and sets 50CZ's
`image = 0`, `useTexture = 1`. Explicit untextured entries bind zero and set
`useTexture = 0`; there is no checkerboard or missing-resource substitution. The
unchanged texture branch samples RGB and multiplies by supplied vertex RGB,
forcing output alpha to one. It does not implement original-game multipass
selection, lighting, alpha blending, texture-alpha cutout, or other material
semantics. Bank selection alone is not evidence of original pass/slot parity.

Sampling uses 50EB's immutable texture parameters. Textured mode requires desktop
core GL >= 3.2 and resolves `glActiveTexture` and `glBindTexture`. GL >= 3.3 also
requires `glBindSampler`; GL 3.2 checks the advertised extension list for
`GL_ARB_sampler_objects` and requires that function only when advertised. The
cold GL 3.2 extension scan rejects counts outside `[0,65536]`. When sampler
objects exist, each frame binds sampler zero on unit zero, preventing an external
sampler from overriding the caller's 50EB choices. No shared texture parameter is
ever changed by 50DS. If both vehicle and track are untextured, no texture-sampling
entry points are loaded.

Per-frame checks and material table access do not allocate. Required textures
are preflighted before private drawing, and binding errors abort the private
frame before publication/blitting. Unit zero remains active, its last material
texture can remain bound, and its sampler binding remains zero; state is not
restored. Closing the renderer unbinds unit zero but never deletes borrowed
textures. Explicit track drawing selects its own mode at the beginning of every frame,
including when the prior frame ended on a textured vehicle material.

## Optional cold track input

Leave `request.track` entirely defaulted to retain the vehicle-only path. To
include an actual decoded visual track, load a `Mesh48B` with `loadMesh48B`, then
set all of the following before `create`:

```cpp
request.track.mesh = &worldMesh;
request.track.maximumVertexBytes = 4u * 1024u * 1024u;
request.track.maximumIndexBytes = 2u * 1024u * 1024u;
request.track.mode = rr::platform::VehicleWindowMode50DS::viewerNeutralUntextured;
```

Alternatively select the explicit `sourceVertexColors` track-only mode described
above, or the real source-ordered `callerMaterialTextures` span path. Both use
the optional finite positive `vertexColorScale` exposure. The original
neutral shader remains byte-for-byte unchanged when neutral mode is selected.

The byte limits above are example caller-selected ceilings, not reservations.
Each supplied mesh must fit both explicit limits. The track is borrowed unchanged
only during the synchronous `create` call; no CPU mesh pointer is retained after
that call, including a failure with incomplete GL cleanup. The caller can release
its CPU staging mesh when `create` returns. Successful creation owns exact GPU
copies of its vertex and index bytes and never uploads them again in `submit`.

Before allocating the renderer owner or issuing any GL call, track validation
requires a nonempty triangle list with at most 1,000,000 vertices and 3,000,000
indices. The index count must be divisible by three and fit `int32_t`; every
index must be in range. All position and UV floats must be finite. Exact vertex
bytes (`vertexCount * 24`) and index bytes (`indexCount * 4`) must fit their
separate nonzero byte ceilings, `size_t`, and GL's signed `intptr_t` size. The
hard Mesh48B caps therefore limit track GPU payload to 24,000,000 vertex bytes
plus 12,000,000 index bytes even if the explicit budgets are larger. A null mesh
with a nondefault track mode or byte limit rejects. Empty or malformed supplied
meshes never silently select the vehicle-only path. Degenerate triangles retain
their original indices; bounds are unused and do not drive culling or collision.

Creation checks the exact uploaded track VBO/EBO bytes using bounded 1 KiB stack
scratch, their actual allocation sizes/usages, and the Vertex48B VAO layout and
associations. No CPU copy, per-frame track upload, application heap allocation,
index remapping, or geometry transform is introduced. Original world placement
must already be in the supplied positions; draw uses `baseVertex = 0`. Authentic
asset provenance and omissions in an exported world remain the caller's
responsibility. The renderer cannot infer authenticity from arbitrary mesh bytes.

On each accepted frame, the private inactive target is cleared exactly once;
the track draws first with its explicit neutral or source-color/texture shader, the same
row-vector matrix, and writable `LESS` depth as the following vehicle draws. No intervening color/depth
clear or matrix replacement occurs. The track remains in the unpublished 50CV
begin stage: a track draw failure aborts the frame before vehicle drawing or any
window blit, and preserves the previous published private target. A later vehicle
failure similarly discards the pending combined track/vehicle target. Final
default-framebuffer copying and swap permission are unchanged for window output;
headless output publishes the same combined private image without either step.

## Resize and frame behavior

For window output, `frame.width` and `frame.height` are pixels in the actual
default framebuffer, not logical window coordinates. Use the host's current
drawable size. The viewport is `(0, 0, width, height)` and the final blit is a
same-size copy of that rectangle, without scaling or a Y flip. Refresh the
caller's camera aspect when the drawable changes. Headless output uses that same
viewport with explicit dimensions independent of host surface size and no blit.

Every frame must fit the cold maximum dimensions. A zero/minimized drawable or
oversized viewport rejects before drawing. Skip minimized-window rendering; if
larger storage is needed, explicitly close and cold-create the renderer at the
new bounds between frames. No implicit per-frame resize/allocation occurs.

Rendering is opaque, solid, two-sided, with writable `LESS` depth, depth clear 1,
and linear RGBA8 private color. Blending, culling, scissor, stencil, polygon
offset, dithering, multisampling, framebuffer-sRGB conversion, depth clamp,
primitive restart, rasterizer discard, and user clip distances are disabled.
The final copy transfers color only; it does not establish default-framebuffer
depth for additional rendering. Other rendering/state changes are outside this
small owner's exclusive-context contract.

## Transactions and errors

Private submission and window copying remain distinct operations:

- 50CS/50CV first begins the private inactive target, including optional static
  track drawing, then uploads and draws the inactive vehicle geometry bank into
  that same target. Its CPU-only commit selects the new private target only after
  all submission GL checks succeed. Abort leaves the previous private target
  unchanged. Same-context command ordering protects storage reuse without a
  forced CPU/GPU finish.
- Only in `defaultBackBuffer` mode, the window copy runs after that private
  commit. It is fallible and cannot be rolled back. A GL error during or after
  the copy can leave a partially modified
  default back buffer. The public `submit` result intentionally does not promise
  display transactionality. Never swap after a false result; stop or recover the
  context and successfully redraw first. Device/context loss can invalidate any
  prior image regardless of the two-bank design.

Headless output ends at private acceptance without a fallible publication-side
copy. Readback, when requested, is a third explicit operation with separate CPU
output-failure semantics described above; it neither commits nor rolls back a
frame.

`diagnostic.batchAccepted` distinguishes private acceptance from window-copy
failure. `defaultFramebufferMayBeModified` becomes true immediately before the
blit call and stays true on success or a later failure. It is false for failures
before that call, and always false for headless submission/readback. The
diagnostic also gives a first-failure category, static operation, component,
GL error, framebuffer status, shader log, and 50CS detail.
Material failures also identify the original material and texture-bank indexes
when applicable; `material` is a distinct error category.
Presentation errors retain their operation and GL error under the `presentation`
category. No successful return claims a successful host swap.

Creation checks actual buffer sizes and usages, retained GPU index bytes with
bounded 1 KiB stack scratch, VAO layouts/associations, target storage/completeness,
and output-mode-specific GL requirements. Per-frame indices/span pointers and
counts must match the retained 50CR meshes. Original material indices are checked against the
corresponding loaded scene. Callers must not mutate those assets or private GL
objects; no names or adapter callbacks are exposed. Optional track buffers are
also private and immutable after their cold upload; their CPU input has no
ongoing lifetime requirement.

## Lifetime and cleanup

All calls, including readback, are synchronous, non-reentrant, and restricted to
the creating thread. The context must not be shared with another renderer/thread. Host polling and
swapping between submissions are allowed. The caller's context callbacks must
not throw, rebind, or release the context while a renderer call is in progress.
The renderer does not make the context current and does not save/restore other
application state.

Failed creation attempts cleanup. If cleanup cannot finish, `ownsResources()`
remains true and `initialized()` is false; retain the object and explicitly retry
`close` after restoring the required context. A failed close retains unresolved
GL names. Do not create or reuse GL object names between unsuccessful close
attempts. If `cleanupIncomplete` is false, cleanup relinquished all ownership
even if the return was false to report a stale GL error encountered during it.
The destructor also closes; unresolved teardown terminates rather than silently
leaking GL objects into the caller's context. Track VAO/VBO/EBO names participate
in the same ownership checks, conservative deletion retries, and failed-creation
cleanup as vehicle resources. The caller must keep the host alive while resolving
cleanup failures.

No build registry, context owner, existing 50CZ shader, or older 50DD renderer is changed by
this edit. A native target must include 50DS, 50CV, 50CZ, 50EB and their existing
50CS/50CO/50CR/50EA dependencies. 50EB linkage is required even when the application
selects neutral mode. No GL SDK headers, direct GL link, EGL dependency,
or SDL dependency are added by 50DS itself; its resolver comes from the host.
