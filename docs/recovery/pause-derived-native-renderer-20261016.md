# Native original Pause derived renderer

Base7b90e67. Source-only; no builds, syntax checks, tests, decoders or application
execution. No driver activation is included.

## Concrete supported path

`OriginalPauseNativeDraw50LA::initialize` strongly retains the genuine Pause
storage, creates the integrated canonical FINAL/native resources, and uploads
no pixels itself. Its immutable CPU texture bank contains only existing decoded
RRTEX bytes. A separate entry per original reference/pass preserves distinct
samplers even when passes share the same original atlas. Numeric bank identities
are native reference/pass ordinals, not claimed original texture IDs.

The actual observation owner's existing `captureLayout` creates the private
`OriginalPauseLayout50LA::Commands` sequence. Commands now carry their original
Pause owner address, so another Pause cannot consume them. Under the same serial
owner lease, `prepareDerived(commands, viewport, cameraPublication, diagnostic)`
regenerates actual packets and lowers their precise reference/pass sequence:
source1BB31C title, eleven1BD5D0 rows, conditional1BD6B8/1BD74C arrows,
then1BD768 prompt. Text uses the real paired sprite corners and equal original
RGBA; arrows retain source strip diagonal through order1,0,2,3. No generated
text, alternate atlas, recoloring or selection is introduced.

Every emission re-prepares actual FINAL maps and compares effective GS fields,
sampler state, map count/UV selection, real image identity and FINAL leaf against
the cold policies. Changed unsupported state rejects. Bounds are4096 quads,
144 packets and144 original reference/pass slots. This covers the current
finite storage and up to four declared passes without merging source calls.

## GPU path and lifetime

`VehicleWindowGl50DS::bindOriginalPauseDerived` installs the strong native owner
on an idle renderer. the frame's `originalPauseDerived` is optional and
null by default; it does not activate Pause. Actual prepared packet geometry,
source camera publication, scene/session, exact640x448 viewport, depth16777214,
material leaf and full span coverage are checked before renderer staging.

The existing source-RGBA UI shader draws into a native-size private target using
per-pass original depth test/write, raw texture and vertex channels, and supported
blend equations. Coherent framebuffer fetch or one-triangle texture barriers
preserve source-order composition. Destination-alpha-dependent materials reject
because this incoming gameplay target does not own original GS destination alpha.
The pending gameplay image receives the result transactionally; presentation
alpha is restored to its existing opaque contract. This pass follows the existing
native gameplay blur in the pending image, without asserting retail scheduling.

GPU upload uses the existing checked MaterialTextureGl50EB path. Partial upload
or target creation remains renderer-owned. Failed bind and renderer close drain
before deleting names; an incomplete deletion retains the strong native owner.
Only completed GL retirement releases the CPU bank, canonical collector pin and
Pause native borrow. The native draw owner has no externally callable close that
could invalidate renderer-held policies or source resources.

## Explicit activation blockers

This is the **derived1BB010 renderer**, not the full1B9220 callback. The genuine
outer producer remains absent: single-view order is backdrops/icon, derived,
optional base text, bottom polygon, conditional top polygon. The multiview branch
moves bottom polygon first and temporarily overrides/restores viewport. Selection
of that path uses actual race1125C, not human count or viewport inference.
Last-camera admission, actual Entity scheduling, open/close callbacks, complete
outer command geometry and reversible forest attachment are still prerequisites.
The source viewport must remain live and stable through the submitted frame.
Commands must be consumed synchronously after their authentic observation/layout
capture; their address tag is ownership validation, not a scheduler receipt.

The optional renderer path deliberately does not claim complete Pause visuals,
full callback completion or original framebuffer ancestry. Future full scheduling
must preserve one shared ordered Pause depth plane, without clearing it between
bottom polygon/backdrops/derived/base text/top polygon, rather than call this partial pass
around invented backgrounds or substitute panels.
