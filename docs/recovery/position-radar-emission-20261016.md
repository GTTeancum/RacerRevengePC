# Original radar point emission, source-only candidate

Base 30528592be354e0df55e7b6033cbaf3b7509a812. No builds, tests, syntax
checks, sanitizer checks, gameplay, guest execution or independent decoder
checks were performed. Original executable was read only for source words.

## Implemented boundary

The resource owner's private appendRadarPoint192C70 uses its actual projection
fields and actual clip owner, then writes the existing primitive owner's source
vertices and packet aliases. This connects the recovered projection and clip
leaves to genuine allocated radar geometry. It is deliberately uncalled until
the original route traversal supplies the inputs. It does not expose an
arbitrary public geometry setter or manufacture observations.

192C70 first obtains the route point through 2A2410, then calls1922B0. This
candidate starts after that lookup. The element is an actual one of16, and the
point is one of118. In type5, source indexing is point*0x60, so only the first
vertex of each pair is written. The original +14/+18 aliases are STQ, not XYZ.
Every existing packet alias receives the same full four-lane source value;
RGBA alpha receives (alpha>>2)&255. RGB and XYZ stay as originally owned.

For point0 no prior endpoint is inspected. Later points read the first prior
packet alpha byte and multiply by4. An outside point following positive alpha
clips previous-to-current; its clipped second endpoint is appended with alpha0.
An inside positive-alpha point following zero alpha clips current-to-previous;
its clipped second endpoint replaces the previous source/STQ aliases with
alpha0, then the current point is appended. Inside points of alpha0 and other
transitions do not invoke clipping. Output scratch W is zero, matching original
zeroed segment outputs; clip119FC0 writes XYZ only. Projected input Z/W are zero.

Both aliases are updated for overlapping strip endpoints. Odd pair vertices,
RGB, XYZ, element active counts, dirty bits and packet counts are untouched.
The returned alpha is the original192C70 value used by1924C0 to decide whether
a point participates; alpha0 still writes geometry, and is not an error.

Native admission is explicit: invalid indices, absent aliases, nonfinite
projection and unsuccessful/unsupported clipping return a diagnostic without
publishing this point's writes. Original192C70 ignores clip119FC0's return;
this bounded implementation rejects that exceptional case rather than emitting
zero-initialized scratch output. Earlier successful points remain unchanged,
not rolled back. The exact clip result is preserved in the primitive diagnostic.

## Exact remaining integration/draw dependencies

1. Recover/join actual1924C0 view racer manager lookup1AF330, body+340 transform
   28B1A0 and heading+E4, with actual owner pose cosine/sine routines13BBA0/
   13BAD0. No host camera position or selected-racer override substitutes here.
2. Recover route manager+B8 queries2A2600 (skip),2A26E0 (group),2A26C0
   (closure endpoint),2A2410 (world point). Preserve source order, group
   changes, alpha-positive participation, closure point,118/16 bounds and
   partial-write behavior. Wire those actual points into appendRadarPoint192C70.
3. Recover element finalization2E2450 and2E21D0 called192624/192650 and
   1927B0/1927DC, including type5 doubled active count and dirty3C publication.
   Source coordinates are in STQ here: native ScreenObject lowering must recover
   the intended conversion rather than treating these as ready host XYZ.
4. Recover original1924C0 marker path: manager loop excludes current view,
   rejects state40!=0, uses map[117-index], applies projection minus3 plus
   layout offsets, alpha/4 and RGB107/9/23; special subtype3 marker map117
   has its own entity-bit gate. This patch does not emit marker textures.
5. Preserve191BE0 gates and preference17FCD0 before traversal/draw, and base
   Entity draw ordering. Complete zero-map RadarOverlay material pass state,
   canonical FINAL representative receipt, ScreenObject vertex lowering and
   actual renderer submission. Do not invent map0, a texture or a GPU receipt.
6. Root alone activates staging/layout/text/radar and performs integrated
   validation. No runtime or visual success is claimed by this patch.

Actual lifetime pins, both drain phases, reverse element retirement and canonical
material release remain unchanged. All racer/AI/rank/control code is unchanged.
