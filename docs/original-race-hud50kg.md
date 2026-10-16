# Original race HUD text owners, source-only packet

This packet supplies native race-lifetime TextOverlay ownership for the actual race arrays reached by `1AD230`, plus the existing lap callback dependency. The later [bounded lap text/gleam GPU join](original-race-hud-render50kg.md) adds actual rendering for the four joined lap text objects while retaining native gauges and other HUD elements. No builds, tests, syntax/import/lint checks, sanitizers, gameplay runs, project asset decoders, guest/VU execution or PC operations were performed during this source work.

## Authenticated source

SLUS_202.68 SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Construction `1A8CC4..1A9058` allocates eleven separate TextOverlay objects per active race1B0-low2 view, each through `29C850` then `29CA40` with capacity40 and that view's real scene. The first loop builds seven objects at `12C+28*view+4*index`; the second builds `17C+4*view`, `164+4*view`, and `16C+8*view`/`170+8*view`. All receive `29DC60(1)` from source constant3C4740. The inactive second view is genuinely absent because construction's loop never visits it.

The source literal bindings are:

- Seven12C: denmarkbold14ingame CEL/PST, denmarkbold14gleam material
- 17C: denmarkbold16ingame CEL/PST and its original missing-PSM ScreenMT branch
- 164: denmarkbold30ingame CEL/PST, denmarkbold30gleam material
- Two16C: denmarkbold16/30ingame CEL/PST, denmarkbold16/30explosion materials

All exist in authenticated OVERLAYS. APPSTART owns actual ScreenMT and the material registry. The fallback is restricted to exactly the real in-game16 CEL/PST, absent PSM, and constructor1A8DD4. It does not borrow a menu widget identity. Existing native resource readers load fonts, textures, materials and localization only when the application executes this source. Preparation used bounded raw archive/text extraction, authenticating the whole archives and retained member hashes; it did not run those readers.

`29C850` initially disables each overlay. Empty initial text is the source's actual `29CA40 ->2DFD90` construction, not an empty-array completion shortcut. Native storage retains40 text bytes plus terminator, independent mutable animation fields, ScreenText caches, actual font/material associations and the borrowed stable viewport. Every active array element owns a distinct object. Resources outlive them; the race owner is neither copyable nor movable.

## Stop and lap dependency

`stopAll1AD230` traverses both view slots in exact helper order:17C,164, then seven12C entries interleaved with the first two16C entries. Each real text object receives the four unconditional29DB10 writes:70=0,88=0,84=-1,CC=0. Other animation fields and the gleam owners remain unchanged. There is no callback that can manufacture completion.

Publishing these arrays makes existing human lap fields non-null. The lap adapter therefore binds their actual native identities and routes22B170,22B370 and22B760 into their real setters. It does not keep reporting the earlier null branch. Record text uses authenticated LOC entries463/464 and the source189000 bounded hundredths conversion; the literal banners are NEW RECORD! and FINAL LAP!. Lock suppression, colors, single/split viewport positions, hold2.5, delayed fade and forced lock are retained.

The unconditional22AE70 tail and final-lap tail also need gleam194/184. Both are real two-ScreenTexture native state owners with authenticated gleam/gleamclear material and gleam PST ownership. The source width query uses the real font and the prior ScreenText scale, including lock-suppressed text replacement. Signed width/2 occurs before binary32 placement; travel uses the source binary64 multiply by1.2.2E6940 leaves an already active gleam's elapsed time intact.2E6E50 advances and expires it.

## Native scheduling/presentation boundary

`advanceAnimations29CDA0` runs once before a real advancing world's racer prefixes; lap setters occur later in that step. It implements the animation fields reachable through these lap callbacks, including the next-update second-phase crossing and hold-owned unlock. There is no numeric-count animation or arbitrary moving-position admission. Source arrays, elapsed clocks and locks survive repeated stop, repeated lap calls and native per-racer arena switches.

The explicit bounded native HUD presentation policy commits ScreenText color/alpha/scale/rotation caches only after actual completed gameplay display, for the matching real viewport and visible HUD. The viewport-owned29D464 path applies these caches independently of dirty flags and copies overlay justification74 into ScreenText justification50. This cache commit alone is not evidence of GPU work. The separate lap renderer reports nonempty checked GPU submission, and the host distinguishes that from completed display. Existing native HUD continues to draw its other presentation and remains the fallback when the optional lap layer cannot prepare.

Root integration appends the actual HUD owner pointer to the camera Session ActualOwners aggregate. The Session may resume176800 only after the real loaded owner's stopAll1AD230 returns. Missing resources, malformed resources or missing view ownership do not publish a loaded HUD.

## Integration inventory

New translation unit: `src/game/original_race_hud50kg.cpp` only. Root must add it to Linux CMake, Linux interactive generator and Windows explicit sources, updating their existing count/provenance metadata centrally. Material parser changes add one tightly bounded missing-PSM branch. The50DU and lap_drive changes are staged as narrow hunks because other workers own camera/death/reset integration. Root owns applying and reconciling them.

The explosion rendering owner188 and general status/countdown/KO text emitters are outside this packet. Their two text objects exist and are correctly stopped by1AD230, but this packet does not claim those emitters ran. Full retail HUD rendering, broader screen lifecycle and Results/profile/save/progression are still separate work.
