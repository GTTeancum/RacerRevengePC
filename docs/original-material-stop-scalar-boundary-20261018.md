# Original material stop scalar boundary — October 18, 2026

## Scope and observed failure

This change adds failure-only material diagnostics and repairs a numerical-admission mismatch on the original stop-only path. It does not change course or racer availability, original resource authentication, human/AI ownership, reset behavior, source RNG, generated-input policy or progression.

Two original-disc attempts on MA/Orotoru G'am reached real race driving and then failed:

- Ben Quadinaros: slot 0 material status 1 (`input`), with source PC 0. The original log did not identify the rejected field. A manual-reset key was attempted near termination; delivery or causality was not established.
- Dud Bolt: diagnostics identify slot 1/Clegg's channel 3 stop, with contact -1, rejecting the shared listener's forward Y, bits `006447c3`, as subnormal. The forward vector bits were `3cb317ff,006447c3,3f7ff056,3f800000`. Other copied position and velocity lanes, and all structural guards, were valid. The run ended before the later attempted reset key.

The listener is the native committed selected-body listener, reconstructed from its quaternion. The finite-only producer permits small components, and the observed DAZ/FTZ settings were disabled. This value does not establish wrong-racer ownership or a requirement to zero a coordinate. Full retail listener scheduling and the exact producing quaternion are not claimed.

## Diagnostic contract

The first failed dispatch records its exact rejected guard, axis and listener index, copied float bit patterns and bitwise classes, channel, contact, class, sound-mode fields, rounding and FP status. Existing input/reset observations are logged without invoking input sampling or reset. The receipt retains its original status and source PC. Logging is failure-only and flushed before teardown; logging exceptions cannot replace the failed dispatch result.

The diagnostic-only implementation preserved predicate order, scalar definitions and admission. It did not retry a failed call or roll back already committed source effects. Diagnostic bit classifications are not substituted for the actual predicate under DAZ/FTZ.

## Source stop correction

Supplied original executable SHA256: `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

Original `1791E8` selects channel 3; `1791F0..179208` clears D40.22C bit 3; `17920C..17921C` calls D44 virtual +A8, which resolves to `2C27D0`; `179224` exits. That helper invokes `2933D0` to stop constructed drive rows and then resets fields 240/244. This path consumes no position, velocity or listener scalar values.

Only the existing numerical-vector loops are conditioned on `channel != 3`. All owner, channel, class, contact, listener shape, sound-mode and rounding checks remain in their prior order. The actual stop block is unchanged. A real stop failure still retains the successful prefix and original failure boundary. No coordinate is normalized or replaced, no FP controls are changed, and no RNG is consumed by this correction.

Channels 1 and 2 retain all previous scalar guards. Original spatial function `102950` does read listener Y at `102A94` in its later vector calculation, despite the heading calculation reading only X/Z. The resulting value reaches `233CA8..233CE8` before `233CF0/233CF4` force multiplier58 to 1. That omitted native calculation and its source-FPU domain require separate work; this change does not claim a complete spatial-domain repair. The known possibility of a channel 1 or 2 scalar rejection remains explicit.

## Verified build and focused regression

The integrated full 354-source application build and link passed. Binary SHA256: `40ccd9db1c6c8c0ccb8cda661308f2e5ce2faaed75c26e09052f0b336345ad21`.

The standalone `tests/native/original_material_stop_lazy50ln_check.cpp` is the byte-identical checker used for 1485 passing assertions. It was compiled with C++20/O2, no fast math, rounding math and FP contraction disabled, then linked to 353 actual non-main application objects. The checker recipe verified all 354 object receipts and 1240 current dependencies, including the rebuilt main, and excluded the gameplay entry point.

Coverage includes the exact captured and signed subnormal values, genuine cold LogicalAudioOwner stops for two mixer voices, bitwise retention of 17 numerical lanes, preserved channel 1 and 2 rejection, structural guard failures and a partially successful stop followed by an invalid-row failure. Its explicit two-sample test PCM is a cold unit fixture, not a runtime asset or fallback. The historical diagnostic-only checker is superseded by this combined regression because channel 3 numeric rejection is intentionally no longer expected.

## Bounded runtime result

Ody Mandrell / MA (Orotoru G'am) completed the normal-user flow on this binary: original Main → Rider → Course → real intro/countdown/GO → W throttle with displayed speed 300 → Pause → confirmed Restart rebuilding session 3 → real countdown/GO → W throttle with displayed speed 243 → Pause → confirmed return to original Main → Alt+F4 from the frontend sky. The run exited with code 0, without timeout, at 2026-10-18 05:52:54 UTC. The configured generated runtime root remained absent, no material failure was logged, and same-run selection and in-race proof images were retained and shared.

This is bounded driving, Pause, Restart and Main-return evidence for Ody/MA. It does not establish a genuine finish/Results flow, audible output, every-racer/every-course coverage, source-FPU parity or user-hardware performance. Failed Ben and Dud attempts remain failed evidence; this successful fresh pair does not retroactively turn them into passes. The compiled source tree also contained separate uncalled Course work, which is excluded from this publication. This is not an independently rebuilt isolated publication-tree certification.
