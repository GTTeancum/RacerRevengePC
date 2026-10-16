# Original Speed/Boost HUD: source entry and inventory

Static original-file inspection only. No guest execution, emulation, project
execution, pixel conversion or software test. No Speed/Boost runtime activation.

Original ELF data SHA256:
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30

## Identity and owner

Race HUD construction at1A8B24 allocates0x6F0, constructs189450 with argument1,
loads via virtual+30, stores race+C4, and binds through18AD30. The original
failure string at3AA830 says `Speed/Boost overlay not created.`
Vtable3CE920 includes loader1895B0,40=18BA70,44=18BBB0,48=18BC70,
and64=18BD60. Constructor creates two direct ScreenTexts at+40/+B0,
fifteen ScreenTextures at+120 with stride60, and owns a separate+6C0 primitive.
The triangle/partial-fill path still requires complete source closure.

## Known numeric source, not an invented physical unit

18BC24..4C selects the genuine followed racer, reads its body340 cached speed34,
multiplies by binary32 bits3F2E8BC1, and stores the result at overlay+30.
The ordinary18C3D8..F0 display branch converts this field with CVT.W.S and
formats `%3d`. Alternate debug modes1/2 read actual Application fields instead;
they must not be confused with ordinary racer speed. This proves the native
numeric domain for the display, not an MPH or KPH label.

Direct text sources:
- 3A8E40: eurostileboldbolditalic26ingame, numeric text+40, color0xFFFDFF67.
- 3A8E60: eurostileboldbold13ingame, text+B0, color0xFF669BCC.
- Actual font-list table382C08 contains those two pointers in that order;
  load282430 is called at18A99C with count2/reload0.
- Text capacity40; initial numeric string3A8F20 is empty.
- Authentic English localization456 is STR_IN_GAME_HUD_TEMPERATURE=`TEMP`,
  called at18ABA4. This is not a speed-unit string.
- Original justification data3C4758=4 and3C4738=0.

## Authenticated assets

OVERLAYS.RES is154528 bytes, SHA256
ab51e88aef70d4a619ca3e614c64b90bad07deb3b9f0ec918577a44a8effb15c.
The existing original UI catalog authenticates:
- ui/textures/spdbost1.pst
- ui/textures/boost_a1.pst through boost_a9.pst
- font/eurostileboldbolditalic26ingame.cel, .pst and .psm

Thirteen exact raw members were extracted with bounded v3 block decompression,
checked against catalog lengths/offsets/SHA256. They remain original bytes;
no native game decoder or image conversion was executed. Shared13ingame source
members were already authenticated for TimeStats.

There are no corresponding spdbost1/boost PSM members in this authenticated
archive/catalog. Preserve original ScreenTexture missing-file construction
through genuine archive absence and actual ScreenMT. Do not synthesize PSM files
or silently reuse the TimeStats stats material. Existing canonical fallback
admission is narrowly Tournament-specific and does not yet admit this owner.

## Remaining native joins

The Application font registry currently supports13ingame and16ingame only.
Adding26italic must preserve genuine insertion order, reload0 reuse, texture
leases, full population side effects and actual cold ownership. A different font
atlas or host text is not a replacement. Texture fallback proof, full source
layout/gates, segmented temperature fill and the separate triangle, actual racer
bindings and checked renderer retirement remain before any implementation is
called gameplay. Current source recovery is still in progress.

## Immutable fallback proof reader

A source-only reader now admits exactly spdbost1 and boost_a1..a9 from the
actual authenticated OVERLAYS owner, requires the real ScreenMT and known
runtime state, verifies each PST and missing corresponding PSM, and produces
only the existing2475C0 NULL-config projection. It preserves null PSM identity
and empty authored sections. Texture and triangle provenance are distinct.
No generic missing material, canonical table reference, population, live HUD,
renderer or startup caller is added. No software test has run for this reader.

## Bounded font-pair staging

The source-only Application font registry extension adds the authentic italic26
entry and a private18A99C pair operation (26 then13). The existing193A0C pair
remains13 then16 and does not acquire the unused third entry. Published entries
now retain explicit insertion order for282190 cleanup rather than enum order.
Reload0 still reuses an existing Entry and its source texture reference. New
acquisition remains cold-only and rejects outstanding Entry borrows; no live
population admission was relaxed. No Speed/Boost startup caller is added.

## Private CPU state kernel

The unactivated native state header now preserves the fifteen source rectangle
states and exact UV constants, view-mode offsets, ordinary speed conversion and
`%3d` spacing, segmented temperature arithmetic and source submission sequence.
The separate three-vertex triangle starts with source Z/W=1, applies the first
dirty kind2/angle0 transform only at its real submission, and preserves later
direct layout/heat writes to vertex/primitive aliases. Idle icon0 remains a
second submission. The bounded accepted input is real Boost50FK heat[0,1] and
states0..2, finite representable ordinary speed, no debug format/input path.

This kernel does not own canonical materials, bind a racer, schedule blink,
certify Application/global draw gates or submit GPU work. The actual owner must
apply geometry preparation at each original sequence point and enforce those
remaining joins. No build or software test was run for this header.

## Source-faithful later font append correction

The initial Entry-use_count acquisition restriction above was too broad for the
actual loader order: Speed/Boost holds26/13 while later TimeStats loads16.
Original29E340 skips every already-published row and creates only pending CPU
resources; appending rows preserves resource addresses and immutable backing.
It performs no material-collector rebuild or native GL mutation.

Replace that acquisition restriction with canonical validation of every existing
Entry before new acquisition and immediately before/after the unchanged full
texture population scan: exact domain/context, live positive token, identical
constructed nonaggregate resource, positive source references and byte-equal
retained PST. Reacquire row views after any append. Preserve original font load
order and reload0; no preloading, fake cold flag or replacement font is added.
The close282190 outstanding-borrow guard remains required. No runtime check has
run for this source correction.
