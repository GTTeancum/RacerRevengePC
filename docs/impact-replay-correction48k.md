# Correction to checkpoint48J's saved impact replay

Checkpoint48J's `evidence/spring48J/impulse-saved-replay.json` does **not** establish
impact response for the saved player body. Its saved-object interpretation is
withdrawn. The original files and archives are retained as failed evidence.

The replay supplied vehicle offset `+344`, object `00987CD0`, as a spring model.
That object is the gravity component already identified in `vehicle-forces48d.md`.
Its `+30` field is1, not a body pointer. The original impact routine consequently
read low-memory addresses through a bogus primary-body pointer. Replaying the
same accesses from a sparse fixture merely reproduced the invalid setup. The
old check also inspected the expected player body without first verifying that
the original routine had actually selected that body.

The correct saved chain is:

| Binding | Value |
| --- | --- |
| Player vehicle | `00C5B0B0` |
| Vehicle `+354`: SpringCollisionModel | `00D3C570` |
| Spring `+30`: primary rigid body | `00D110A0` |
| Vehicle `+358`: detection component | `0095D0A0` |
| Detection `+30`: spring model | `00D3C570` |

Checkpoint48K verifies both links before replaying the complete original2964C0
coordinator. The unchanged saved state has no positive penetration and therefore
does not call impact or friction. Explicit positive-depth variants of the real
record are separately labeled; they exercise contact preparation, impact and
friction in the original order, including repeated contacts and another body.

The corrective evidence is
`evidence/springOrchestration48K/48j-saved-replay-withdrawal.json` and
`coordinator-saved-replay.json`. The latter preserves exact object binding,
arguments, before/after state, helper order and guarded original-data reads.
The affected48J report's SHA-256 is
`56daf565bac16665e3a7f1f364f7edc02ee0c851be37928812263beadd2be18d`.

This correction does not invalidate the separately initialized48J friction and
point-force fixtures, the independently bound terrain query, or the extracted
original instruction bytes. The new48K isolated impact fixtures use explicit
typed test objects and a known primary-body pointer; they do not depend on the
withdrawn saved-object assumption. Full original races remain unverified.
