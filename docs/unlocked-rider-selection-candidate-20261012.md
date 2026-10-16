# Original fresh-unlocked Rider selection candidate

Incremental to the frozen Rider19/20 lifecycle patch. The prior patch is unchanged.

## Implemented owners

- Source profile reset admits Rider IDs2,4,10,11,12,15,19,20. Navigation preserves
  the original control order and stops at an unprepared visible neighbor. It
  never invents a shortened wrap-around roster.
- Dynamic cold descriptors and backend pair readiness derive pod/character
  widget IDs from authenticated rider.dat first/fourth control membership.
- Graph/material retirement, reacquisition, orientation restoration and deferred
  original movement publication are reused for each actual prepared pair.
- Additional pairs lacking a genuine authenticated kind2 image cache are omitted
  before immutable material-bank construction. Malformed references remain errors.
- Profile, Confirm and Course entry retain the selected original rider. Original
  Course-to-race admission now supports TA/GA with each of the eight unlocked
  riders when its own authenticated race resources are prepared.
- Confirm voices use source constructor event ranges5–6,9–10,21–22,23–24,25–26,
  31–32,39–43,44–45, explicit event lookup and one shared original RNG draw.
  The12 added files are SPEECH/UI/{BEN,DUD,ALD,ODY,SHR,TEE}SEL{01,02}.VAG.
- The IPUM owner aggregates independently authenticated original PST sources;
  texture/provenance lookup requires the exact member identity and borrowed view.
  Existing19 catalog and API remain. Additional compiled catalog defaults absent.

## Production inputs

Original archive:
`/workspace/shared/input-recovery-20261012/extracted/RacerRevenge-original-game-UI-menu-and-videos/UI/UIRIDER/UIRIDER.RES`

Whole archive5552683 bytes, SHA256
`112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856`.

`src/assets/original_ui_archive_catalog50kg.inc` retains the exact original
member identities, offsets and SHA-256 values. Existing
`tools/gameplay/recover48b.py::unpack_resource(blob)` is the generic bounded
RES reader; use only that function, never the module's unrelated recovery main.
Extraction must authenticate the whole archive and every requested member.

Cold image preparation, executed only by the authorized root:

```sh
python3 tools/assets/recover_unlocked_rider_ipum50kg.py \
  --repo "$REPO" --source-dir "$ORIGINAL_FLAT_PST_DIR" \
  --rider-archive "$UIRIDER_RES" --elf "$SLUS_202_68" \
  --output-root "$FRESH_OUTPUT" --ffmpeg "$FFMPEG" \
  --execute-cold-conversion
```

For only Rider12 add `--source 12c_1 --source 12c_4`. Omit source options to require
all8 pinned inputs. Actual indexed sources are recorded and skipped. New output
catalog is `src/assets/original_ui_ipum_texture_cache50kg_additional_generated.inc`;
RRTEX files/manifest are in `evidence/ui-rider-ipum50kg`. Merge real retained files
into the existing runtime cache directory without overwriting Rider19 artifacts.

The unchanged50FC native FFmpeg helper supplies decoding, integer color conversion,
RGB16 dither and TEXA/AEM. There is no emulation or original-hardware parity claim.
No invented source/output hash enables a cache.

## Source material closure

Root supplied authenticated original47-member extraction at
`/workspace/shared/unlocked-rider-original-inputs-20261012`. Actual PSM reading
shows02c,11c and12c_2/3/5 activate one diffuse pass; dormant source sections stay
excluded.12c_4 is indexed kind1 with four mips, not IPUM. Its three active passes
now preserve first/third source-alpha blending and middle FIX64 additive
specular with depth writes. The native source-RGBA shader applies
`floor(sourceRGB * 64 / 128) + destinationRGB` and writes source alpha; no
fixed-function blend approximation or fabricated image is used.

The seven actual additional kind2 PSTs each contain five mips. Their retained
RRTEX bytes and generated catalog still require authorized production conversion.
Without these outputs the corresponding whole rider pairs remain unavailable;
no source neighbor is silently skipped.

No builds, tests, syntax/import checks, decoder execution or gameplay were run by
this worker. Human acceptance and authorized central production preparation remain
separate from these source edits.
