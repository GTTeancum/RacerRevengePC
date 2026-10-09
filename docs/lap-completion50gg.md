# Native player lap-completion callback (50GG)

This component recovers the control flow and direct operations of original
`1A2B80`, as entered by the map-gate progression code after incrementing player
`CC8` and setting `CD8` bit 1. It pauses at every external helper, with explicit
caller-owned return boundaries. It does not implement those helpers, invent
records, acknowledge missing effects, or reset the progression cursor `FF8`.

The new source files are `src/game/lap_completion50gg.h/.cpp`. No build or test
was run for this change. The implementation was reviewed against the immutable
source words below; this is not a guest-execution or numerical-oracle claim.

## Immutable source

Original `work/native/project/disc/SLUS_202.68`, SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

The ELF was read as source, never executed. The relevant PT_LOAD mapping is
`file_offset = VA - 0x100000 + 0x80`. The complete half-open range
`[0x1A2B80,0x1A2DD0)` has SHA-256
`011e9c7ed89871f1e3a0574a37c3b240168e21b8b3f831b8d4cd11c169ba14cd`.
No RAM image, excluded fixture, or PC connection was used.

## Recovered order

1. `1A2BA0..1A2BB8`: read player `D40`, then its `488` owner; call `292360`
   with `a1=1`, `a2=0`, `a3=0`. This mandatory owner-selection/dispatch boundary
   happens before any timing or configuration read. Its return is ignored.
2. `1A2BBC..1A2BE0`: load current game global `382B80`, its `2DC` configuration,
   and configuration word `11258`. If that word is 2, jump to the final mode
   check at `1A2D74`; no timing or record storage is inspected on this path.
3. `1A2BE4..1A2C40`: multiply player `CB0` by binary32 100.0, convert with
   `CVT.W.S`, convert the signed integer back with `CVT.S.W`, then divide by
   binary32 100.0. This truncates centiseconds; it is not rounding the original
   time to the nearest hundredth. Preserve the resulting `f21` across helpers.
   Capture configuration byte `1125F & 15` as the selection index.
4. `1A2C44..1A2C68`: start `f20` at that quantized lap time, and sequentially add
   the first `max((configuration byte1125C & 31)-1,0)` floats from player `CC0`.
   The signed loop bound derives from configured lap count, not completed
   `CC8`. Do not sum only completed laps, reverse the order, reassociate the
   additions, or quantize the accumulated total again.
5. `1A2C70`: call `1861A0` on the configuration with captured selection in `a1`
   and quantized lap in `f12`. A real zero return prevents the lap-record flag.
   With a nonzero return, load live player `CB4`, and set that flag exactly when
   `CB4 <= 0` or quantized lap `< CB4`. Both comparisons are source ordered.
6. `1A2CBC..1A2CFC`: reload current configuration and its lap-count byte. Only
   when its low five bits equal 3 and live player `CC8` also equals 3, call
   `186280` with the earlier captured selection and accumulated total. Its
   actual nonzero return sets the race-record flag.
7. `1A2D04..1A2D24`: if the lap flag is set, call `22B170(player, f12=lap)`;
   then, if the race flag is set, call `22B370(player, f12=total)`. Each flag
   survives intervening calls; it is not reevaluated afterward.
8. `1A2D28..1A2D70`: reload current configuration. If live mode `11258 != 3`,
   and wrapping unsigned `player.CC8 + 1 == (byte1125C & 31)`, call
   `22B760(player)`. This is a source last-lap notification condition, not a
   newly invented race-finish condition.
9. `1A2D74..1A2DA8`: reload current configuration again. If mode `11258 == 3`,
   call `217940(player)`, then store player `7C=0` in the delay slot before
   calling `217420(player)`. Otherwise return without these operations.

Only player `7C` is directly stored by the recovered body. Timing fields,
record storage, `CC8`, `CD8`, and `FF8` are not rewritten by this component.
External helper implementations may have their own additional mutations.

## External boundaries and mode 2

`OwnerSelection292360` is deliberately generic. Related source analysis shows
an ordered lookup of owner `+200` by `294350` with key 1; a first match feeds
entry `+4` to `292810(owner, object, 0, 0)`. That helper stores the object in
owner `+1F0` and tail-dispatches object virtual `+1C` with flags 0. A real
no-match returns without that mutation. The concrete class chain recovered separately is player `D40` constructor
`19EBA0`/vtable `3CECA0`, `+488` constructor `2921C0`/vtable `3D3370`, and
`p_event.ini` EventN objects registered through `232DB0`/`232ED0` with vtable
`3D1BB0`; that concrete object virtual `+1C` is `233410`, a sound-response
method. Sound terminology therefore depends on this concrete binding, not
on the generic `292360` call alone. The integrating owner must provide the
real list contents and method or prove its genuine no-match path. The callback
does not implement that lookup or equate absent backing with a no-match; an
unavailable sound provider is not permission to acknowledge the call.

For an explicitly configured, steady mode 2, the only external boundary is
that initial `292360`. After its real completion, the live mode word is read
at `1A2BD8` and again at `1A2D8C`, with no intervening helper. No numeric player
state is consumed, so `lapTimesCC0=nullptr` and count 0 are valid for this path.
If the initial helper changes mode to another value, the post-call value
controls the remaining work. A similarly numbered application byte is not
this configuration field.

Ordinary and mode-3 paths preserve every recovered call boundary. Their
record lookups require actual original-return values; zero is never assumed.
Mode 3 is structurally supported, with `217940` and `217420` explicitly
external. A successful component result cannot establish an unbound helper's
semantics or constitute evidence of a playable race by itself.

## Integration contract

1. Keep one `LapCompletionCursor50GG` alive for the suspended player callback.
   Call `beginLapCompletion50GG`; it exposes the mandatory initial request.
2. A result of `NeedEffect` or `NeedQuery` contains an exact source call site,
   callee and return PC, target-owner category, relevant integer/floating
   arguments, and a monotonically increasing sequence number.
3. Invoke that actual native helper on the actual owner. Apply its mutations
   to the canonical player/configuration state. For effects, pass an explicit
   matching return with `query=false, originalV0=0` because the source ignores
   their return register. For the two queries, pass `query=true` and the actual
   returned word. `acceptLapCompletionReturn50GG` reports `ReturnAccepted`,
   not callback completion. Wrong type, kind, sequence, or stale returns fail
   without consuming the pending request.
4. Refresh the projections, then call `advanceLapCompletion50GG`. It preserves
   register-local values across helper returns and consumes fresh fields only
   at their source continuation. Calling it while a request is still pending
   returns that same request unchanged; it never executes a helper twice.
5. Before the `217420` boundary, copy the component's `word7C=0` to the canonical
   player so it is visible to that helper. After every helper, synchronize all
   native fields it actually changed before resuming.
6. Only an `advance` result of `Complete` permits the enclosing 50GF callback
   return. Then use `resumePlayerLapAfterCallback50GF` to execute the source
   `1A0F2C` reset of `FF8`; subsequent `C8C/C88` stores remain with progression.

The field projections' C++ defaults are convenience initialization, not
recovered game-constructor values. They must describe the real live owners.
Do not reset cursor fields manually, treat an unavailable effect as complete,
or supply a synthetic record-query answer merely to keep the race moving.

All storage is caller-owned and serial. The lap array must be correctly typed,
aligned, bounded, and disjoint from mutable state, configuration, and cursor.
Only the required prefix (at most 30 entries) is inspected. A zero-length
prefix needs no backing. No allocation, callbacks, guest pointers, global
mutable state, or guest interpreter is introduced.

The numeric domain is finite normal-or-zero binary32, including signed zero,
under `FE_TONEAREST` without fast math or contraction. Arithmetic is forced
through separately materialized binary32 operations. `CVT.W.S` uses signed
truncation and saturation; NaNs, infinities, subnormal arithmetic and host
rounding modes outside this admitted domain fail explicitly. Unsupported
storage/numeric state can be corrected and resumed without replaying a
previously acknowledged helper. No timing/record store is committed by those
failed arithmetic preparations. External helper effects already performed
remain committed. Sequence exhaustion also stops explicitly.

## Complete instruction-word ledger

These words are copied directly from the hash-verified range, little-endian
32-bit decoding. They are source evidence, not a generated expected-result
fixture.

```text
001A2B80 27BDFFA0
001A2B84 7FBF0050
001A2B88 7FB30040
001A2B8C 7FB20030
001A2B90 7FB10020
001A2B94 7FB00010
001A2B98 E7B50004
001A2B9C E7B40000
001A2BA0 8C820D40
001A2BA4 70809E28
001A2BA8 24050001
001A2BAC 70003628
001A2BB0 8C440488
001A2BB4 0C0A48D8
001A2BB8 70003E28
001A2BBC 3C010038
001A2BC0 8C242B80
001A2BC4 3C060001
001A2BC8 34C51258
001A2BCC 24030002
001A2BD0 8C8402DC
001A2BD4 00852821
001A2BD8 8CA50000
001A2BDC 10A30065
001A2BE0 00000000
001A2BE4 C6600CB0
001A2BE8 3C0342C8
001A2BEC 44830800
001A2BF0 34C2125C
001A2BF4 00821021
001A2BF8 46010002
001A2BFC 3C010001
001A2C00 00810821
001A2C04 90420000
001A2C08 9025125F
001A2C0C 46000024
001A2C10 70008E28
001A2C14 70009628
001A2C18 70001E28
001A2C1C 3042001F
001A2C20 30B0000F
001A2C24 46800020
001A2C28 2446FFFF
001A2C2C 70002E28
001A2C30 46010543
001A2C34 00000000
001A2C38 00000000
001A2C3C 10000008
001A2C40 4600AD06
001A2C44 8E620CC0
001A2C48 24630001
001A2C4C 00451021
001A2C50 C4400000
001A2C54 24A50004
001A2C58 4600A500
001A2C5C 00000000
001A2C60 0066102A
001A2C64 1440FFF7
001A2C68 00000000
001A2C6C 72002E28
001A2C70 0C061868
001A2C74 4600AB06
001A2C78 10400010
001A2C7C 00000000
001A2C80 C6610CB4
001A2C84 44800000
001A2C88 00000000
001A2C8C 46000836
001A2C90 00000000
001A2C94 45010006
001A2C98 24030001
001A2C9C 4601A834
001A2CA0 00000000
001A2CA4 45010002
001A2CA8 24030001
001A2CAC 70001E28
001A2CB0 10600002
001A2CB4 00000000
001A2CB8 24110001
001A2CBC 3C010038
001A2CC0 8C242B80
001A2CC4 24030003
001A2CC8 8C8402DC
001A2CCC 3C010001
001A2CD0 00810821
001A2CD4 9025125C
001A2CD8 30A5001F
001A2CDC 14A30009
001A2CE0 00000000
001A2CE4 8E630CC8
001A2CE8 14650006
001A2CEC 72002E28
001A2CF0 0C0618A0
001A2CF4 4600A306
001A2CF8 10400002
001A2CFC 00000000
001A2D00 24120001
001A2D04 12200003
001A2D08 4600AB06
001A2D0C 0C08AC5C
001A2D10 72602628
001A2D14 12400004
001A2D18 00000000
001A2D1C 4600A306
001A2D20 0C08ACDC
001A2D24 72602628
001A2D28 3C010038
001A2D2C 8C242B80
001A2D30 24030003
001A2D34 8C8502DC
001A2D38 3C010001
001A2D3C 00A10821
001A2D40 8C241258
001A2D44 1083000B
001A2D48 00000000
001A2D4C 3C010001
001A2D50 00A10821
001A2D54 8E640CC8
001A2D58 9023125C
001A2D5C 24840001
001A2D60 3063001F
001A2D64 14830003
001A2D68 00000000
001A2D6C 0C08ADD8
001A2D70 72602628
001A2D74 3C010038
001A2D78 8C242B80
001A2D7C 24030003
001A2D80 8C8402DC
001A2D84 3C010001
001A2D88 00810821
001A2D8C 8C241258
001A2D90 14830006
001A2D94 72602628
001A2D98 0C085E50
001A2D9C 00000000
001A2DA0 72602628
001A2DA4 0C085D08
001A2DA8 AE60007C
001A2DAC 7BBF0050
001A2DB0 7BB30040
001A2DB4 7BB20030
001A2DB8 7BB10020
001A2DBC 7BB00010
001A2DC0 C7B50004
001A2DC4 C7B40000
001A2DC8 03E00008
001A2DCC 27BD0060
```
