# Native lap sound owner with an explicitly disabled provider (50GH)

`tests/native/lap_sound_owner50gh_fixture.h` supplies the concrete native owner
needed by the initial `292360(1,0,0)` boundary of lap completion `1A2B80`.
It uses the actual six-entry Pod01 event authoring, six native sound objects,
and six registered resource rows. The lookup finds Event1. It never treats
missing backing, an empty table, or a deliberately absent key as completion.

The native client explicitly selects `DisabledAudioProvider`. That provider's
actual result at the `268930` boundary is `Unavailable(-1)`. This is a host
configuration choice to permit silent native progression. It is not a claim
that the original backend failed, that a physical device is unavailable, or
that any audio was played. No original sound backend or hardware is emulated.

## Authenticated authoring

The cold adapter reuses the retained-resource reader and SHA-256 implementation
from `pod_materials50ed_fixture::detail` and the existing native
`decodeResource48C` / `extractResMember48C` decoder. It reads:

- `work/native/project/disc/PODS/POD01/POD01.RES`: 349,110 bytes;
  SHA-256 `08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3`
- Member `audio\p_event.ini`: 462 bytes;
  SHA-256 `49856d31023a3c57d4792df3675ef83928b82d25332e1b0d9b7f9fd80f0e5dfd`

The original archive metadata contains 96 entries and 115 blocks, totaling
2,826,240 decoded bytes. The event member begins at logical offset 983,040.
The adapter bounds decoding to 512 entries, 256-byte names, and 16 MiB of
decoded storage; the member has an exact 462-byte bound. It authenticates both
archive and member before parsing or changing native owners.

`[ResponseEmitter] NumEvents=6` and every actual `[EventN]` block are parsed in
source order. These are the resulting entries, not a synthesized fallback:

| Key | FileName | IsStereo | IsLoop | IsStream |
| --- | --- | --- | --- | --- |
| 1 | `SFX\Race\lapcount.vag` | 0 | 0 | absent, default 0 |
| 2 | `SFX\Race\boostmax.vag` | 0 | 0 | absent, default 0 |
| 3 | `SFX\Race\overheat.vag` | 0 | 1 | absent, default 0 |
| 4 | `SFX\Race\damage.vag` | 0 | 1 | absent, default 0 |
| 5 | `SFX\Race\wrongway.vag` | 0 | 1 | absent, default 0 |
| 6 | `SFX\Race\wspray.vag` | 0 | 1 | absent, default 0 |

Each registry name comes from the authenticated parsed filename. The native
provider's bounded namespace uses these exact names; an original device-bank
namespace, mutable path prefix, sample byte loader, and full audio-global
construction are outside this adapter. The disabled provider needs no sample
bytes. An unloaded sample does not mean a missing registry row.

## Native construction and resolution

The source chain is the actual player `D40` owner, its `488` response emitter,
and EventN sounds created through `232DB0` / `232ED0`, with concrete sound
vtable `3D1BB0` and response method `233410` at virtual slot `+1C`.

The adapter owns typed native records, not guest pointer overlays. It projects
the proven constructor fields used or preserved by this bounded path:

- `28=-1`, `2C=-1`, `A0=-1`, `1B8=-1`
- `20=0`, `24=0`, `7C=0`, `80=0`, `1A8/1AC/1B0/1B4=0`
- `38=100`, `3C=0`, `40=4096`, `44=100`, `48=0`, `4C=4096`, `50=4096`
- `54/58=1.0` bits, `5C=0.25` bits

Unlisted sound fields and original parent-pointer values are not guessed.
Fresh native emitter selection begins null. `233110`'s actual event flags are
`1` for keys 1–2 and `0x81` for keys 3–6; loop/stereo/stream field projection
follows the authored flags. No full original player/audio-system constructor
parity is claimed.

The ordering of resource registration and resolution is essential:

1. Insert each real authored name into the fixed native registry. The source
   `298900` insertion stores the name at row `+0`, its supplied value at `+4`,
   initializes reference count `+8` to zero, and returns the new index. The
   disabled host registry initially supplies the unloaded `-1` sentinel; this
   initialization is not counted as a provider call.
2. `233110` resolves the name through `26DAD0 -> 298C80`, stores the valid
   index into sound `A0` at `2332A0`, increments row `+8` through
   `26DAF0 -> 298A30`, then registers the actual sound owner through
   `26DB00 -> 298F80`.
3. Registration stores the sound owner into **the same row `+4`**, at
   `299038`. An initial `-1` is therefore not sufficient proof of a later
   negative resource result. `ResourceValueKind::RegisteredOwner` and a real
   `Sound*` represent this phase; no host pointer becomes a fabricated integer.
4. The source `26D9F0` loop processes referenced rows. It obtains the row name
   and registered sound owner, then calls `2332F0`. That method calls provider
   `268930` at `2333A0`, passing the sound's request. This is the only boundary
   replaced by the deliberately disabled native provider, once per event.
5. Store the actual provider result in sound `28` at `2333A8`. A negative
   result skips the metadata helpers and their `20/50/40/4C` writes.
   `2332F0` still returns 1 at `2333F8`; that return does not mean playback.
6. `26DA68` loads sound `28`; `26DA70 -> 298F80` publishes that value into
   row `+4`. The typed alternative becomes `SampleResult` containing `-1`.
7. `26DA84 -> 299060` clears row `+8` at `299110`. The sound retains its valid
   `A0` index. All six rows remain present with a real negative result.

The cold receipt records six insertions, six initialized sounds, six provider
requests, six sound-28 stores, six row-4 publications, and six reference clears.
Each per-entry registration-before-resolution check is true. Playback calls
remain zero. Owner and provider must be fresh, serially owned, and stable in
memory; both disable copying and moving to preserve internal references.

## Actual lap dispatch and mutations

Only the proven key-1, nonstream, flags-zero lap call is executable here.
The other five entries are real, initialized backing but do not expand this
adapter into the unrelated response methods.

1. Validate the bound provider, complete six-entry event table, actual sound
   object references, and all valid resource rows. A missing or invalid row
   is an error; it is never interpreted as a sound failure return.
2. `292360` scans entries in order through `294350`. The first entry really
   matches key 1 and yields its actual native `Sound*`.
3. `292810` first stores the selected object into emitter `1F0`, then invokes
   concrete `233410` with flags 0.
4. `233410` invokes `233E90` before checking the result. With negative sound
   `28` and nonstream `1A8=0`, `233E90` calls `26DB10 -> 298E60`. That helper
   reads the **valid indexed row `+4`** at `298E98`; `233EC4` stores the actual
   negative result in sound `28`. Its negative branch skips metadata calls.
5. Back in `233410`, negative `28` and zero `1A8` take the return-zero path
   at `23344C/233450`. Playback and all later writes are bypassed.

The only source stores on the successful hot path are emitter `1F0` and sound
`28`. Repeated calls repeat both stores even when their values are unchanged.
Sound `2C`, `24`, `7C`, every other projected sound field, the other five sound
objects, authoring, registry rows, and cold receipt are preserved. A dispatch
receipt carries the selected object, previous selection, resource index,
source PCs, actual provider result, source return 0, and before/after sound
fields. `ownerSelectionStored` and `sound28Stored` denote stores, not value
changes. `orderedEntriesVisited` is 1 for the actual lap event.

Positive sample handles, stream sounds, different keys, and nonzero caller
arguments are unsupported. Guards run before the first source mutation and
fail explicitly. There is no success substitute, missing-event no-op, hot
allocation, source file I/O, metadata callback, sample decode, or device call.

## Integration API

```cpp
lap_sound50GH::DisabledAudioProvider disabledAudio; // Explicit host policy.
lap_sound50GH::Owner lapSoundOwner;
std::string error;
if (!lap_sound50GH::prepareDisabledFromPod01(
        lapSoundOwner, disabledAudio, error, pod01ArchivePath)) {
    // Keep startup blocked; report error.
}

// When 50GG requests OwnerSelection292360 on the bound player D40->488:
const auto sound = lap_sound50GH::dispatch(lapSoundOwner, 1, 0, 0);
if (!sound.completed()) {
    // Do not acknowledge the pending 50GG helper.
}
// Only here may the matching 50GG effect return be accepted with
// query=false and originalV0=0. Continue the actual callback afterward.
```

`prepareDisabledFromPod01` defaults to the verified Pod01 archive path above.
Bind this owner to the actual native player's `D40->488` role; merely creating
it elsewhere does not complete the callback. The 50GG adapter must validate
the request kind, target, and arguments, perform this dispatch once, and then
acknowledge that same pending sequence. No progression cursor or player time
is changed here. Only completed 50GG callback processing permits the enclosing
50GF continuation and its `FF8` reset.

This is a development-client cold fixture, not a public general audio API.
The including target already needs the existing native resource decoder and
its zlib dependency. No build-system changes are included.

## Immutable source receipts

Original `work/native/project/disc/SLUS_202.68` SHA-256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The relevant ELF mapping is `file_offset = VA - 0x100000 + 0x80`.
The following ranges are half-open and were read as immutable source words:

| Source range | SHA-256 |
| --- | --- |
| `292360..292410` | `db85e2f7133f92912ced5781f8e0457bdc30082a8f9b4a72202049ce9ba8cf84` |
| `292650..292810` | `a3f8b5aa61635e4ab710519d28f92eeafb5a24c256257275cb5f264744d4d50f` |
| `292810..292850` | `0ec66279849a7297b4785fd6b48ced1465353af4a41228512b85fbee8fe43680` |
| `232ED0..233070` | `86d4935d795bcf3aa6f8e19b9edff254a4b971439b38907a981b480d722c7e4c` |
| `233110..2332F0` | `782637c3e4feb5a58291a7ae147189179c493e70996723f17387dc7bdce58542` |
| `2332F0..233410` | `7d8895a980a4272dc36fa91fda624d6f2137386b2b2af17ac5e610e9f6f5693a` |
| `233410..2335A0` | `d7b343890ba024e55f3cffd0edd0bccbae09aad6606905cd220ed5a607148935` |
| `233E90..233F20` | `20ae9966e0ca2ddd41595e310c3588883e887109b5297c099f271fe231e66daf` |
| `26D9E0..26DAD0` | `d4c9f4cf01d451fb0b128dd18684f10160fbdd720beda4accf5866956d4192ec` |
| `26DAD0..26DB20` | `9d1f0bdd7eef87af9e19bb1c99598a6d58c57efc6161c8e199b67aa858e79d6f` |
| `298900..298A30` | `df79d27153c32799643a78cbf551e82f15a59386ff0d9002daaccea30140a36a` |
| `298E60..298EB0` | `7bd64df67603c710777bbd63fd2427e7e3b9b8a3c61e792bfe4cda18d70071f8` |
| `298F80..299060` | `4afd74e0334e0354e5ed809885369e4242928d720556fe941a47519babd04570` |
| `299060..29912C` | `18bf607f160bc4eba3e7cdf79cd6b00d84f750a026bc26b2919d89b2ef93c2d4` |

Decisive source words include `292810 AC8501F0` (selection store),
`2332A0 AE4200A0` (resource index), `299038 AC700004` (row value store),
`2333A0 0C09A24C` (provider call), `2333A8 AE020028` (sample store),
`26DA68 8E460028` / `26DA70 0C0A63E0` (publication),
`299110 AC600008` (reference clear), `298E98 8C420004` (row read),
`233EC4 AE020028` (refresh store), and `233450 70001628` (zero result).

The implementation author ran no build or test. Verification in this document
is source-word, source-hash, archive-metadata, member-identity, and static
implementation review. The integrating target's checks are reported separately.
This document does not claim runtime integration validation, audible output,
guest execution, full race completion, or a complete original audio backend.
