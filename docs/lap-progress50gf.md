# 50GF: source-gated player lap/progress effects

## Delivered scope

`src/game/lap_progress50gf.h/.cpp` supplies the previously excluded **accepted map-crossing effects** of player `1A0DA0`: authored progress-marker consumption (`FF8`), shortcut-suppression bit handling (`CD8` bit 2), and the actual lap-counter increment (`CC8`). It does not infer a lap from a graph index decrease or a route wrap.

A counter increment is **not completion of the lap callback or the race**. The component returns an explicit pending `1A2B80` request and stops before `1A0F2C` resets `FF8`. The caller must retain that request until the actual native callback has finished. No fake completion, no assumed race length, no winner, no countdown, no audio, no timing record, and no synthetic game-state dependency are supplied.

All work is bounded and allocation-free. It touches only caller-owned records. No native callback is invoked by this component; no guest memory, executable execution, device access, filesystem I/O or hidden static state is used at runtime. Failure preserves state, continuation and receipt. Calls require serial ownership and disjoint backed records.

## Real source branches and order

1. At each accepted map crossing, compare the crossed node with the next authored progress marker. Increment `FF8` only when it matches and `FF8 < track+48`.
2. For a **sequential** crossing only, examine the live pre-store `C88`:
   - If it is not node 0, no lap branch runs.
   - If `CD8` bit 2 is set, clear that bit and stop this lap branch, even if bit 0 is also set or all markers were consumed.
   - Otherwise, only when `CD8` bit 0 is clear and `FF8 == track+48`, increment `CC8` with low-word wrap and set `CD8` bit 1.
   - Request `1A2B80` at call PC `1A0F24`. The real call has `a0=player`, `a1=0`, and `a2=the CD8 byte loaded before setting bit 1`. The request captures those scalar arguments and the actual field values at the call.
   - Stop there. `resumePlayerLapAfterCallback50GF` represents only the subsequent `1A0F2C` store `FF8=0`, after the real callback has returned.
3. For a **scan** crossing, no lap increment branch exists. After marker handling, set `CD8` bit 2 only when live `C88==1`, player `10CC` bit 1 is set, and the accepted scan node equals `2A26E0(map,0)`.

The scan condition reads **current C88**, not the accepted node number. Later accepted scan candidates must see earlier candidates' progression stores. `2A26E0(map,0)` returns the first authored node whose `Next0` is zero, not necessarily the last map node.

`2A2FD0` reads `track+4C+4*FF8` before the source count comparison. At `FF8==count`, that read cannot enable any FF8 increment: even a match fails the signed less-than test. This effect projection elides that inert read rather than fabricating its value. This matters when `count==10`, where the source read reaches the unrelated `track+74` field. The represented domain is `0 <= FF8 <= count <= 10`; within that domain the same state stores and callback branches result. No meaning for `track+74` is claimed.

## Preparing actual authored markers

Immediately after a successful **map** `readTrackNavigation50EU`, pass its still-current `TrackNavigationScratch50EU` to `prepareLapProgressTrack50GF`, before the scratch is reused for AI or another track. The existing reader retains actual ProgressNode presence in `fieldMask[node] & 16`. It does not retain the marker table in `TrackNavigation50EU` itself.

The source parser (`2A18F0..2A19C0`) uses key presence, ignores the numeric ProgressNode value for ordering, walks nodes in ascending authored index order and appends each present node to `track+4C`, incrementing `track+48`. It admits at most 10. The source constructor initializes the table to zero. Neither `NodeHeight==10000` nor authored distances can substitute for the presence mask. The helper copies that genuine parser result and derives the exact node-zero predecessor from the authored `Next0` graph. It fails explicitly if the required predecessor does not exist.

## Central navigation join

The parent owns integration; this component does not edit `track_navigation50eu`, frame or client code.

- At `1A0E60`, after the accepted sequential map gate and **before** `C8C=C88; C88=Next0`, call `applyPlayerLapCrossing50GF` with kind Sequential and both node inputs equal to the current live C88.
- When it requests the callback, commit CC8/CD8/FF8 as returned and suspend the enclosing source continuation before any map progression stores. Complete the actual native `1A2B80` behavior, reflect every callback mutation into the player owner, then invoke `resumePlayerLapAfterCallback50GF`. Resume the navigation loop at `1A0F30`, which rereads live C88. Do not overwrite possible callback changes with a saved pre-call C88.
- At `1A10D0`, after every accepted map scan candidate and before scan C8C/C88 stores, call with kind Scan, the accepted candidate, current live C88 and the actual player10CC byte. Preserve source ascending candidate order. Then perform the original candidate's progression stores.
- Do not call from AI crossings, from a post-frame before/after index comparison, or once per receipt count. An end-of-call navigation receipt loses the accepted candidate sequence and cannot reconstruct these branches.
- A pending callback rejects another crossing. A missing callback implementation is a genuine integration blocker at the first qualifying lap, not permission to consume the request. Source `1A0DA0` currently can complete only its existing four-index projection; integrating this component requires the enclosing continuation to honor this boundary.

## Caller initialization and other writers

Two separate APIs preserve distinct source lifetimes:

- `initializePlayerLapCursor50GF`: `FF8=0`, from player construction `1A0360` and the explicitly admitted full-initialization store `1A258C`. It does not reset CC8 or CD8.
- `initializePlayerLapRaceFields50GF`: `CC8=0`, clear CD8 bits 1, 0, 2 from `21669C..216704`, preserving all upper CD8 bits. That source block does **not** write FF8.

Default aggregate zeros are storage defaults, not authorization to reset live state. Do not reset during undertrack recovery or from a route wrap. Initialize a fresh empty continuation only for genuinely fresh ownership. `FF8` has additional source writers outside this component, including player path `1A2E8C` assigning the marker count; callers must continue supplying their actual live value. The byte `10CC` is read-only input here, not a synthesized constant.

## Exact callback boundary, not a no-op

The retained original `1A2B80..1A2DD0` contains:

- `1A2BA0..1A2BB8`: player `D40 -> 488`, then `292360(object,1,0,0)`.
- Global `382B80 -> 2DC`, configuration `+11258` (race mode), `+1125C` low 5 bits (configured lap count) and `+1125F` low 4 bits.
- Player `CB0`, `CB4`, `CC0` lap-time storage and CC8 consumed by timing/record branches. Calls `1861A0`, `186280`, conditionally `22B170`, `22B370`, and `22B760`.
- Mode 2 bypasses the timing-record block after the initial `292360` call. It does not bypass that initial effect.
- Mode 3 also calls `217940`, clears player `7C`, and calls `217420`. `217940` contains virtual calls, so their concrete behavior cannot be assumed inert.

This component does not assume those calls leave other player or navigation fields unchanged. Implementing the callback is the next source dependency; setting its request as completed without those concrete behaviors would be a fabricated lap-system claim.

## Provenance and checks

Original ELF: `work/native/project/disc/SLUS_202.68`.
SHA-256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Existing complete player listing: `evidence/trackNavigation50EU/source1A0DA0.txt`.
Existing predecessor listing: `evidence/trackNavigation50EU/source2A26E0.txt`.

Only retained original ELF bytes and existing native sources/docs were read. No builds, tests, guest execution, user PC access, excluded49C, held48U, ee.bin, or sealed archive edits were performed for this component. Central integration and compilation remain with the parent.

The following original-byte extents anchor the additional dependencies (end exclusive):

- `002A2FC0..002A2FE0`: `e33c1df55c9bfb425b86aeb3721c982dcbb97a26f8bb7e81221706e1bb555a17`
- `002A1180..002A11C0`: `f87893648af37f38e161a1bd4f8a36d0d55eda7ef23baa7f923763847a930eda`
- `002A18F0..002A19C8`: `2b86c68f47f9bbc467d3519ed22497eb3540ab691f4b6229ec264cab2f867a82`
- `001A2B80..001A2DD0`: `011e9c7ed89871f1e3a0574a37c3b240168e21b8b3f831b8d4cd11c169ba14cd`
- `001A035C..001A0364`: `466b22b6134c6efa51a1be7e45088cc74c1c34a8af51a09995f29de2b9ad36a4`
- `001A2570..001A2590`: `c9641fec9c51007677ddd87fb01a93f0a4ce8c55db80f7c8f5200254123caebe`
- `00216690..00216708`: `03dadc95a769b572a37653fee0de529e27961ca3f4ce8b164d330a0c46259ba7`

### Original getter and authored-table stores

```text
002A2FC0 03E00008 jr ra
002A2FC4 8C820048 lw v0, 0x48(a0)
002A2FC8 00000000 nop
002A2FCC 00000000 nop
002A2FD0 00051080 sll v0, a1, 2
002A2FD4 00441021 addu v0, v0, a0
002A2FD8 03E00008 jr ra
002A2FDC 8C42004C lw v0, 0x4c(v0)
002A1980 8E620048 lw v0, 0x48(s3)
002A1984 2841000A slti at, v0, 0xa
002A1988 1020000F beq at, zero, 0x2a19c8
002A198C 00000000 nop
002A1990 8E630030 lw v1, 0x30(s3)
002A1994 3C02461C lui v0, 0x461c
002A1998 34444000 ori a0, v0, 0x4000
002A199C 001011C0 sll v0, s0, 7
002A19A0 00431021 addu v0, v0, v1
002A19A4 AC440050 sw a0, 0x50(v0)
002A19A8 8E620048 lw v0, 0x48(s3)
002A19AC 00021080 sll v0, v0, 2
002A19B0 00531021 addu v0, v0, s3
002A19B4 AC50004C sw s0, 0x4c(v0)
002A19B8 8E620048 lw v0, 0x48(s3)
002A19BC 24420001 addiu v0, v0, 0x1
002A19C0 AE620048 sw v0, 0x48(s3)
002A19C4 00000000 nop
```
