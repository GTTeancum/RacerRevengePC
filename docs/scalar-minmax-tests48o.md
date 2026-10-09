# 48O corrective scalar MIN/MAX regression

`tests/native/scalar_minmax48o.cpp` verifies the shared finite scalar selector
in `src/game/scalar_minmax48o.h` and the corrected native 48L spring helpers.
It is a focused, serial Linux correctness check, with no performance claim.
The historical 48N and earlier archives are preserved. Their evidence is not
silently relabeled as newly validated 48O evidence.

## Independent finite selection reference

The production selector maps raw words into monotone unsigned order keys.
The test reference independently interprets each word as a signed integer,
then reverses the integer comparison only when both inputs are negative. It
never calls the production key function or uses the production transformation.
Expected values are selected input words, so comparison is bitwise rather than
numeric float equality.

Each of four rounding modes runs the same deterministic coverage:

- All four ordered pairs of positive/negative zero, for both MIN and MAX and
  again with the operands reversed.
- The full 24-by-24 Cartesian product of raw-word anchors: both zeros,
  minimum/maximum subnormals, the normal/subnormal boundary, neighboring words
  around 0.5 and 1, and maximum finite values, with both signs.
- 65,536 seeded raw finite pairs, including periodic equal-word pairs. The
  xorshift seed is `0x48c04e71`; exponent-255 words are rejected explicitly.
  The identical sample is repeated under each rounding mode.

This gives **264,464 pair evaluations** and **1,057,856 native MIN/MAX word
comparisons**, including **16 explicit zero-pair evaluations**. The test checks
that selection does not change the caller's rounding mode. Raw subnormal
selection does not establish EE denormal arithmetic or flush-to-zero behavior.

## Original-instruction edge projections

The separately generated `evidence/minmaxAudit48O/edge-fixtures.json` has schema
`scalar-minmax48o-edge-v1`, exactly **8 secondary** and **6 force** cases, and
SHA-256 `6ad3fb19adb023724eabd4afdfdfca2157b91c5aa8d813e59244cc24c4e24f67`.
Its original-word replay has no patched instructions and no uninitialized
source reads. See the audit script and replay report alongside that fixture
for provenance and the saved-corpus comparison. The test reads the fixture
from disk; it does not derive expected spring outputs from the native helper.

Secondary cases execute `removeSpringInwardVelocity48L` and compare every
output word. Six signed-axis cases expose the scalar MIN at `002983F0`:
negative-zero projection must select negative zero rather than positive zero.
For normal `(+0,1,+0)`, cached velocity `(-0,-0,-0)`, and incoming velocity
`(-0,-0,-0)`, the corrected result is `(+0,+0,+0)`. The legacy comparator gives
`(-0,-0,-0)`. Two additional controls distinguish a positive-zero dot from a
positive-zero accumulation destination. All eight fixtures also execute with
the output exactly aliasing each of the three input vectors, giving **24 alias
cases**.

Force cases execute `applySpringForce48L` using the established 76-input-word
layout and compare all **15 output words**, including both recipient loads and
the no-other initialized output. The edge corpus contains original intermediate
selection differences at `0029724C`, `00297294`, and `002972AC`; the final force
outputs of these six cases remain unchanged. The corpus does not claim a
changed-output example at the fourth force scalar site, `002972C8`.

Across both helpers, **10 original intermediate deltas** are additionally
checked against the native selector and independent signed-integer reference.
The test verifies that the legacy comparator selects the saved legacy word
and differs from the corrected word for each delta. The historical 2,048-force
and 512-secondary fixture regressions remain separate checks; this focused
file neither replaces nor modifies them.

## Failure sensitivity and native safety

A test-local legacy comparator replay matches the independently saved legacy
secondary projection and disagrees with the corrected projection on exactly
**6 cases / 18 output words**. The ordinary finite utility coverage also detects
**16 legacy first-orientation selection mismatches** across its explicit and
anchor zero pairs.

For an additional actual-build mutation check, a temporary copy of the shared
header was changed back to `first < second ? first : second` and its MAX
counterpart. A temporary copy of the native spring-force translation unit
used that header, as did the test. Both were built with the same strict flags;
no repository source or oracle was changed. That executable returned **1**
with **114 failed assertions**: 32 utility comparisons, 10 intermediate-delta
comparisons, and 72 secondary/aliased output comparisons. The corrected build
returned **0**. This proves the new regression rejects the prior selector.

The **84 native safety cases** cover positive/negative infinity and NaN in
consumed vector lanes, limits, penetration, and selected ordinary/impact
coefficients; dot, correction-multiply, final-addition, spring, and damping
overflow; same-body rejection; and late opposite-recipient failures. Every
rejection checks that the complete output and immutable inputs remain
byte-for-byte unchanged.

Global scalar, array, and aligned C++ allocation overloads are watched only
around native calls. Fixture parsing, reference execution, snapshots, and
logging are outside those scopes. The verified run observed **0 allocations
in 264,596 watched scopes**, including successful and rejected paths. No
console, original-Xbox, frame-rate, physical EE exceptional-mode, or
whole-game performance conclusion follows from this check.

## Reproduction and observed result

Run serially with the focused runner, using a fresh output directory outside
the repository:

```sh
python3 tools/test_native_minmax48o.py \
  --tests scalar_minmax48o --output-dir /path/outside/repo/minmax48o-check
```

The focused CMake configuration is also available through
`RR_NATIVE_MINMAX_ONLY=ON`; build and run CTest with parallelism 1.
Both entry points use strict floating-point compiler settings. The runner
records source/fixture/compiler hashes and keeps build/test logs outside the
source tree. The parent aggregate rerun is the authority for the complete
corrected snapshot.

The clean GCC development run of this test passed **1,323,100 checks**, with
**0 failures**, **0 compiler warnings**, and **0 watched allocations**. The
intentional legacy-mutant run failed as described above. A focused pass does
not imply that the remaining detector, producer, spring, or terrain suites
were run by this test.

## Final full validation

The final control-inclusive release and ASan/UBSan runs are retained at
`evidence/nativeDetector48O-corrected-linux-20261006/minmax-release/report.json`
and `minmax-asan-ubsan/report.json`. Each passes131,629 corrected control checks
and1,323,100 scalar/edge checks. All current input hashes match; no warnings or
runtime allocations were observed. LeakSanitizer is excluded explicitly for
the traced-host restriction. The two corrective compiled mutations (legacy
scalar selectors and legacy control selectors) are rejected, alongside the
thirteen detector/query preparation mutations.
