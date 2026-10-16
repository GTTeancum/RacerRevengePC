# Add POD20 to the authenticated prepared roster

This is a cold preparation entry point for the central operator. It never
launches the game. It imports only POD20 and preserves the original eight plus
POD19 from the exact retained prepared archive. The proposal has received source
review only: no build, conversion, syntax check, or runtime test is claimed.

## Required inputs

- Python 3.11 or newer and the sibling preparation scripts and identity catalog.
- `rider19-original-roster-prepared-runtime.tar.gz`, exactly 12,860,696 bytes,
  SHA-256 `7e8679cb981f12f0b0444e69fc4bf03b852f6c2d1839a4a2606a5ce7014b3805`.
  This is the only accepted base. An arbitrary extracted runtime tree is not an
  input. The roster root is inferred only when the archive contains one
  `roster.json`; otherwise supply its relative directory with `--base-prefix`.
- Full48F ZIP, SHA-256
  `60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c`.
- Original `POD20.RES`, exactly 309,724 bytes, SHA-256
  `8c5f282b53c230861e26acf797b8ab2ba1ef8856b8b476e3cfea55b48e3040ec`.
  Its PHY must hash to
  `a047bb90bb1275ad5d31e596cea5eaa02fd63ba9bbdc4ef931835f567c87d19d`.
- An explicitly reviewed native `import_original_pods` executable containing
  the narrow original speech INI retention change, and its independently
  recorded executable SHA-256. The executable hash is different from the
  importer source hash; do not use one in place of the other.
- A new output directory under an existing parent, disjoint from every input
  and the source-tool directory. No source asset is copied into its original
  location, deleted, or modified.

The importer retains only `CURSES.INI`, `TAUNTS.INI`, `SPECIAL.INI`, and
`SOUNDS.INI` below `audio/speech/<this-pod>/`. It does not parse those catalogs or
relax the existing supplemental speech digest checks. Existing member numbers,
geometry conversion, material checks, and PHY admission remain unchanged.

## Central invocation plan

First integrate the reviewed source changes and reconcile the exact importer
entry in `tools/assets/ma-sa-preparation-source-pins.json` with the separate SA
bridge. Do not bypass or disable that pin. Build the importer only in the
central authorized operation, and retain its executable hash in the build
receipt. Set the variables below to explicit paths in that operation; no path
in this document identifies a particular user's machine.

```sh
python3 -B tools/assets/original_roster/append_pod20.py \
  --base-archive "$BASE_ROSTER_ARCHIVE" \
  --full48f-archive "$FULL48F_ARCHIVE" \
  --pod20-resource "$ORIGINAL_POD20_RES" \
  --importer "$REVIEWED_IMPORTER" \
  --importer-sha256 "$REVIEWED_IMPORTER_SHA256" \
  --output "$NEW_PREPARED_ROOT"
```

The output is usable only after successful completion and the final
`roster.json` is present. Existing outputs, duplicate additions, links,
reparse points, unsafe archive paths, and overlapping outputs fail closed. A
failed preparation deliberately leaves its new partial output for inspection;
it never deletes that output or retries into it. Choose a different fresh
directory for a later authorized attempt.

The base archive digest authenticates the complete input. The helper derives
a sorted, canonical `base-files.json` from that exact archive, recording every
selected file's original archive member, size, and SHA-256. It checks original
containers, PHY bytes, and three component source/native hashes against the
authenticated full48F generation before staging. Base files retain their
bytes except `roster.json` and explicitly requested resource-path rebases.
`pod20-addition.json` records the input identities, canonical manifest digest,
importer executable digest, additions, path changes, and final roster digest.
The eight racer entries keep their original order, and POD19 remains the first
supplemental entry; POD20 is appended second.

By default, existing `pod.json` resource paths are preserved exactly. They may
refer to the machine that originally prepared the archive; preservation alone
does not make them usable on the destination. To rebase the nine existing RES
paths during preparation, add:

```sh
  --rebase-original-assets-root "$ORIGINAL_ASSET_ROOT"
```

That root must contain `PODS/PODxx/PODxx.RES` for the original eight and POD19.
Each destination RES is verified against the authenticated full48F container
size and digest before any output is created. The original `pod.json` bytes
remain identified in `base-files.json`, while every old/new resource path and
resource digest is retained in `pod20-addition.json`. POD20 uses the explicit
`--pod20-resource` path. Do not move those original resources after preparing
the runtime. Preparation on one host does not authorize silent path rewrites
on another host.

After the additive helper succeeds, prepare a new visual selection using the
existing selector. This copies authenticated cached visuals and original
authoring from the prepared imports; it does not reimport the eight or POD19:

```sh
python3 -B tools/assets/original_roster/select_original_roster.py \
  --archive "$FULL48F_ARCHIVE" \
  --raw-imports "$NEW_PREPARED_ROOT" \
  --audio-root "$ORIGINAL_AUDIO_ROOT" \
  --additional-pods 19 20 \
  --output "$NEW_VISUAL_ROOT"
```

`ORIGINAL_AUDIO_ROOT` is the original AUDIO directory containing `UIACCEPT.INI`
and `SPEECH/PODxx/TAUNTS.INI`, not the outer asset root. Keep `NEW_VISUAL_ROOT`
fresh and separate from inputs and the prepared output. Both supplemental IDs
are required to preserve POD19 in the visual selection. The selector retains
its existing archive, authoring, and name-provenance authentication checks.

Successful conversion does not establish gameplay equivalence, correct
rendering, or an Occo smoke pass. Root owns all compilation, conversion,
integration, and any later focused runtime operation.
