# Static review receipt

Review completed on 2026-10-10 at approximately 16:40 UTC. No executable validation was performed.

Reviewed complete staged files against preimages and the active source, including candidate storage and relocation, collection/membership, previous accepted matrices, racer-bank lifetime, animation/collision publication, terrain/type3 rejection and admission alias guards.

One concrete issue was found and corrected: the new Gate binding was initially admitted as read-only even though callback dispatch mutates its state, receipt and counters. Both indexed-query and live-wrapper guards now admit the one unique Gate binding as a writable span before validating all immutable inputs. Duplicate references to the same binding are allowed; distinct Gate bindings reject. The indexed guard has 12 slots for its original 11 plus Gate, and the live guard has remaining capacity. The header now explicitly distinguishes immediate irreversible action side effects from transactional numerical outputs.

The correction was reread. No further concrete static integration defect was found. This does not establish build, runtime or source-hardware equivalence.

Confirmed limitation: First-method Gate collection retains prior cached-sphere/list admission. The new swept narrowphase cannot detect a crossing that was never collected. This is documented in the main README and must not be described as universal tunneling protection.

Existing preimages matched active source bytes during review, including the terrain driver's newer fleet telemetry. The stage does not replace or delete that telemetry.
