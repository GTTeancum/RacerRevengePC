# Gasgano / Rider07: original-byte evidence, round two

Source-only work against `79fc223eda2da3d98929c9f3b1253745d17e7c2a` plus the
preserved round-one handoff. Round-two before-state is `ROUND2-BASELINE`.
The original-byte gap for artwork and constructor voice range is now closed.
Native destination readiness is not: **Course and voice admission are unchanged**.

## Original files actually read

All paths below are relative to the explicitly authorized original root
`D:\Programming\GitHub\RacerRevengePC\work\native\project\disc`.
There were no original-directory scans or writes. The bounded evidence reader
parses RES3 filename/offset tables and inflates only selected 24576-byte blocks
in memory. No archive members or converted images were written as asset files.
Evidence JSON retains hashes, authored text and numeric layouts only.

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| SLUS_202.68 | 2972720 | c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30 |
| UI/UIRIDER/UIRIDER.RES | 5552683 | 112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856 |
| UI/UIPODTEX.RES | 2653798 | ffb0f32563e3e5b6e3fefe915bbb37a00610e1a9a44163bce9737c2ac9d6e641 |
| UI/PODPHYS.RES | 46563 | 8361cb484ea841e1772cc0ddec2223fa923e397c1cc9a89f06ab06b1cd7ca2c8 |
| UI/UIBKG.RES | 1330049 | 67b7774815bd4352e72103160e323c00e062f2e2845e05e9720b38796185091f |
| PODS/POD07/POD07.RES | 354874 | 0cde5fac5d9a379e1ca426409809ba5c04eec6895013577f03639c69859fd8a8 |
| AUDIO/UIACCEPT.INI | 4411 | 245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b |
| AUDIO/SPEECH/DAX/INRACE/POD07ANN.INI | 2496 | ee0e83e2bc80e896c783d0f0f4058d381677faf8da7ce8f8e9cbfb06671d17e9 |

The archive paths/pins originate in `src/assets/original_ui_archive_catalog50kg.inc:13`
and its neighboring archive entries; the POD07 pins originate in
`tests/native/original_player_identity50ik_fixture.h:52`. Hashes above were
computed from these actual originals, not copied into an observation result.

## Exact constructor voice range: first 15, count 2

The ELF program header maps file offset `0x80` to virtual address `0x100000`,
with 2972160 file-backed bytes. Thus these instruction words are read from
`fileOffset = VA - 0x100000 + 0x80`. They were interpreted statically, never run.

| Virtual address | File offset | Word | Static meaning |
| --- | --- | --- | --- |
| 0x1e7c7c | 0xe7cfc | 0x24030002 | addiu v1, zero, 2 |
| 0x1e7ca8 | 0xe7d28 | 0x2410000f | addiu s0, zero, 15 |
| 0x1e7cfc | 0xe7d7c | 0xaca353d8 | sw v1, 0x53d8(a1) |
| 0x1e7ddc | 0xe7e5c | 0xac505444 | sw s0, 0x5444(v0) |

The surrounding retained words show the page pointer loaded from `sp+0xac`
and no intervening overwrite of the relevant value registers before stores.
`0x53d8 = 0x53bc + 7*4`; `0x5444 = 0x5428 + 7*4`.
Consumer words at `0x1ea658..0x1ea668` load selected rider from page+0x4c4,
multiply by four and load count from page+0x53bc. At `0x1ea764..0x1ea784`,
the consumer again indexes by selected rider, loads first from page+0x5428
and adds the random choice. This establishes Rider07's own range rather than
extrapolating from adjacent riders.

UIACCEPT independently authors Event15 `speech\UI\gassel01.vag` and Event16
`speech\UI\gassel02.vag`, both `IsStream=1`, beneath the Gasgano comment.
`recovery/20261014/gasgano/original-evidence/ui-name-comment.json` retains its exact byte offset/snippet.
The exact authored paths under AUDIO were read: GASSEL01.VAG is 26656 bytes,
SHA `4076d3b4943d5828a0b7977ca0bc9a6e1fdffc1e4a0925f41aed560713183c90`;
GASSEL02.VAG is 26512 bytes, SHA
`c647576fb9e438d58b921142c0592041ba2a9ccb0a9db539e0f6ceeb34eb28e9`.
These prove source presence, not native PCM conversion or audible playback.

## Authentic artwork and authoring

All offsets in the next table are **logical member offsets**, not offsets in
the compressed RES file. `original-data.json` records each member's index file
offset and the exact compressed block file offsets/lengths as well.

| UIRIDER member | Logical offset | Bytes | SHA-256 |
| --- | ---: | ---: | --- |
| ui/textures/07_flrg.pst | 417792 | 5148 | 961be88fb4f5aeda59b9c8cffefc064db020c8fc6e9b9ba939e4d45e919d67d6 |
| UI/UIRider/Widgets.Dat | 2064384 | 24173 | 6e5fab48528286a04a6782362457cc66ef0ae478470a4cb77302b871db0f2bd5 |
| ui/geometry/07.psg | 5652480 | 108636 | 81ddf3fdf1b79815551c816dd45baf207426822428b00bf17df1d25acd7920dd |
| ui/anim/07c.mcf | 5775360 | 612 | 55445c18457279fd56bd2ddb5a13af583f212ec5f211b28c28078e776828a463 |
| ui/anim/07c.psg | 5799936 | 74020 | ce618b019509b130c6509248154b28195a4263d6d82ba975686e52b7d1d8ee31 |
| ui/anim/07c.vut | 5898240 | 788 | f1c459643d70c7b8cdf5f9c2719b2d39f5ffc41d6240348eaab3115e0c6a14c2 |
| ui/textures/07c_1.pst | 6217728 | 66588 | 95c250dcc8083d62c321c13db6600bc55304807e3e52b75406389397cb45caea |
| ui/textures/07c_2.pst | 6316032 | 29512 | cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce |

Widget41 explicitly authors `Type=Soultree_Character`, `DynamicLoad=T`,
`MCF=07c.mcf` and its own transforms. MCF motion order is ent, idl1, breathe,
sel, idl2, idl3; all six actual MOT member hashes are recorded in the evidence.
Do not reorder them or copy another character's geometry/animation.

`07c_1` is kind1 indexed artwork. Its material has active
`TotalShaderPasses=1`; the commented two-pass declaration and later specular
section must not be mistaken for two active passes. `07c_2.psm` authors one
DiffuseCompressed pass referencing `07c_2`. POD preview materials `07_1` and
`07_2` have two active passes, with `specmap1` in their specular pass; `07_3`
has one. The original specmap1 PST is present in UIRIDER, logical884736,
66588 bytes, SHA `d928592fbf2b0b7bd47075992bdcef7c9be426a766994148dce98ea2360f3f84`.
UIPODTEX supplies the own `07_1/07_2/07_3` PSTs. UIBKG supplies `07_bkg.pst`
at logical589824, 132124 bytes, SHA
`b66c62f4f956e76bde4d12581f01138182ddc765bb9d521673c05c814ebf8d7c`.
The shadow and shared podstat/podstat2/notch source identities are also recorded.

The unadmitted `07c_2.pst` has kind2, five ascending mips, exact end29512:

| Dimensions | Member header offset | Payload offset | Payload bytes |
| --- | ---: | ---: | ---: |
| 16x16 | 12 | 32 | 216 |
| 32x32 | 248 | 268 | 776 |
| 64x64 | 1044 | 1064 | 2392 |
| 128x128 | 3456 | 3476 | 7101 |
| 256x256 | 10577 | 10597 | 18915 |

Each payload hash is in `original-data.json`. This member spans compressed
blocks257 and258: file1456936/23755 bytes and file1480692/4995 bytes. Those
are source payload identities; no RRTEX, RGB, YUV or converter hash is fabricated.

## Handling and native admission boundary

The actual POD07 PHY in POD07.RES (logical1056768) and the menu copy in
PODPHYS.RES (logical98304) are both 6617 bytes with SHA
`8615aca792d1efb68abcda048dd2b6cc9207c6c044300bf2306b6618edaffa48`.
It authors Name Gasgano, Prefix07, chariot/englt1/engrt1 and EngineMotion5.
Its audio fields select `sfx\race\gen2_ngn.vag` and `sfx\race\as_beam.vag`,
with own pitch modifiers .84/1.89 and .47/1.59, volumes85/100. Both exact files
are present and hashed in supplemental evidence. Own curses/taunts/special and
sounds INIs were read in POD07.RES and match the retained identity pins.
The handling values are evidence, not a replacement tuning table.

`src/assets/original_ui_ipum_texture_cache50kg.cpp:57` admits only its finite
additional source pins; neither that list nor generated conversion catalog
contains `07c_2`. Its current conversion outputs and native image publication
are unavailable. This is a concrete owner gap outside this worker's edit
ownership. Full gameplay rig/COL/PCM behavior was not established by static
source hashing, and no runtime test is permitted here. Therefore none of the
three owned domain/voice C++ files was modified.

## Delivered source and parent integration

The preparer now optionally accepts `--gasgano-ui-source-root` together with
`--slus`. In a later authorized preparation run it authenticates the exact
four UI archives and four named audio inputs without a scan, checks the SLUS
hash and four constructor words, and records a source-evidence block. It does
not execute SLUS, convert Rider images or set any readiness flag. Omitting the
two optional arguments preserves the round-one CLI and yields null UI source
evidence. The receipt now names the real pending IPUM/owner work rather than
claiming that the voice range remains unknown. The preparer itself was not run.

Parent next steps:

1. Apply this round's delta after the round-one handoff; reconcile against the
   parent's newer source without overwriting shared work or baselines.
2. Add the exact `07c_2` source and mip pins to the native cold conversion/cache
   ownership in a separately authorized phase. Obtain authentic converted
   outputs and codec provenance, then extend the finite cache catalog and
   Rider image/material publication path. No other rider may substitute.
3. Preserve Widget41, the original MCF ordering, own pod preview/portrait/
   background and shared stat source identities. Authenticate the existing
   POD07 whole rig/PHY/COL/audio owner with eight distinct original entrants.
4. Once actual destination closure is established, add the original voice row
   `{7,15,2,"Gasgano"}` with count anchor1E7CFC and first anchor1E7DDC to the
   voice table, update its header comment, and extend Course capability to7.
   Keep source unlock/profile gating separate. All driving controllers,
   including followed slot0, remain original AI; host course driver stays off.
5. Parent owns build manifests, all tests and integration. No claim of a tested
   native image, audible sample, gameplay result or PS2 rendering parity is made.

No builds, tests, syntax checks, converters, asset preparation/extraction
pipelines, executable probes or guest execution were run. Only the explicitly
authorized bounded static data reader ran. The attempted Python reader could
not start because Windows resolved python to an unavailable app alias; the
PowerShell/.NET reader supplied the evidence. All WORKER deliverables and
ROUND2-BASELINE snapshots remain untouched.
