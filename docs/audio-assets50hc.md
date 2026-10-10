# Original audio inventory, native cold bank and bounded VAG decoder (50HC)

## Restored original assets and current result

The authorized original AUDIO transfer is now verified and extracted at `/workspace/scratch/74cd1d3d7a45/received-audio50hc/extracted/AUDIO`. Its ZIP is 79,019,408 bytes, SHA-256 `c246ad58020726d92a65a56e09e4cd9b68ba60cdcc9aa697d2cb48e51275aafb`, with 2155 members including one manifest. All 2154 original files, totaling 96,437,028 bytes, passed ZIP CRC and manifest SHA-256 checks. Extraction used a fresh directory, exclusive file creation and path/case/symlink guards. The exact receipt is `solo-audio50hc/assets/transfer-receipt.json`.

The current authenticated 60-clip bank is **60 ready, 0 missing, 0 unsupported, 0 invalid**, with 4,556,328 bytes of nonzero decoded original PCM. Rates are 22050 Hz (50 clips), 16500 Hz (4), 18500 Hz (3) and 19000 Hz (3). Six clips have valid embedded loops: overheat, damage, wrongway, wspray, engine AS_NGN and beam AS_BEAM. All 40 selected Pod01 voices and three Dax intro voices load under explicit native preload policy. This is not a claim that every original game's sound trigger is integrated, nor a physical-audibility check.

## Initial retained-input inventory (historical)

Before the authorized transfer, the retained inputs contained audio names and authoring, but zero original sound-sample bytes. The initial inventory and first decode failure are preserved. No tone, replacement sound, cloned voice, saved guest audio, guest execution, excluded49C/48U input, or external proprietary download was used. Synthetic decoder test vectors are isolated under `solo-audio50hc/assets/synthetic-test-only/`; they are never selected by the game or played.

The inventory checked both retained original resource archives, all 158 allowed retained ZIP indexes, and loose audio-format files in the current scratch/shared workspaces. The ZIP indexes contain 36 `.RES` entries, all retained copies of the two original resource names. There were no `.vag`, `.wav`, `.pcm`, `.vb`, `.vh`, `.bnk`, `.str`, `.sfx`, `.iso`, `.cso`, `.chd` or `.irx` entries. These are metadata inventory checks; excluded paths and guest snapshots were not read.

| Input | Bytes | SHA-256 |
| --- | ---: | --- |
| `PODS/POD01/POD01.RES` | 349110 | `08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3` |
| `TRACKS/TA.RES` | 3334562 | `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8` |
| `SLUS_202.68` | 2972720 | `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30` |

The complete static executable file-name table has 2154 `AUDIO` paths, including 2010 `.VAG` names: 267 SFX, 1684 speech and 59 music. A name in this table proves an original reference, not available sample bytes or a live runtime trigger. Exact original spelling, file offsets and executable identity are in `solo-audio50hc/assets/slus-audio-file-table.json`.

The restored original disc `AUDIO/` subtree preserves its paths. This component did not access the user's computer; it verified and extracted the supplied cloud-local transfer. The destination audio root remains explicit.

## Authenticated current-race authoring

`readRetainedAudioRequests50HC` authenticates both complete archives and the executable, uses the existing native RES decoder, and collects 60 distinct original names. All 60 samples were initially missing and are now ready from the verified transferred AUDIO root. The exact authored members and member hashes are preserved in `retained-audio-catalog.json`.

- Pod01 response events: `lapcount`, `boostmax`, `overheat`, `damage`, `wrongway`, `wspray`; the last four have authored `IsLoop=1`. This is the same six-entry authoring used by the earlier `lap_sound_owner50gh_fixture.h`.
- Debris: events 1–4 are `debris2`, `debris1`, `debris3`, `debris4`.
- Wrecks: sounds 1–4 are `podexpl5` through `podexpl8`.
- Pod01 engine and beam: actual `[Pod Audio]` fields are `sfx\race\as_ngn.vag` and `sfx\race\as_beam.vag`. The source also gives idle pitch modifier 0.32, pitch range modifier 4, idle binder modifier 1, binder range modifier 1, engine volume 100 and binder volume 90. This bank does not convert those values to runtime pitch/gain policy.
- `sfx\race\boost.vag` is the exact null-terminated source literal at executable file offset `0x2A8210`. Filename `boostmax` is not treated as evidence for a boost-start trigger. Runtime event mapping is separately owned.
- Pod01 curses: 19 clips (4 low-damage, 4 wall-hit, 6 passed, 5 attacked).
- Pod01 taunts: 12 clips (6 passing, 6 attacking).
- Pod01 special: 9 clips (4 death, 3 win, 2 lose).
- TA intro: 3 Dax clips `speech\Dax\Tracks\Tatooine\daxtaa01.vag` through `daxtaa03.vag`.

There are 43 authored stream requests (40 Pod01 voices and 3 Dax voices), zero authored stereo requests, and 4 explicitly authored loop requests. Missing flags default to false in the catalog; this does not define engine-loop behavior or live event policy.

`tracks\ta.aub` was also decoded with the existing reversible native BINARY1 parameter decoder. Its 69 blocks contain 8 material-collision rows, 8 material-drive-on rows, 52 spatial/environment rows and 1 general block. These reference 27 distinct extensionless SFX resource names. Every field and source offset is retained in the catalog. These additional spatial/material rows are inventory only; 50HC does not invent live spatial triggers or silently append filename extensions to select them.

## Original provider boundary

Static source at `268930` is an EE-side file/registration/RPC provider. The mono path normalizes an extension to `.vag` at `268EC0..268EF4`. The stereo branch opens separate `%sL.vag` and `%sR.vag` paths (source strings `3B9388` and `3B93D0`; construction at `268D00` and `268DA0`). It passes file extents/sector metadata through RPC command 5 at `26917C`, then requests metadata through RPC commands 15 and 16 at `2692A4` and `26931C`.

These retained EE words do not parse a VAG header and cannot prove the actual game's on-disc codec variant. The sample-format interpretation occurs beyond this boundary, in missing IOP/audio inputs. Static word receipts are preserved under `solo-audio50hc/assets/source-*.txt`; all source reads use the immutable original executable. No RPC, executable, guest, or IOP code was run.

## Native API and admission

Files: `src/assets/audio_assets50hc.h` and `.cpp`.

- `readRetainedAudioRequests50HC(discRoot, requests, error)` produces the 60-name authenticated cold catalog.
- `loadAudioBank50HC(audioRoot, requests, bank, error)` reads only explicitly selected names from that `AUDIO` root. Matching folds ASCII case and treats slash/backslash equivalently, so original uppercase Linux directory trees work. Traversal, unsafe characters, duplicate requests and case-colliding filenames fail. Symlinks below the root are not followed.
- A clip has explicit `missing`, `ready`, `unsupported` or `invalid` state, an error/detail, exact selected file path, byte count and SHA-256. Missing samples remain records and never become synthesized audio.
- `bank.find(name)` is an allocation-free lookup returning a stable clip pointer while that prepared bank remains unchanged. The mixer may borrow `clip.pcm.samples.data()` for the bank's lifetime.
- `AudioPcm50HC` owns signed 16-bit interleaved PCM, source sample rate, channels, frame count, embedded loop presence and `[loopStart, loopEnd)` sample-frame bounds. No rate is guessed for missing files.
- Authored `stream=true` mono clips use an explicit native **cold preload** delivery policy. The original stream flag remains in the request; this does not emulate original IOP streaming. Every one of the 40 Pod01 voice names can take this path if its actual bytes satisfy the supported format. That compatibility has now been established for all 40 selected Pod01 voices and the three selected Dax intro lines.
- Authored stereo remains unsupported until original paired samples and framing are inspected. The bank does not route stereo authoring through a mono decoder.

Cold admission limits are 4096 requests, 16384 directory entries, 32 MiB per encoded file, 32 million PCM frames per clip and 256 MiB retained PCM per bank. Individual missing or rejected clips do not erase valid neighboring records. Bank-level failures leave the previous bank unchanged. No file read, decode or allocation belongs in the hot mixer path.

## Decoder and verified actual-file variant

`decodeVagMono50HC` supports conventional big-endian `VAGp` mono with a 0x30-byte header, standard versions 0/2/3/0x20 and 16-byte PS-ADPCM frames (28 samples). It validates the payload, rate, reserved fields, predictor, shift, flags, loop markers and bounded zero padding. The actual Racer Revenge variant retains big-endian length at 0x0C and rate at 0x10, with all other 48-byte header fields zero. Admission requires these exact zero fields plus exact payload/file extent; no headerless stream is guessed. All 60 current samples have this verified shape. Stereo/interleaved/other custom variants are rejected explicitly. It returns the rate from the file header and preserves embedded loop bounds. The first end flag terminates the finite decoded clip. A loop end without a start and multiple loop starts are conservatively rejected.

The decoder implements integer software sample reconstruction with predictor division truncating toward zero and output saturation to int16. It is **not claimed to reproduce PS2 SPU2 rounding, interpolation, ADSR, filters, reverb or hardware output bit-for-bit**. Format and software-decoder references: [FFmpeg VAG demuxer](https://raw.githubusercontent.com/FFmpeg/FFmpeg/master/libavformat/vag.c), [FFmpeg PSX ADPCM decoder](https://raw.githubusercontent.com/FFmpeg/FFmpeg/master/libavcodec/adpcm.c), and [vgmstream VAG variants](https://raw.githubusercontent.com/vgmstream/vgmstream/master/src/meta/vag.c). No external implementation source is linked into the runtime.

## Focused verification and preserved limitation

- 35 directed checks pass after the actual-header extension (32 before transfer) in the normal build and with AddressSanitizer/UndefinedBehaviorSanitizer: real catalog authentication/counts, all-missing state, uppercase paths, lookup, bounds, malformed input, transactionality, flags/loops, traversal/case collisions, symlink rejection and authored stereo rejection.
- A 700-frame **synthetic decoder-only** vector exercises all five predictors and multiple shift values. Output matches the installed FFmpeg decoder byte-for-byte; both PCM hashes are `73d2fef22fe4a1fd229b80a5825b8cc8dd3758e161f975fe14bb34f3a8b4dad7`. This is codec testing, not original-game playback evidence.
- The first sanitizer run hit LeakSanitizer's container-ptrace restriction. Its stdout/stderr remain as `sanitized-first-attempt-lsan.*`. ASan/UBSan were rerun successfully with `detect_leaks=0`; no leak-check pass is claimed.
- All 60 genuine current-bank files decode and contain nonzero PCM. Exact rates, loop bounds, source hashes, extents and sample ranges are in `actual-zero-header-current60.json`. The first standard-magic-only result (60 unsupported) remains `actual-first-attempt-standard-header.json`.
- Genuine AS_NGN engine PCM matches FFmpeg for all 52,108 frames. AN1RDL01 voice (13,916 frames) and LAPCOUNT (18,256 frames) match the FFmpeg prefix byte-for-byte through their original end flag. FFmpeg additionally decodes one 28-frame `07 00 77...` padding block after that flag; this bank stops at the source end marker. Initial whole-file comparison failures, exact outputs and this bounded comparison are preserved in `actual-crosscheck/`.
- The full restored library contains 2010 VAG files. Header inventory found 1996 zeroed-header and 14 conventional VAGp files; 56 files have data beyond the declared mono payload and remain outside this conservative mono contract. These are largely unselected assets; this component has not claimed to decode or integrate the entire 2010-file library.
- No output-device or perceptual check is claimed by this component.

Receipts, exact authoring members, manifests, source words, test source and logs are under `solo-audio50hc/assets/`. No broad regression suite was run. The files do not change the live client, backend, build system, or historical 50GH disabled-provider behavior.
