# Independent source review: terminal copy once

## Verdict

**No correctness findings. Source-equivalent for the composer's admitted, non-reentrant lifecycle.** The patch preserves every per-packet binding update and its error handling, then projects the same final canonical representative's construction provenance and effective GS state once. It is suitable for the root-owned regression gate. This verdict is source review only, not a build, test, runtime, or measured-performance result.

Reviewed against baseline `18c25134a8a7c6893822d21dc9ec8ea8a881ebdf`. The checkout was at that commit with an empty `git status --porcelain` result. The staged preimage matches that commit's file byte-for-byte by SHA-256. A fresh textual unified diff of the preimage and staged file matches `PATCH.diff` by SHA-256.

## Scope and source identity

Only the staged `platform/linux/native/original_ui_rider_draw_contribution50kg.cpp` changes implementation. The diff is confined to `finish()` terminal projection, plus a comment changing “publication” to “acceptance.”

- Baseline/preimage SHA-256: `98ec5a291905f3a8896e2fcdd5e372ab83d200f604bc4536b22f3b42c00228d6`
- Staged file SHA-256: `0fd73e6d9281e75df94a6347357a8e3340c2380b92791bae5e04d437e7094d37`
- PATCH.diff / independently generated textual diff SHA-256: `12ad7df69f7b6c0ca1a16e781965485a2f627cb2b81c2ee542bd147e18d06ef4`

Supporting sources reviewed from the baseline:

- `src/assets/original_ui_material_binding50kg.h` and `.cpp`
- `src/assets/original_ui_material_equivalence50kg.h` and `.cpp`
- `src/assets/original_ui_material50kg.h`
- `platform/linux/native/original_ui_native_draw50kg.h`
- The composer's registration, bank remapping, frame setup, and submission completion paths
- `src/game/original_ui_rider_backend50kg.h` and relevant `.cpp` frame-lifetime code

Line references below use the staged composer unless identified otherwise.

## 1. Binding history and final terminal equivalence

The packet-validation, sorted drain order, ordered vertices, spans, and native state construction at lines 648–679 are unchanged. The ascending final-leaf / FIFO-ordinal comparator is unchanged.

Within the history-staging loop (685–708), each packet still:

1. Resolves the same canonical leaf and representative.
2. Performs the same leaf, representative-index, and registration-index rejection.
3. For a textured representative, calls `recordDrainedBinding()` exactly once on the same pending registration, with the same bank and the packet's actual selected image.
4. Immediately returns on that call's failure with the same schedule error and binding source PC.
5. Checks state readiness before proceeding to the next packet.

The loop is not reduced to a call for each leaf or for the terminal packet alone. Repeated packets, redirected aliases, animated-frame selections, and nonterminal leaves retain all original binding updates and validations. Untextured representatives still perform no texture bind. The retained history state for every representative is therefore unchanged, including the final real image within each FIFO.

Previously, each successful packet overwrote a local terminal value. Only the last overwrite survived to `frame_.terminal_`. The new `terminalLeaf` is assigned after each successful packet and therefore names exactly that last actual drain leaf. It is not selected from the highest registry row, an empty registered group, or a packet's native prototype.

After the loop, lines 710–728 read that same representative and pending registration. Its rank, pass, leaf, generations, construction field provenance, enabled/map/UV values, and sampler fields are the same values formerly copied on the last iteration. Its effective GS state is read after the identical final binding update. For IPUM, this includes the same three TEXA changes at binding implementation lines 63–70. Construction provenance still comes from the immutable source pass; effective values still come from binding history. These two sources are not conflated.

There is no later packet after the chosen final packet that could change the outcome of this equivalence argument. The local terminal is not observed between old loop iterations; the frame receives it only after successful completion (line 746).

## 2. Proof of the readiness invariant

The old per-packet `copyEffectiveGs()` rejects `!initialized_ || !effective_.prepared_` (binding header lines 49–52). The patch replaces these intermediate copies with `initialized()` and retains the full `copyEffectiveGs()` check for the final projection.

For every reachable registration state in this composer, `initialized_` implies both construction and effective keys are prepared:

- Both flags begin false in their respective default objects (binding header 54–55; equivalence header 44).
- Key preparation builds a local key and publishes it only after setting `prepared_=true` (equivalence implementation 81–139). Key fields are private; preparation and the binding class are the relevant mutation paths.
- `initialize()` rejects an unprepared key and an already initialized receiver. Its successful path copies the prepared key to both `construction_` and `effective_`, sets `initialized_=true`, and commits the local object (binding implementation 11–18).
- The composer only appends newly registered history records after successful `initialize()` (composer 605–611). Existing records are retained or copied transactionally.
- `recordDrainedBinding()` starts from a copy of the current state and resets the effective key from the prepared construction key. It may change three GS scalar fields, but never clears either prepared flag or the initialized flag (binding implementation 58–76).
- Successful copies/moves used by this lifecycle preserve these scalar flags. Observers do not mutate them. A failed staging copy or binding operation does not produce a successfully sealed frame.

Thus the intermediate readiness rejection is equivalent for admitted states. An artificial private-state corruption with `initialized_=true` and `effective_.prepared_=false` could distinguish the two implementations on a nonterminal packet, but there is no supported mutation path producing that state here. The patch appropriately retains the per-packet initialized check rather than silently relying only on the final leaf's readiness.

## 3. Bounds, ownership, and lifetime

- Packet bindings are validated before sorting; order entries are generated directly from those packet indices. The existing 2,048-span cap bounds the packet-index conversion.
- Every value assigned to `terminalLeaf` has passed both representative-array checks and the registration-array check in that same iteration.
- No subsequent operation in the history loop resizes or changes `bindings`, `representatives`, or `pendingHistory`. `recordDrainedBinding()` changes one state object, not vector shape or mappings. Consequently, re-resolving the checked final leaf after the loop is safe without duplicate bounds checks.
- The unresolved sentinel is `0xffffffffu`; packet validation already rejects that value. The post-loop guard therefore cannot confuse an admitted last leaf with “no packet.”
- Source GS fields and both terminal provenance arrays use `originalUiGsFieldCount50KG`; the loop bound and destination extents match. `copyEffectiveGs()` assigns an array of the same fixed size.
- Source pass pointers originate from real pass references in `add()` and are checked through schedule construction. They are separate from the mutable pending binding-state objects. Delaying their scalar reads does not make them aliases of changed state.
- The existing non-reentrant frame lease and pinned resources/bank remain in force. The patch introduces no callback, owner transition, pointer retention, or concurrency mechanism. The terminal remains an owned, pointer-free value copied into the sealed frame. It does not extend any borrowed lifetime.

## 4. Empty frames, diagnostics, and failed submissions

- Truly empty geometry still fails the unchanged `E::bank` precondition at 652–655. Nonempty input vertices with no packets still fail the unchanged ordered-size/capacity check before history staging. An empty drain cannot newly produce a successful frame.
- The local terminal remains value-initialized; the sentinel guard avoids selecting a fabricated representative when there is no drain.
- Leaf/representative/registration errors remain `E::schedule` with the same default source PC. A binding failure retains its specific binding diagnostic source PC. An uninitialized textured state still fails inside `recordDrainedBinding()` at the original source PC; an uninitialized untextured state still fails with `E::schedule` and the default PC.
- No moved terminal field copy has a diagnostic side effect. Moving those copies after binding updates cannot suppress an earlier packet's binding validation or error.
- Missing page, allocation failure, and every other preexisting failure still prevent terminal/frame sealing. Tentative history updates remain in `pendingHistory`; authoritative `history` is untouched by `finish()`.
- `finishSourceSubmission()` remains unchanged: acceptance swaps staged history into committed history; a rejected started submission poisons the history; ordinary rejected staging does not commit it. `drain()` clears the pending state. The comment's use of “acceptance” is consistent with this actual commit condition.

## 5. Exceptions and performance boundary

The moved work consists of fixed-size scalar/array reads and assignments. `initialized()` and `copyEffectiveGs()` are `noexcept`; the GS arrays contain scalar integers. No allocation, potentially throwing string operation, external action, or callback was moved across a binding update.

The potentially allocating history copy and every potentially allocating `recordDrainedBinding()` operation remain present and ordered as before. Their existing exception handling and the composer's outer allocation-failure catch are unchanged. No exception path commits tentative state or publishes a partial terminal.

For N successful ordered packets, terminal construction provenance and effective-GS array projection drops from N copies to one, while all N packet-specific history operations and checks remain. The final owned terminal assignment to the frame also remains. This is a source-level work reduction only; no speedup or runtime measurement is asserted.

## Verification limits

Performed only source reads, textual diff inspection, Git metadata/source reads, and hashing. No builds, tests, syntax checks, project parsers/importers/decoders, GL probes, native/guest launches, sanitizers, PS2 emulation, or VU emulation were run. No implementation was edited. The root retains the regression slot.
