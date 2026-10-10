# Named native race standings, 50HY

Status: integrated and exercised in the full eight-racer race at 09:16 UTC on
2026-10-09. Final capture shows all eight names, correct sealed order, three
completed laps each, and FIN. Normal-size gameplay and finish rendering passed;
small/resized windows and synthetic invalid-input cases were not exercised.

## User-visible change

The existing lap/place/countdown/status HUD is unchanged. A compact eight-row
standings panel appears beneath it during the original-roster race, using the
same native 5x7 text and rectangle batch. Each row shows current place, the
complete verified original racer name, a `DONE` completed-lap count, and `FIN`
once the race owner has sealed that racer as finished. The player row has a
cyan name/rank and dark-blue highlight.

The original UI comments provide these names, in racer-ID order:

- POD01: Anakin Skywalker (ep1)
- POD03: Clegg Holdfast
- POD04: Dud Bolt
- POD07: Gasgano
- POD08: Mars Guo
- POD09: Mawhonic
- POD10: Aldar Beedo
- POD11: Ody Mandrell

The existing font draws ASCII letters in uppercase; the stored original names
are unchanged, including the player's `(ep1)` suffix. K, J, Q, Z and parentheses
are now admitted glyphs. No downloaded font, new asset, web UI, abbreviated PHY
label, nickname, or fabricated name is introduced.

## Source and ordering contracts

`race_session50HM::Session::ordered` is the only row-order source. The caller
copies each indexed racer's committed `place`, `observed.completedLaps` and
`finished`, after checking the observation exists. No additional sorting,
nearest-track ranking, synthetic progress, finish detection or racer-bank
visits occur. Existing countdown, lap ownership, finish order, physics,
opponent stop handling, player controls and race restart remain unchanged.

Opponent names already passed the visual owner's original UI-comment and pod
speech checks. That exact block is extracted to `verifiedDisplayName` and is
still called by every original opponent visual owner. POD01 now uses the same
check through `loadVerifiedDisplayName`, rather than a hand-written label.
Its selected catalog row must match the actual POD01 archive digest already
authenticated by startup. The original UIACCEPT hash, source archive and
generation identities, byte-offset comment match, speech hash and original
pod speech-prefix association remain required. No additional visual, physics,
or audio owner is constructed just to obtain the player's name.

Names are copied once during cold race setup to fixed 25-byte buffers. A name
longer than 24 characters or empty name is rejected, never truncated. The
renderer accepts bounded, terminated ASCII letters/digits/spaces/parentheses.

## Renderer contract and limits

`VehicleWindowHostHud50DS` appends a zero-disabled `standingsCount`, explicit
`standingsPlayerIndex`, and eight inline `VehicleWindowStanding50DS` rows.
Each row has its fixed name, racer ID, place, completed-lap count and finished
flag. Default HUD callers preserve their previous presentation.

Enabled standings require a complete race field of at most eight, one unique
ID per row, sequential places, valid names and completed counts from 0 through
999. Finished rows must precede unfinished rows and have reached the lap
target. The player's place, displayed lap and finished state must match the
existing race HUD. Invalid data rejects before frame GL work; the renderer
never repairs an inconsistent order or fabricates a result.

The added panel is 222 glyph cells wide and 110 high for eight racers. Its
normal glyph scale matches the existing HUD. Only this new panel shrinks when
necessary to fit between the unchanged top race readout and bottom damage
panel, while reserving the right-side route cue. In a viewport with no free
vertical space between existing HUD panels, the added standings are omitted.
Existing HUD positions and glyph sizes are never reduced to fit standings.

The existing cold HUD batch grows from 1,024 to 8,192 quads: at most 768 KiB of
CPU vertices, a same-sized GPU vertex buffer, and 192 KiB of immutable indices.
There is no new GL resource type or frame allocation. The fixed bound covers
the old 1,024-quad allowance plus eight worst-case 31-glyph rows, thirteen
heading glyphs and all rectangles, using the conservative maximum of 21
horizontal runs per 5x7 glyph. The existing overflow guard, synchronous
publication, indexed draw and GL-state restoration paths remain intact.

## Root-owned verification still needed

- Compile the integrated source, including the extracted name helper.
- Confirm original-name startup, with no shortened or substituted names.
- Inspect countdown, overtakes, tied progress, lap changes, stopped opponents,
  player finish, other finishers, full completion and restart.
- Confirm rows follow `race.ordered`, sealed finishes remain ordered, player
  highlighting follows the player after place changes, and `DONE` starts at 0.
- Inspect normal and small/resized viewports with route cue, warning states,
  heat/damage, countdown and finish text all enabled.
- Exercise invalid count/index/order/name/lap/finish inputs, maximum-length
  names and 999-lap formatting; verify overflow rejection and prior GL state.
- Confirm no successful hot-path allocations and race-disabled HUD continuity.

The full-race normal-window flow and final eight-row results are now observed. Other listed cases remain untested.
