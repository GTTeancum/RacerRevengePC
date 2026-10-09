# 50FD: actual Pod01 connecting cables

## Result and scope

`src/game/pod_cables50fd.cpp` builds the two ordinary connected Pod01 cables
from the current graphics scene. This is a native geometry generator derived
from the original `PodCables` source words. It does not invent straight lines,
sag amounts, sample counts, thickness, textures, or animated endpoint poses.

The generator emits 36 world-space `Vertex48B` vertices and 96 triangle-list
indices. Its caller owns all storage. Every draw rebuild uses the current
scene attachment matrices and view direction. No physics, scene, cached world
matrix, noise, RNG, or original asset is changed.

This slice admits `vehicleState40 == 0`. Original detached-engine states1/2,
nonordinary states3/4, distance/camera-mode culling, ghost material switching,
cable shadows, complete shader/fog behavior and original GPU pixel equivalence
are outside it. An unsupported state returns a distinct error instead of
silently drawing attached cables.

## Recovered original source

Input ELF: `work/native/project/disc/SLUS_202.68`, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The source evidence contains original address/word pairs plus readable scalar
MIPS disassembly. Unrecognized PS2 vector instructions remain explicit raw
words. No original executable was run to discover or display these cables.

| Original range | Recovered behavior |
|---|---|
| 211B68–211D54 | Named drawing attachments, with fallback to physics attachments only when the draw node is absent; engine cable endpoints are independent named scene nodes |
| 214120–214254 | Number of connected cables; `8 + 1` samples per cable |
| 173EB0–174358 | Four Hermite basis arrays; tangent multiplier6.5; UV U0/1 and V10*t; each ribbon has18 vertices |
| 224720–2255A8 | Graphics-side endpoint update; current chariot world axes; cachedSpeed34-dependent world-Y tangent flattening; original nonordinary/disconnected branches |
| 1743E0–1748FC | Hermite evaluation, camera-facing cross product and normalization, one-sided0.25-width ribbons; last sample reuses last segment direction |
| 174900–174A24 | Submission of computed ribbon positions to the existing geometry owner |
| 28A700–28A884 | World forward is matrix row2 and world up is row1 |
| 3C9490–3C950C | Source initialization of fixed zero/up vectors used when creating geometry |

### Actual endpoint resolution

The original constructor looks for `drawcableleft1`/`drawcableright1` beneath the
chariot. Pod01 has neither, so the source fallback is
`chariotcableleft1`/`chariotcableright1`. These are PSG rows4/5, current graph
nodes35/36, beneath `nullcg0`. They are not the hand IK targets.

The other ends are `enginecableleft1`/`enginecableright1`, PSG row7 in each
engine, graph nodes8/23. Engine model roots0/15 are used by physics and engine
response, but are not these authored render attachment points.

The cold fixture resolves names and verifies the complete scene parent/index
mapping and lifecycle fallbacks. The hot function validates each endpoint as a
descendant of its expected model, snapshots all53 nodes, and calls the existing
49P world refresh on that snapshot. It uses those current graphics worlds,
not a retained image, offscreen showcase, fixed root pose, or later body scene.

### Exact represented curve

The sample parameter is `t = sample/8` for sample0 through8. The original basis
is evaluated in binary32 source order:

- h00 = 1 + (2*t - 3)*t*t
- h01 = 1 - h00
- h10 = t*(1 + t*(t - 2))
- h11 = t*t*(t - 1)

For ordinary state0, let F and U be current chariot world forward and up.
Set the chariot tangent direction to -F and the engine tangent direction to
U-F. Scale only each direction's world-Y coordinate by the source clamped
linear mapping of cachedSpeed34 from `[0, float(0x4392AA7F)]` to `[1,0]`.
Normalize each and multiply by the source6.5.

The centerline is `h00*engine + h01*chariot + h10*engineTangent +
h11*chariotTangent`, preserving the source's multiplication and addition order.
The original ribbon runs from that centerline to centerline plus
`0.25 * normalize(viewAxis cross (sample - nextSample))`.
There is no arbitrary fallback axis when the cross product is zero.

`cameraForward` in the API means the original view matrix's column2. For the
current50DG RH look-at camera, pass the world `eye-target` direction. Its
magnitude cancels in the source normalization; it must be finite and nonzero.

## Actual texture and submission

`textures\\cable.psm` in Pod01.RES is the original one-pass `StripOptFogCode`
material. It references `TextureMap_1=cable`, explicitly enables texturing and
explicitly sets `alphaBlendingOnOff=0`.

Source PSM SHA256:
`701039c97a0f4b74a65e90fdb1e386b40cbbb17bcedb2347196884234d0eee33`.
Source PST SHA256:
`e6b0abd320de50c784816a4e15dc6bfa056c8241a173eb26481243ff84dc301f`.
The cable is an original64x64, 16-entry indexed kind0 texture. The legacy48E
kind1-only decoder correctly rejects it; the source-derived50FC cable-only decoder handles this original format,
without manufacturing pixels or modifying the original header. It proves
low-nibble-first texels and identity16-entry CLUT ordering. The exact RRTEX001
SHA256 is `10329f55fd0edd413b71c74915b6694700beee922c36255c410099eccd903d64`.

`tests/native/pod_cables50fd_fixture.h` authenticates the exact archive and
members, verifies the material's actual slot/opaque policy, resolves the named
attachments and publishes a separate50EA bank for the original cable texture.
Source RGB/alpha is kept unmodified. Vertex bytes are neutral-white texture
modulation because this compatibility layout needs a color, not an assertion
of recovered shader lighting. Render opaque/two-sided with repeating UVs in
the same depth target and current frame camera as the actual pod and track.

## Host integration

1. Cold-load `pod_cables50fd_fixture::Owner` after `VehicleDrawAssets50CR`.
2. Save `owner.bindings()` and upload `owner.bank()` to a separate50EB GL bank;
   its single cable texture is index0. Keep the GL bank alive until renderer close.
3. Keep `PodCableMesh50FD`, `PodCableWorkspace50FD`, and diagnostic in host-owned
   storage. Call `buildPodCables50FD` after current graphics preparation with
   the same camera used for the pod draw.
4. Submit all36 vertices and96 indices in the same successful frame. Do not
   submit an old mesh after a failed build.
5. Add `src/game/pod_cables50fd.cpp` to the native build source list. No49W file
   or existing sealed implementation is changed.

## Focused validation

`tests/native/pod_cables50fd.cpp` checks actual attachment-coordinate inputs,
endpoint agreement,9samples per cable,0.25 strip width, UV repeat10,96 bounded
indices, engine movement through the Hermite weights, source speed bow,
canonical frame preservation, and transactional bad binding/state/input paths.
Test root placement is explicitly synthetic and is never presented as a game
screenshot or authentic spawn result.

Build and run:

```
g++ -std=c++17 -O2 -Wall -Wextra -Werror -fno-fast-math -ffp-contract=off \
  tests/native/pod_cables50fd.cpp src/game/pod_cables50fd.cpp \
  src/game/scene_graph49p.cpp -o /tmp/pod_cables50fd_test
/tmp/pod_cables50fd_test
```

This focused native test passed in optimized and AddressSanitizer/UBSan builds.
The core object has no allocator/free references. The actual64x64 cable texture
was decoded and loaded into its50EA bank successfully in a separate cold smoke. Live in-game rendering and visual acceptance
remain the host integration's responsibility; no game launch or broad
regression run is claimed here.
