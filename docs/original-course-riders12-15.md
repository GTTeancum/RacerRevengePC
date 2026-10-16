# Supplemental original Course racers12 and15

Source-only incremental patch over the combined original Rider/TA Course source.

- Course capability now covers4,10,11,12,15,19 through one shared domain array.
- Catalog readiness is per original racer, using the same authenticated cold
  roster helper that the eventual target world uses. Missing imports disable
  only that rider. Selecting a new rider uses the typed destroyed-world/cold-load
  handoff; no live rig or resource identity is retagged.
- Supplemental rows are found by exact POD identity, not row0. Multiple prepared
  originals may coexist. Only the selected complete row replaces POD01; seven
  authentic opponents remain. Existing-field4/10/11 still use the complete-row
  swap. Unsupported identities never silently map onto POD19.
- Racer names come from the original PHY: POD12 "Shrivel Braittrand", POD15
  "Teemto". Their own RES, PHY, imported model and audio identities remain required.
- The retained host-menu eight-entry cycle remains unchanged. Its permutation
  helper rejects all supplemental Course identities rather than returning POD01.

## Actual source exception

POD12 left-engine VUT authors `binderleftt1=binderleft1`. Its PSG owns
`binderleft1`, not the misspelled lookup key. The original153248..15325C behavior
logs and skips absent nodes. The native admission applies exactly that existing
behavior only to this authenticated VUT/PSG pair, requiring one skipped key at
byte15. It does not repair names, invent rows, or relax other source admission.

- VUT SHA256: cb7148c2199a65c9a6378f0d3c8a14e646359925700bea4133dc94389db38894
- PSG SHA256: bbd1a0452fecc07503104461c73d60e2d086b728ae04765e5c1cd57d4c1cdcb2

Both selected chariots retain actual hand/shoulder/target nodes and conventional
three-model topology. POD15's zero-motion engines remain genuinely empty and use
the existing native procedural engine path. No extra motions are synthesized.

## Production preparation and limits

Original source inputs are retained under
`/workspace/shared/unlocked-race-originals-20261012`, with readable original text
and motion/model bytes under the corresponding original-text/original-motion
folders. Existing native import and visual-selection tools must prepare each
selected original, not reuse POD19 outputs:

- `prepare_original_roster.py --additional-pods POD12 POD15 POD19` with the
  existing importer/disc/output arguments and a fresh output directory.
- `select_original_roster.py --additional-pods 12 15 19` with the existing48F
  archive/raw-import/output arguments and a fresh output directory.

These are source preparation instructions, not executed commands. Runtime also
requires each selected pod's original RES at its canonical disc path and genuine
category/Dax audio files. Source metadata availability is not successful native
cold construction or gameplay acceptance.

POD20 and POD02 remain outside this patch's runtime capability domain. Their
additional model/animation ownership work is independent. No builds, tests,
syntax/import checks, model/motion/image decoders, or gameplay were executed.
