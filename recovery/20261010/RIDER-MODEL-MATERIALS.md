# Rider model material and texture population

Source-only new cold owner; no builds, syntax checks, project parsers, tests, or guest execution. No destination readiness is implied.

`OriginalUiRiderModelMaterials50KG` binds a real loaded character or pod's `Scene48D::materials` list to the existing authenticated PSM/DefaultMT reader and actual ScreenAssets owner. It preserves material slot order, duplicate material names, every declared pass, authored program spelling, and original texture provenance. Undeclared spare PSM sections are not interpreted as passes. Static zero/one-map passes are supported. Actual ATM, missing, ambiguous, unsupported, or malformed inputs fail without replacement textures.

Pod loading requires the exact shared `podTextures` archive identity from the sibling `pod-texture-archive` candidate. Original `1B7630..1B7838` loads UIPODTEX and creates/retains named texture resources before releasing that archive. Pod body PSTs are supplied by that original archive; Rider-specific cable/window/shadow/specular PSTs remain in UIRIDER. Both archive owners are pinned and ambiguous same-name collisions reject. Character loading needs UIRIDER and APPSTART only; real `19c_1` additionally requires the typed authentic IPUM cache.

## Actual initial Rider19 materials

Pod PSG slots: `19_1`, `19_2`, `cable`, `pod_wndw`, `19_shdw`, `19_2`. Character PSG slots: `19c_1`, `19c_2`, `19c_2`. Duplicates are meaningful and retained.

- `19_1` and `19_2`: two single-map passes, DiffuseCompressed body then SpecularMapCompressed specmap1.
- `cable`: one DiffuseCompressed pass.
- `pod_wndw`: three single-map passes, DiffuseCompressed window, SpecularMapCompressed specmap1, DiffuseCompressed window again.
- `19_shdw`: original lowercase `diffusecompressed` program name remains unchanged.
- `19c_1`: one DiffuseCompressed pass, actual kind2/five-mip PST.
- `19c_2`: one declared pass; spare specular section is unused.

The authenticated Rider PSMs contain no two-map SpecularMapDual pass. No generalized two-map material reader is needed for these assets. SpecularMapCompressed still requires its genuine renderer/alpha ownership; this cold owner does not relabel it Diffuse or drop its pass.

Owner-local image IDs are deliberately not globally meaningful. The combined image bank must remap each actual owner/index/member identity and each subsequent renderer material span must use that combined index and identity. Actual mip presentation policy and program admission remain with the renderer. All source image mip records remain owned by ScreenAssets.

## Files and evidence

New files are under `files/src/assets`. No existing-file replacements. The independent bounded data reader and `archive-member-identities.json` authenticate all original Rider members against the retained catalog and materialize only necessary cable/window/specular dependencies. Original body PST identities are in the sibling pod-texture-archive report. These independent reads are data research, not native decoder validation.

Remaining complete destination owners include concrete dynamic backend entity/resource registration and draining, full ordered frame composition, per-model original lighting cache semantics, source-specular alpha ownership, all selected/unlocked Rider assets, application/profile transitions, and root-run validation.
