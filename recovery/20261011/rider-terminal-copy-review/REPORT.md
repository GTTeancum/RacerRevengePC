# Copy final source terminal once

Date:2026-10-11. Exact preimage commit:`18c25134a8a7c6893822d21dc9ec8ea8a881ebdf`.

## Result

One-file source-only performance refinement to `platform/linux/native/original_ui_rider_draw_contribution50kg.cpp`. The candidate still processes every actual packet's representative binding in order, but copies the terminal source GS/provenance arrays and pass scalars only once, after the complete pending binding-history update.

No renderer, source acceptance, publication, movement, clock, audio, or runtime caller change. Root retains integration and the regression slot. No measured speedup is claimed.

## Equivalence argument

Before this refinement, each ordered packet overwrote the terminal with its canonical representative's identity/provenance and copied that representative's effective GS state after its real `recordDrainedBinding`. The only surviving terminal value was the final packet's value.

The candidate retains every leaf/representative/registration bounds check, every selected actual-image lookup, and every `recordDrainedBinding` call in exactly the same sequence. It saves only the last successfully processed leaf index. After the loop, it reads that leaf's same canonical representative and its final pending state and performs the same terminal copies once. No operation after the final packet changes pending material state; no registry/binding vector is resized by this loop. No representative or pending-state lifetime is extended beyond the existing synchronous composer call.

Repeated leaves still receive every actual bind, including their final selected ATM image and effective IPUM TEXA mutation. This is not an optimization of binding updates or a representative deduplication; only repeatedly overwritten terminal copies move.

## Rejection and empty-state preservation

- Each packet still checks `initialized()` at the location where the old `copyEffectiveGs` could reject. The binding owner's `initialize()` accepts only a prepared key and sets both construction/effective state before setting initialized. `recordDrainedBinding` copies that construction key and changes selected GS fields without clearing prepared. No other method mutates these private states. In the admitted object-state domain, initialized therefore implies effective prepared, exactly preserving the old per-packet readiness rejection without copying the array.
- The one final `copyEffectiveGs` still checks both initialized and effective prepared before any frame seal.
- Textured binding failures retain their original per-packet error source-PC because `recordDrainedBinding` remains before the initialization check.
- If the order is empty, the terminal remains default-empty and no final leaf is dereferenced. Existing earlier geometry validation and downstream renderer empty-span rejection are unchanged.
- Any earlier binding/validation/allocation failure still returns before the frame seal and marks the composer failed. Pending history still commits only after successful actual source acceptance.
- Copying fixed scalar/array fields cannot throw; moving those copies does not defer a fallible operation past another binding update. The pending-state allocation/copy operations remain in the original locations.

The nearby pending-history comment now says source **acceptance**, matching the integrated split-source contract.

## Artifacts and verification

`preimages/` and `files/` contain the exact old/new path. `PATCH.diff` is the one-file patch. Hash manifests pin preimage, candidate, patch, and reports. Independent source review is recorded in `INDEPENDENT-SOURCE-REVIEW.md`.

Only source reads, textual editing/diffing, and hashing were performed. No build, test, syntax check, project parser, importer/decoder, GL probe, native/guest launch, sanitizer, PS2 emulation, or VU emulation was run. This is source-level removal of redundant copies, not a runtime performance result.
