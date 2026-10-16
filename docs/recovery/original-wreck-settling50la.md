# Original wreck settling prerequisite, 50LA

## Delivered scope

`src/game/original_wreck_settling50la.h` implements the complete reached numerical/state leaf at original `167640..167918`. It borrows the existing canonical `BodyState48C`, `SpringRecordState48N`, actual body-enabled flag, and source piece scalar fields. It creates no persistent physics owner and is not a constructor, activation receipt, world-query result, or complete wreck runtime.

The native caller must reach this leaf at its original position in `166E40` or `2CA840`, after real collision/detection, force/integration, scene/query updates, and measured acceleration producers. The helper does not certify those prerequisites. No whole-frame admission guard has been widened.

## Source operations and ordering

- `16764C..16765C`: read piece130. A nonpositive value skips all time/damping accesses, but still reaches the angular norm and settling comparisons.
- `167660..167684`: if either piece34 or piece38 is zero and actual DC.contact3C is nonzero, choose the increasing-time branch; otherwise choose the decreasing branch. Short-circuit flag/contact reads are preserved.
- `167688..1676C0`: update time12C to `MIN.S(time12C+elapsedFactor,2*piece130)`.
- `1676C4..1676E8`: alternate update `MAX.S(time12C-elapsedFactor,+0)`.
- `1676EC..1677A4`: interpolate rateDampingF0 from piece104 to108 over the interval `[piece130,2*piece130]`, then clamp using the original ordinary comparisons. Preserve separate multiply, subtraction, multiplication, division and addition operations. The original equal-endpoints branch writes positive zero.
- `1677A8..167860`: repeat for dragEC from piece100 to10C. Rate is computed before drag.
- `167864..16788C`: always compute the norm of canonical body angular velocityB0. Source scalar instruction order is Y square, X square, accumulated X-square plus Y-square, Z square plus accumulator, then square root.
- `167890..1678EC`: test strict comparisons in order: actual cached body34 speed below piece134, angular norm below138, measured124 below13C, and measured128 below140. Later source thresholds/magnitudes are not consumed when an earlier comparison fails.
- `1678F0..16790C`: only after all four comparisons, read actual DC.contact3C. If nonzero, call body's slot90 with zero.
- Actual `1653C0` with argument zero is exactly `body204=0; return`; it does not traverse linked bodies, clear loads, edit activity29, reset a model, or remove collision memberships. The header can therefore write the existing typed owner's bool enabled field directly for this branch.
- `1653C0` with argument one is a different owner operation: after storing204 it examines body208, traverses actual268/28C linked bodies if needed, enables them, and sets208. This enable contract remains outside this leaf.

The `MIN.S` and `MAX.S` instructions reuse `detail::scalarMin48O` and `scalarMax48O`, including their signed-zero ordering. The two interpolation clamps intentionally use ordinary floating comparisons instead.

## Native boundary

The API accepts only reached normal-or-zero finite operands and intermediates under `FE_TONEAREST`, strict binary32, and no contraction/fast-math. Native admission failure is transactional: time12C, dampingF0/EC, and enabled204 remain unchanged. Success commits the final source outputs. Intermediate source stores are collapsed only within this isolated serial numerical leaf; callers must not alias outputs with each other or inputs, and must not interleave observers during the call.

The borrowed descriptor has no persistent state. Owner/backing lifetimes and serial exclusivity belong to the caller. Output fields are committed in source time, rate, drag order, followed by optional body-disable. Existing damping values, body position/velocity/orientation, unrelated body derivatives, and body-enabled's old value are not inputs. A disabled body does not imply an inactive wreck piece: component activity29 and scene14C remain separate authoritative fields.

## Evidence and work performed

Static source/ELF-word review only. No build, compiler, syntax/import check, test, lint, sanitizer, asset decoder, game run, or PC operation was performed.

Evidence in this packet:
- `evidence/settling-167640-167920.txt`: source leaf words including its return.
- `evidence/body-setters-1652e0-1654c0.txt`: exact disable-zero and linked-body enable contracts.

The words came from the supplied original ELF at `/workspace/shared/input-recovery-20261012/extracted/RacerRevenge-50HS-Race-LinuxRuntime-20261009/RacerRevenge50HS/work/native/project/disc/SLUS_202.68`, using the parent-permitted copied rawword formatter. Existing complete owner report is `/workspace/shared/wreck-owner-research-20261012/REPORT.md`.

The packet's `source.patch` adds this header and this report only. It does not modify the earlier frozen launch packet or central repository. `SHA256SUMS` is an artifact identity manifest, not a build/test result.
