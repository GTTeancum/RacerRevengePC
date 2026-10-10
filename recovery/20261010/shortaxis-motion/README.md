# Authentic SHORTAXIS motion candidate

Staged only. No active edits, commits, builds, tests, syntax checks, project-parser executions, sanitizers, gameplay, guest execution, or PS2/VU emulation were performed. Evidence inspection used only bounded standard-library byte reads and static disassembly. This is not a validation result.

## Deliverable

Three full replacement candidates under `files/`, exact source preimages under `preimages/`, unified `patch.diff`, and SHA-pinned `source-manifest.json`.

- `src/assets/model_geometry49o.h/.cpp`: Extend existing admitted MOT format with explicit original `rotationType` metadata (0 ByteAxis, 1 ShortAxis). Position remains Bezier1. Add strongly typed `ShortAxisKey49O` accessor; preserve original ByteAxis accessor and reject cross-format use. Native original ShortAxis bytes remain unchanged, without converting to ByteAxis.
- `src/game/model_motion49t.cpp`: Existing49T sampler dispatches typed rotation keys by original metadata, preserving its original ByteAxis arithmetic and tables. ShortAxis flows through the same checked interpolation, matrix, scene-graph and transactional workspace owners. No separate model, scene graph, palette or renderer is introduced.

Historical frozen samplers49O/49Q are intentionally not changed. Their ByteAxis accessor rejects ShortAxis; authentic preview callers must use49T (including its existing direct-time entry point).

## Original source equations and layout

- Loader152684 constructs ShortAxis145AF0 for rotationType1. Header offsets+12/+16 remain original rotation/position types. Short key record = float time at+0, unsigned16 angle at+4, signed16 axisXYZ at+6/+8/+10; stride12. Original ShortAxis sampler145E00 and interval selection145EE0 use this stride.
- Decode1010B0 uses angle to index sine base3D6BC0 and cosine base3DABC0. Quaternion layout is w,x,y,z. Each axis lane is separately converted to binary32, multiplied by binary32 bits38000100 (approximately1/32767), then multiplied by sine. w is cosine. Source does not normalize the compressed axis before this step. Native unchanged matrix writer normalizes the quaternion as before.
- Constructor100B90..100C68 builds4096 entries, index0..4095. Domain is half-angle `f32(0.5 * f32(f32(2*pi) * f32(index/4096)))`, with 2*pi bits40C90FDB. Table size is16KiB each. Angle4096..65535 is now rejected rather than following original unchecked OOB reads.
- Original constructor widens the computed float through311038, calls software-double sine302580/cosine301F50, then narrows through310440. Native candidate explicitly substitutes standard host double `std::sin`/`std::cos` for those software-double functions, in fixed static4096-entry float tables. This is a documented precision policy, not an original-byte lookup table: last-bit parity for ShortAxis trig is not claimed. No guest arithmetic emulator is implemented. ByteAxis tables are untouched.
-145E44/145E54 decode the bracketing keys;145E68 calls existing quaternion interpolation14FD00;145E78 performs hemisphere correction14FF70 against the later decoded key. At/after the last key,145E90 decodes the last key directly. Candidate retains existing strict upper interval lookup, final-key handling, quaternion interpolation, and hemisphere behavior in49T. Existing bounded admission rejects samples before first key and fraction samples past last key, while direct-time sampling allows the source endpoint behavior.

## Bounds and actual authenticated assets

Reader retains256KiB MOT, frameCount1..256,64 tracks per channel,1..256 keys per track,4096 total keys, row0..63, finite strict times in[0,duration], and finite Bezier controls. It adds short-angle<4096. Both formats consume every byte exactly. Accessors use bytewise LE reads and signed conversion; no packed structs or alignment assumptions. Failure does not change outputs. Runtime still requires at least two keys per sampled channel, matching existing narrow49T admission; all Rider19 keys satisfy this.

Authenticated Rider19 bytes were supplied from `ui-rider-controller-reconstruction/preview/members/ui/anim/19c*.mot`, tied to UIRIDER.RES SHA256 `112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856` by that worker's `preview/archive-member-identities.json`.

Independent byte inspection recorded in `evidence/rider19-motion-data.json`: six authentic motions (breathe, ent, idl1, idl2, idl3, sel), every header rotationType1/positionType1; three position and30 rotation tracks;31,41 or46 keys per track. Every track exactly consumed using40-byte position and12-byte rotation keys. Maximum observed angle3058, maximum motion22372 bytes. No runtime loader execution is implied.

## Remaining joins and checks

1. Parent integrates only after exact preimage review. Rider worker binds original MCF channels to original PSG nodes and uses49T for character playback; character animation timing/render/palette remain that worker's owners.
2. Parent's next authorized hourly software check must compile and exercise both legacy ByteAxis and ShortAxis paths, all six authentic Rider19 motions, direct-time endpoint behavior, angle4095/4096 boundary, truncation, output preservation, and no-allocation behavior. None was run here.
3. Rendering and native host-math numerical acceptance still need authorized runtime evidence. Do not call this candidate original bit parity or validated animation.
