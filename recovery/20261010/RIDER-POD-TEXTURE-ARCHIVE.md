# Authentic shared UI pod texture archive

Source-only candidate. No project parser, build, syntax check, test, or guest execution was run.

The original UIRIDER archive does not contain the pod body textures `19_1.pst` or `19_2.pst`. They are in the separate original `UI/UIPODTEX.RES`. It is 2,653,798 bytes, SHA256 `ffb0f32563e3e5b6e3fefe915bbb37a00610e1a9a44163bce9737c2ac9d6e641`. This exactly matches the retained incoming UI-menu-and-videos manifest. A bounded independent Python standard-library byte reader recovered the original 79-member table and hashes, preserving order, with 4,546,560 logical bytes. Only the two necessary Rider19 texture members were materialized. The report records both original manifest provenance and each member identity.

## Original source ownership

Static ELF listing is in `evidence/source-load-populate-release.txt`. ELF SHA256 is `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

- `1B7630` supplies string `3AC5C0 = ui\\uipodtex.res`; `1B7648` calls `12CEA0` to load/open the resource archive.
- `1B7690..1B779C` iterates pod indices 1 through 26 and four texture slots. `1B76BC` uses format `3AC5D0 = %s%02d_%d`. `1B7704` creates the actual named texture via `24DA30`.
- Successful texture creation is stored at manager `+B8 + pod*16 + slot*4`; `1B7728..1B7768` obtains its resource handle, installs the actual texture via `298F80`, and retains it through `298A30`. The corresponding handle is stored at `+2C0 + pod*16 + slot*4`.
- `1B7830` supplies the same UIPODTEX string and `1B7834` calls `12E5E0` with a2=1 after population. Thus subsequent Rider material resolution uses actual retained shared texture resources, not textures implicitly embedded in UIRIDER.

The native immutable archive owner pins original bytes rather than reproducing the original temporary resource-file lifetime. This is an explicit native lifetime choice; it does not create a resource manager or grant destination readiness.

## Candidate changes

Three exact preimages are under `preimages/src/assets`. Existing ten archive enum values and all 958 member ordinals are unchanged. New `podTextures=10` appends this exact archive and its 79 members. The existing compile-time catalog range check changes to 11 archives / 1,037 members. No source files are registered or integrated here.

The Rider model-material candidate consumes this authenticated archive explicitly for pod body PSTs. The combined image bank must receive the same pinned archive when remapping full member identities.
