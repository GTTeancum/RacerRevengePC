# 50ED: retained Pod01 material/texture fixture

`tests/native/pod_materials50ed_fixture.h` is a development-only cold adapter from the retained `evidence/visual49G` selection to a load-once `MaterialTextureAssets50EA` bank. It does not contain renderer, window, runner, build, guest-execution, or virtual-GPU work. It is a source-first playable diagnostic bridge, not original shader parity.

## Explicit caller policy

`pod_materials50ed_fixture::Request` has:

- `root`: the directory containing `asset-selection.json` and `cacheobjects`.
- `choices[3]`: dense `Choice` vectors in chariot / left-engine / right-engine order, exactly matching every original 50CR scene material entry, including duplicates and entries outside LOD0.
- `coldSourceByteCapacity`: a positive explicit ceiling, at most 32 MiB.
- `textureBudgets`: the caller's complete 50EA resource, selected-slot, serialized-byte, decoded-RGBA-byte and dimension ceilings.

Each `Choice` supplies native `pass` and `slot`, `MaterialTextureRequirement50EA`, `MipMode`, and `mipIndex`. There is no implicit first pass, map or mip. The native Material48E records number passes and slots from **one**: `01_1`'s ordinary diffuse-image selection is pass **1**, slot **1**. Passing zero rejects even under optional policy.

`MipMode::exactIndex` selects the exact retained mip number. `MipMode::largestRetained` explicitly asks for the largest area among mips declared extracted in the selection. The mode is diagnostic policy; neither choice asserts the original renderer selected that mip. A chosen retained file that is absent, corrupt or has mismatching metadata rejects rather than silently substituting another mip. A mip not declared retained can remain unresolved only under the caller's optional-untextured policy.

For the current retained Pod01 pass-1/slot-1 diagnostic, callers can size all three choice vectors to the loaded scene's material counts, then explicitly fill each entry with `{1, 1, MaterialTextureRequirement50EA::required, MipMode::largestRetained, 0}`. No default policy is supplied by the fixture.

## API and consumption

Create `Owner` and call `load(request, loaded50CR, diagnostic)`. Success publishes together:

- `bank()`: immutable 50EA owner, suitable for a separate caller-controlled 50EB upload.
- `component(c)`: immutable dense `Selection` vector. Its array index remains the native material index. `Selection::textureIndex` is the exact 50EA bank index, or `unresolvedMaterialTexture50EA` for an explicitly permitted absent selection. `bankSlot`, native pass/slot, source names/keys, mip, caller requirement and unresolved reason remain inspectable.
- `authoredSlots()`: every actual pass/map on every original material entry, retaining duplicates, exact native numbers, resource-resolution status and whether the caller selected it for display. Unselected maps are not loaded or executed.
- `textures()`: exact `(content key, mip index)` resources, in bank order, with native mip dimensions, retained path and digest.
- `coldSourceBytes()`: the charged serialized-input total.

Root 50DS wiring remains outside this fixture. For each dense entry, map a real texture index to `VehicleWindowMaterialMode50DS::requiredTexture`; map only an explicit unresolved optional result to `untextured` with the unresolved sentinel. Preserve all entries. Upload `*owner.bank()` to 50EB first and use that same publication in 50DS. The fixture neither uploads nor infers sampling, alpha, blending, texture animation, shader uniforms, or original multipass behavior.

Success is load-once and views remain valid until owner destruction. Failed load leaves the owner empty and retryable; a reload rejection leaves published data unchanged. No frame-time use should invoke this loader. `Diagnostic` identifies the component, material, selected native pass/slot and failure text, plus any 50EA diagnostic.

## Exact retained associations

The fixture admits only these source component keys and members in `PODS/POD01/POD01.RES`:

| Component | Exact retained scene key | Original materials |
| --- | --- | ---: |
| Chariot | `cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5` | 10 |
| Left engine | `210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3` | 8 |
| Right engine | `f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60` | 8 |

The 10 chariot entries are `01_2, 01_3, pod_wndw, 01_2, 01_3, pod_wndw, 01_2, 01_3, 01_2, 01_3`. Each engine alternates `01_1` with its own `flame01l` or `flame01r` material four times. Both flame materials explicitly reference the retained texture named `flame`; lookup does not guess from their material basenames. They share one material content key but remain distinct scene/material associations.

Source inspection of pass 1 / slot 1 with largest-retained policy gives five unique texture resources:

| Native texture name | Retained key | Mip | Dimensions |
| --- | --- | ---: | --- |
| `01_2` | `1e31063ae9ea71f8e9d3bd6098b538c27100b91a3285fa382e2904919f04d5e0` | 4 | 256 × 256 |
| `01_3` | `e9c2db8ff9cfcc7bd26cf2e01d37e7c3368bee355c5b10b0de453088e164d65e` | 0 | 128 × 128 |
| `pod_wndw` | `394bcabc19c83b150c6b00a682b710e5de8b3efdd6f4d39a1826c27b902ad1dc` | 2 | 64 × 64 |
| `01_1` | `da9d5d84a83c02b496872f7be04ce69135ec08c3ece3191d2c2d0671434f583a` | 4 | 256 × 256 |
| `flame` | `abec7a3d99994cdedfaf7cab18576f51dc903f89afb20e977e791d97743ac378` | 0 | 64 × 64 |

These account for 622,592 RGBA bytes and 622,672 serialized RRTEX bytes. The choice tables have 26 displayed slots. The authored provenance table retains 40 pass/map entries, including 14 additional `specmap5` requests (the two-map `01_1`/`01_2` passes and `pod_wndw`'s second pass). The selection reports no exact `specmap5.pst`; its same-stem TGA is explicitly non-substitutable. Selecting one of those slots with `required` rejects; `optionalUntextured` retains the requested slot and reason. Requesting an absent native pass/map likewise rejects when required and remains explicitly unresolved when optional. Corrupt metadata never becomes optional success.

## Provenance and bounds

The selected-object and canonical-record hash checks follow `tools/visual/export_surface_bindings49h.cpp`; its existing SHA-256 implementation is reused inline here. Record keys are checked against converter plus original source digest. The adapter reuses the existing `encodeSceneAsset48D` writer to compare the immutable loaded 50CR scene against its retained RRSCENE digest and byte extent; it checks the complete serialization size before allocation. No raw PSG decoding or source resource extraction occurs.

Every material association is exact within the Pod container, includes the actual scene requester and duplicate-use count, and is checked against its retained material record. Every authored texture slot must have exactly one matching material-key/name/pass/slot/program requester. Cross-container candidate selection, duplicate associations, unsupported schema, unsafe paths, unexpected Pod scenes, malformed records and unproven unresolved reasons reject. The adapter never searches directories or substitutes a same-stem resource.

Only generated `cacheobjects/<64 lowercase hex key>` record and numbered-mip paths are opened. The entire selected-object path table is shape checked; cache-directory and final-file symlinks reject. The caller chooses the root. Selection input is capped at 4 MiB; each record at 64 KiB; tables and scene/material counts have hard ceilings. The cold source budget charges selection bytes, each unique read record, the three bounded re-encoded scene byte streams, and unique selected RRTEX bytes. It describes serialized source work, not total allocator usage: JSON trees, hash copies, scene-writer scratch and provenance strings have additional bounded overhead. Unrelated TA cache files are not read.

Unique selected `(key,mip)` payloads are budgeted before reads; expected dimensions and total RGBA/source bytes are checked before 50EA decoding. 50EA independently validates and owns unchanged raw RGBA payloads. Identities are fixture-local dense integers, not truncated hashes: component identities are 1/2/3; present texture identities are bank index + 1; absent requested identities use a disjoint high-bit range. Different selected mips of one original texture have distinct bank identities. Shared content is deduplicated without collapsing native scene material entries.

## Verification limit

This change was authored and reviewed from retained source and record contents only. No build, test, executable, runtime probe, renderer, desktop or virtual-GPU execution was performed for this fixture. The resource/count statements above are source-inspection expectations, not a reported runtime pass. Original game shader/material parity is not claimed.
