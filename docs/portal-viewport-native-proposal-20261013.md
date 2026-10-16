# Native portal viewport proposal — 2026-10-13

Source-only additions to `portal_geometry50eo.h/.cpp`, pending integration with
newer Windows changes and centralized validation. Neither leaf is called by
render traversal. No gameplay culling or performance improvement is claimed.

## Source and boundaries

Original source identity: SLUS_202.68 SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The implementation is native CPU geometry; no guest instructions, registers,
chip state or emulation are executed. The finite, nearest-rounded binary32
arithmetic boundary is explicit, not a claim of exceptional hardware parity.

- `projectPortal230880_50EO` preserves the reviewed projection/clipping ordering,
  six clipping planes, fixed scratch limits and borrowed camera/viewport inputs.
- `intersectPortalViewport263260_50EO` preserves B-before-A containment shortcuts,
  directed inclusive half-planes (`2636A0`), ordered segment intersections
  (`262FE0`), the shared four-point reorder (`263730`), nonpositive turn check
  (`263180`) and the original-order envelope (`2635E0`).
- Intersections retain duplicate points and inclusive endpoint contacts. A zero
  denominator gives no segment candidate; no epsilon or collinear substitute is
  introduced. All 16 edge pairs execute when neither containment shortcut wins.
- Candidate storage is bounded at 24 pairs (at most 22 on the edge-enumeration
  path). The final upper-bound segment branch retains its reached X product.
- Four candidates are reordered, then checked for nonpositive adjacent turns;
  all other positive counts and a rejected four-point arrangement use the
  original candidate envelope. Point/line envelopes remain valid outputs.
- Only `Hit` publishes output; `Miss` means completed-empty. Input, arithmetic
  and capacity failures must never be interpreted as visibility rejection.
- Original diagnostic/assertion globals are not reproduced. Their candidate
  assertion is replaced by bounded native storage and explicit failure status.

## Remaining work

Bind actual camera/434940 viewport ownership, zone traversal, viewport unions,
separate admission/dispatch generations and model observers before enabling
culling. Preserve the documented source ordering and unusual union fields;
do not substitute accumulated AABBs for incoming recursive quads.

No build, test, syntax check, sanitizer, gameplay run or FPS measurement was
performed for these additions. Existing Windows smoke evidence predates them.
