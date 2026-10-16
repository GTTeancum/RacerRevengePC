# RA/RC omitted ATM initial phase: bounded assembler proof

Cloud-worker source-only candidate. Changed files are tools/assets/assemble_ryloth_runtime.py and this proof note. No project source was executed or imported; no preparation, converter, build, syntax check, test or gameplay run occurred. Original executable bytes were read as data through ELF program-header mappings; no guest instructions were executed.

## Canonical inputs

- Assembler: Library libfile_ea473030cd548191bb5d1d8713ea730c, file_000000006fb481f5aa9efd3fe846eeba, 40425 bytes. The complete Library text was read; the omitted final newline was restored when recording the isolated baseline. The Library source itself was not replaced. Local baseline commit19a8e38 is packaging scaffolding, not an integration change; apply only the subsequent two-file patch.
- Shared native source: Library libfile_0cf41810596081919fe5da249962bfe7, original_ui_texture_animation50kg.h, 6572 bytes. Config initialFrame is float0, initialization copies it to phase, valid initial phase is finite and 0<=phase<frameCount, count1..64. selectOriginalUiTextureAnimationFrame50KG truncates the binary32 phase toward zero, with retained source anchors29AFDC/29B028.
- Original disc/SLUS_202.68 SHA-256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30. Read only from the authorized original disc directory, never executed.

## Original static proof of the absent-field default

Virtual addresses below were mapped through the actual ELF PT_LOAD headers, not guessed file offsets.

| Original address | Word / data | Meaning relevant to this change |
| --- | --- | --- |
| 29B9C0 | 44806000 | MTC1 zero,f12: +0.0 default argument |
| 29B9CC / 29B9D0 | 3C02003C / 2445DB50 | Build key pointer3BDB50 |
| 3BDB50 | animatedMapInitialFrame | Actual original ASCII key |
| 29B9D4 | 2606000C | Result destination is controller+0x0C, the float phase |
| 29B9DC | 0C0562E0 | Call original158B80 numeric lookup |
| 158B98 | 46006506 | Preserve supplied f12 default in f20 |
| 158B9C | 0C056090 | Call field lookup158240 |
| 158BA4 | 10400009 | Missing lookup branches toward fallback |
| 158BDC | E6140000 | Store f20 default to caller's output |
| 29B9E4 / 29B9E8 | 3C023F80 / 44826000 | Next FPS lookup separately receives1.0 |
| 3BDB70 | animatedMapFramesPerSecond | Distinct original FPS key |
| 29AFD8 / 29AFDC | C440000C / 46000024 | Load phase float and convert to integer frame |
| 29B024 / 29B028 | C440000C / 46000024 | Same selection on the alternate getter route |

The shared native port supplies the source-specific truncation interpretation of the conversion instruction. A plain int(authored_string) is not the original path: the constructor stores a floating phase and only the getter selects an integer.

The existing native caller src/game/original_ui_rider_stat_owner50kg.cpp:70 also passes fallback0 to originalUiNumber50KG for this key at sourcePC29B950. src/assets/original_ui_pages50kg.cpp:number/originalUiNumber50KG and src/assets/pod_physics49n.cpp:parsePodPhysicsNumber49N retain the bounded decimal parse into binary64 then binary32. These are source cross-references, not newly executed evidence.

## Bounded change

animation_bindings now receives the actual parsed ATM frame count at all three call sites. A genuinely omitted initial key uses phase0.0 from29B950. Explicit authored initial fields remain authoritative, matched case-insensitively and retained verbatim (including source offset) in initial_frame_provenance.authored_field. Original material fields and PSM bytes are not edited. Provenance distinguishes authored from original_constructor_default and records sourcePC29B950 and phase-to-frame selection semantics.

The initial field is parsed using the retained bounded native decimal grammar: optional sign, digits with optional nonempty fractional digits, leading-dot shorthand, maximum11 characters after expansion, at most9 digits and6 fractional places. Exponents, underscores, empty text, NaN/Inf, suffix junk and wider numeric syntax are rejected as outside this bounded native subset, not replaced by zero. Numerator and denominator are exact integers; division yields binary64 and struct packing stores binary32 as in the retained native conversion. Native parsed zero is canonical +0 even for an authored negative zero. This does not claim admission for every textual syntax accepted by the original library parser.

Phase must be finite and in [0, actual frame_count), with frame_count1..64, before math.trunc selects the integer frame. This prevents a negative fractional value being accepted merely because truncation produces zero. An upper-edge decimal that rounds to frame_count in binary32 is rejected before selection.

There is an important existing output limit: the generated animation index uses %du for initialFrame, and retained native ATM fixture owners initialize float phase from that uint32 field. Silently emitting trunc(1.5)=1 would change future animation timing. This patch therefore rejects any nonintegral binary32 initial phase with an explicit phase-preserving-row diagnostic. Such a phase can be valid in the original game; it is unsupported by this assembler's current output schema. Integral binary32 phases (including equivalent explicit decimal spellings) are admitted and recorded in both initial_phase and initial_frame. A future fractional extension must preserve phase in the generated/native contract and compare shared bindings by phase, not only selected frame; it is outside this patch.

Completely missing keys alone receive the default. Empty/unspecified declarations remain rejected when no authored value exists, avoiding an unrequested precedence/empty-field expansion. FPS still must be explicitly authored, finite, positive and <=60, exactly as before; although the constructor has an FPS default too, this patch does not broaden that gate. Multiple authored starts for a shared ATM remain rejected by existing selection gates. Every archive/hash/material/frame-count/TGA/geometry/output guard remains in place.

## Consequences and remaining work

The actual RA crowd and finishline pass2 material fields omit initialFrame and author FPS10/FPS16, respectively. They can use source default0 once root validates this source change. No output was generated by this worker, so no assembly-success claim follows. RC uses the same constructor-based rule without a planet-specific default.

TGA remains blocked: LargeFlag, MetalGrate and TwiDesigns02 are authentic RA sources, and this patch adds no fallback, decoder, substitution or texture relabeling. No renderer, native animation schema, shared world code or importer is changed. Root alone reviews, integrates and executes validation.

Suggested root-owned review coverage (not performed): omitted0, explicit0/integral decimal, malformed/empty/nonfinite values, negative fractions, phase at/above actual frame count, binary32 upper-edge rounding, distinct shared starts, and explicit fractional rejection rather than timing loss. Confirm provenance survives both selected-frame and all-frame-bank reports.
