# POD20 current material selection

The retained Full48F generation authenticates original POD20 member identities,
but historical material conversion may be unsupported even when the reviewed
current native importer admits the same original bytes. In particular, root
reported `textures/20_4.psm` (5198 bytes), source SHA-256
`fab09bdafddf6eccd3ab63ac2d506ad4d8bac672edd74df38765bfcc49aa56fc`,
with historical `psm.sections` rejection and current one-pass
`DiffuseCompressed` conversion. This proposal does not itself execute or verify
that conversion.

The selector now offers an explicit POD20-only path:

```sh
python3 -B tools/assets/original_roster/select_original_roster.py \
  --archive "$FULL48F_ARCHIVE" \
  --raw-imports "$PREPARED_EIGHT_PLUS_19_20" \
  --pod20-current-import-sha256 "$REVIEWED_POD20_IMPORT_JSON_SHA256" \
  --audio-root "$ORIGINAL_AUDIO_ROOT" \
  --additional-pods 19 20 \
  --output "$FRESH_VISUAL_OUTPUT"
```

The new digest is the SHA-256 of the successful central operation's
`pod20/import.json`, independently recorded in its trusted preparation receipt.
It is not the importer executable hash, source hash, roster hash, or a digest
taken from an unreviewed replacement tree. Root supplies this digest from the
actual successful operation; this source proposal does not invent it.

Without the new option, material selection is unchanged. With it, a missing
historical conversion may use current metadata only for a single local POD20
PSM member whose historical status is `unsupported`. The current pinned report
must describe exact original POD20 and report `converted_material`. Member
hash, byte extent, logical offset, and retained original PSM bytes must agree
with the authenticated Full48F generation. Missing, duplicate, ambiguous,
different-container, altered, or still-unsupported material data fails closed.

Parsed `general`, active `passes`, authored/unspecified fields, field offsets,
programs, texture-count defaults, and every texture slot are retained from the
authenticated current importer report without rewriting. The original PSM
bytes are retained, including any inactive numbered sections; the selector
does not parse, activate, invent, or delete those sections.

The emitted native record keeps the existing `rr-native-material-48E-v1`
converter, material type, source-based key, canonical record digest, and exact
general/pass schema. The material row and top-level
`current_material_conversion` explicitly identify `current_native_pod20_import`
and retain the historical unsupported row, generation, and current report hash.
The pinned report is copied to `provenance/pod20-current-import.json`; original
PSMs are retained below `authoring/POD20/`. These provenance files are excluded
from the runtime's cache-only `files` table. Only the standard material record
is added to that table. No runtime admission code is changed.

Source compatibility assessment covers every material name requested by the
three POD20 component records, then every active pass and texture slot:

- Historical converted materials continue through the unchanged cache path.
- Every affected historical unsupported local material must independently pass
  the current-report and exact-original-byte checks; there is no 20_4-only
  shortcut that skips another required material.
- For POD20 with this option, active texture slots require a local historical
  Full48F `converted_texture` record, with one exact exception: material 20_1 or
  20_2, pass 1, slot 2, SpecularMapDualCompressed with two slots, may retain
  missing specmap5.pst as an explicitly unresolved binding when no candidates
  exist. The original mounted specmap5.tga belongs to canonical runtime
  authentication; this selector does not create an image or waive that check.
  SA legacy rendering does not thereby gain canonical paired-map coverage.
  All recorded mip objects still pass existing hash checks; other missing,
  ambiguous or unsupported slots fail without substitution.
- A missing/unsupported texture stops selection and names the member. That
  would require a separately evidenced texture change; this proposal does not
  assume the material fix also repairs textures.

`tests/native/original_racer_visual50hw_fixture.h` reads the normal record,
checks converter/equality, reads the original PSM from the selected raw imports,
decodes it with the native reader, and compares all general/pass fields and
texture declarations. Those checks remain intact. Current conversion status
does not waive runtime decode or equality checks.

Limitations: source-only assessment, no tests, syntax checks, builds, decoders,
asset conversion, or game execution. The actual complete POD20 affected-member
list and successful texture resolution remain for root's named metadata review
and central rerun. No claim is made that new records were converted by the
historical Full48F importer.
