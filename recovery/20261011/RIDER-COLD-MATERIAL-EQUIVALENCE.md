# Bounded native material-pass equivalence

Source-only recovery, 2026-10-11. Two NEW native files, staged outside the active checkout. No compiler, syntax checker, test, sanitizer, project parser, gameplay, guest execution, interpreter, or chip/device implementation was run or added. Static original-byte listing helpers, original text/source reads, file writing and hashes only.

## Delivered API and exact bound

`files/src/assets/original_ui_material_equivalence50kg.{h,cpp}` supplies a private prepared value key and a tri-state comparator. Preparation copies resolved native material state, not guest memory or an encoded command stream. Default keys are unsupported. Failed preparation preserves output and reports a source PC/field. The enum outcomes are `unsupported`, `notEquivalent`, and `equivalent`.

Admitted programs: ScreenObject, Diffuse, DiffuseCompressed, SpecularMapCompressed, under the original case-insensitive program-name lookup. Zero/one-map builder2476A0 only. Source data must be the actual unchanged material owner and resolved canonical PST/ATM provenance, with the same video/depth construction state for the entire comparison domain. Different known templates may merge if their resolved states match. Material filename, pass ordinal, token order and pointer identity of the material are not equality fields.

IMPORTANT: this key covers COLD construction-state registration. It is not enough for arbitrary post-draw14D440 rebuilds. Original249610 mutates the shared shader+30 TEX0 and sometimes TEXA;29BEA0 clears only group indices. Retained drawn and new material passes must not all be silently reset to cold state in a faithful dynamic scheduler. That later comparison needs a separate native semantic binding-state/history owner. Actual guest GPU addresses or resident-mip history must not be invented, emulated or inferred from image-bank generation. This limitation was sent immediately to root and the composer owner.

Composer integration files are owned by the other worker. Add the new cpp to the existing native target/source-preparation lists when integrated. No build lists or active files were changed here.

## Original comparator249BA0

Primary original ELF SHA256: c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Original-data file: `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`. For inspected segments file offset=VA-0x100000+0x80. Retained listings are static text, never executed.

- 249BB0..249C0C compare program ID(+18), map count(+20), both texture resource IDs(+1A/+1C), and both UV bytes(+1E/+1F). Constructor246FA0 initializes unused slots to-1 and UV0. In the bounded zero/one-map path only the first texture/UV can vary.
- 249C14..249C44 compare flag bits2 and1. 2471E4..247218 establishes bit1 from EnableThisShader;247384..2473A0 establishes zero-map bit2 from EnableFillColor. FillColor RGBA at+2C is NOT compared. The native key preserves this distinction.
- 249C58..249C8C compare state block size and all state-data bytes after its fixed header. Constructor2476A0 always zero-initializes the same256-byte allocation, sets count15, and emits the same constant command skeleton. Its complete variable native inputs are the35 resolved state values plus six raw TEX1 values. Within admitted nonoverlapping field widths, fieldwise equality is equivalent to equality of its constructed state data. No guest packet is reconstructed or interpreted.
- Even inactive blend/test fields, CLAMP limits, TEXA values and raw mip/filter/LOD settings remain compared. For example minFilter1 versus5 cannot collapse merely because native MXL0 samples both with linear texels. Negative K is retained as its original signed input; it is not collapsed to a native sampler choice.
- The external video ZBUF base/format read at248060..2480A4 is common only under the explicit unchanged construction-owner precondition. The key does not claim to compare materials across modes/targets/lifetimes.
- 249C9C..249CF0 rejects either pass when shader+10->0C->18 is nonzero. This is specifically the unique texture-animation exclusion. 29B890 calls29B950,29BAF0,29BBD0 and installs an animation owner through29AAA0. 29B950 reads TextureAnimation default none;29BA84 initializes its+18=0;29BA98..29BAA8 sets+18=1 for unique. Binder/arrow/medstrsm unique texture animation never merges, even with itself and even at the same frame.
- 249CF4..249D18 rejects ShaderPassGroup=-2(unique), otherwise compares the group intern ID. This is independent of TextureAnimation=unique. Named animation modes are currently rejected by this bounded preparation, rather than guessed. Uncovered authored fields reject; absent-value placeholders are ignored like the existing source reader.

## Correction: group strings are case-insensitive

Older ui-main-registry SOURCE-EVIDENCE describes named groups as exact-string equality. Directly inspecting308F00 establishes ASCII uppercase classification and+32 folding at308F30..308F50 and308F90..308FB0. Both named-group intern299330 and special none/unique parsing247530 call this function. Therefore TOP and top name the same group, just as NONE and none select the same sentinel. The native key folds ASCII group strings; it does not assign order or a priority to top.

## Texture identity and unique ownership

The supplied image provenance must resolve to the actual canonical original member. Whole PSTs require offset0 and the original byte length. ATM provenance retains the original member and validates its range; the selected frame offset/length are intentionally excluded from equality. CEL metrics, native bank-local IDs and dimensions cannot replace resource identity. Source material registration collisions/aliases must already be resolved by the real registry owner, not guessed here from matching bytes or basename.

Unique texture animation remains a merge exclusion, not an instance tag. No pointer-self shortcut is valid. The comparator does not own or advance clocks, create unique instance state, or claim that one shared binder asset is one live binder animation.

## Confirmed post-draw gap

`evidence/shared-texture-binding.words.txt` shows249804 loading shader+30,249814 replacing its texture state, and249828..24983C replacing TEXA when texture+F8 bit0 is set. `evidence/group-index-reset.words.txt` shows29BEA0 only clears material+5C+4*pass. Existing material-load-rebuild evidence shows14D488 calls29BEA0 before re-registering at14D4C8.

Thus preserving current image-bank generation and rebuilding cold keys is insufficient evidence for full historical group equality after earlier draws. The safe deliverable is the bounded cold comparator plus this explicit integration blocker. No complete-Rider destination admission, full dynamic scheduler correctness, visual parity or runtime readiness is claimed.

## Static review examples, not executed tests

- Same initial specmap1 state across19_1/pass2 and19_2/pass2 is eligible for equivalence; the scheduler still applies pass barriers.
- pod_wndw/pass2 differs in inputAlphaC and fixedAlphaValue, so must not merge with those destination-alpha passes.
- Identical pod_wndw/pass1 andpass3 keys do not grant a merge across their intervening barrier.
- Two binder keys are non-equivalent even when all image/state inputs match.
- Same zero-map state across distinct material names can be equivalent; shader group none versus top prevents equivalence.
- TOP versus top does not prevent equivalence; group unique always does.
