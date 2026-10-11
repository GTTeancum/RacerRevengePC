# Rider cold schedule: canonical window registry row

## Finding and narrow correction

The original material table contains `ui\textures\pod_wndw` twice. The native scheduler currently rejects its second occurrence, although the original registry retains both rows and resolves a material name to the first matching row.

The staged single hunk preserves rank **2490** only for the authenticated later `pod_wndw` token at ordinal **3221**, byte **70650**. Other duplicate names still reject. No pass, model slot, image, reference or registry token is removed; no cold owner becomes registered.

This is a concrete source contradiction on the Rider19 cold path and matches run02's `rider=17`, `rider_draw=13`, `rider_pc=0`, with the nested resource/material errors clear. The log does not uniquely identify every sourcePC-zero schedule site, so this is not a claim that a rerun has succeeded or that no later gate can fail.

## Original evidence

- The retained 70962-byte `Mat_Stbl.txt` hashes to `a3b786f57a1a5f468e649a7f1b3d076a95c318a2fab2f622b754038d01dedd35`, exactly the APPSTART catalog identity. Space/CR/LF tokenization gives 3235 tokens. The window occurrences are rank 2490 / byte 57229 and rank 3221 / byte 70650.
- Loader `29887C..2988B0` appends every token's row, with object/reference fields initially zero. It does not remove or replace duplicate names.
- Lookup `298C80` begins at row zero, compares ascending rows, and returns immediately on the first equality (`298CA8..298CD8`). The real material wrapper calls it at `299CAC`, stores the returned row token at `299CB4`, and passes that token to material reference operation `14D590` at `299D3C`.
- Material loading `14D398..14D3A8` skips rows with nonpositive references. Consequently the later duplicate table row does not establish another window owner through name lookup.
- Authenticated original Rider19 PSG bytes contain the material names `19_1`, `19_2`, `cable`, `pod_wndw`, `19_shdw`, `19_2` at offsets 688 through 1008. The PSG SHA256 is `b86f1e1cf4c116bcc452e82ab9ed7ffedf99bf221744cec6e98c31b5c942cda9`, matching the Rider catalog.
- The real window PSM SHA256 is `0b5ba9f38a5db1ca4670dcc41e91efd847b0ed9844a26c8b954d9bb6b9e28429`, also matching the catalog. It declares three passes: DiffuseCompressed/window, SpecularMapCompressed/specmap1, DiffuseCompressed/window. All three remain distinct passes under canonical rank 2490. Repeated slots and specular references elsewhere are unchanged.

Raw retained files, byte observations, native source excerpts and run02's log are in `evidence/`. The original registry instruction listings and independent diagnosis are preserved under `independent-audit/`.

## Why cold, unregistered bindings reach this gate

Session bind first preflights the real Rider19 navigation resources, then calls `collector.prepareBank` before any model attachment. Backend entries therefore own genuine material/image data while `materialsRegistered` remains false. The draw collector adds every bank-resident model slot/pass so every uploaded image has its actual sampler policy, including cold and just-detached owners (`prepareBank`, lines 210–227 and 264–272).

The scheduler resolves and validates every such binding's table rank before selecting only `sourceRegistered` bindings for source draw grouping. Skipping cold bindings would lose this admission check; removing them would also lose the required sampler policy. The staged change retains both distinctions: cold bindings still resolve to their original name, then retain unresolved draw leaf/registration until a genuine owner registers.

Each window pass and alias independently sees the first token and keeps rank 2490. The new `continue` advances only the inner binding loop; the outer token ordinal still advances and the final 3235-token check remains. Existing rank/pass sorting, contiguous pass checks, duplicate-reference identity/provenance checks, dependency barriers, binding history and real-registration checks are untouched.

## Integration artifacts

- `preimages/platform/linux/native/original_ui_rider_draw_contribution50kg.cpp`
  - SHA256 `263d74234dc98e6d79c641693f8499934f00f98eb65e3e14b3e86bce6b6b4932`
- `files/platform/linux/native/original_ui_rider_draw_contribution50kg.cpp`
  - SHA256 `e2293e491fd891ff1e5f4f599f17325c8a6823b23f4555a5cf82a9e92181e549`
- `canonical-window-registry.patch`
  - SHA256 `4782d0583a2800af46bd4f9330c3c868983b125ec0981027478f06ceb1e445b4`

The relevant active source is unchanged from run02 commit `3d6e953` through inspected HEAD `54f6cbba210b49bd555be9569dbe34f2d22f302c`. Root owns integration. `REVIEW.md` records independent source review.

No extra diagnostic overlay is needed: session bind already propagates `d.draw.sourcePC` into the reported Rider PC. Unexpected duplicate cases in this hunk now identify original lookup `298C80`.

Source inspection, original-byte reads, ordinary text utilities, copies, diffs and hashes only. No build, test, syntax check, project parser/decoder/importer, native program or guest execution was performed. Central runtime validation remains pending and is owned by root.
