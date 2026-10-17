# Reuse checked coverage payload texture state

UNTESTED. Independently source-reviewed. No builds, syntax checks, gameplay,
benchmarks or instrumentation. Base remote 7787eb22.

## Composition selector

Remove only composition-entry activeTexture(textureUnit0) from
native_coverage_storage.inl::drawCoveragePairTriangle. Unit0 is established before
the payload setup error gate. Intervening indexed drawing, error queries,
framebuffer binds/blits, depth/sample changes, program/uniform writes and VAO
binding do not change the active texture selector. There are no caller callbacks.
bindSampler uses an explicit unit argument and does not select a texture unit.

The payload texture bind therefore still targets unit0. All composition texture
and sampler binds, unit1/unit2 selections, error gates and tail transitions remain.
Earlier failures skip composition and retain the original 2-to-1-to-0 cleanup.
Empty regions return before both old and new setup. The RA bridge is unchanged.

## Same-group payload entry

The first nonempty run explicitly selects unit0 and binds image. Later nonempty
runs omit these two entry calls only after the previous bridge returns accepted
and restored. The unconditional tail selects unit0 and binds the same by-value
image, then checks errors. The group does not change image or make callbacks;
intervening clip lookahead and empty-triangle accounting are CPU-only. A failed
bridge aborts the group before firstRun advances. New groups start firstRun=true.
Empty groups perform neither old nor new setup.

These changes compose: unit0 is established before every payload setup gate and
remains selected until composition binds payload. All failure cleanup and six
per-run error gates remain. No persistent state cache is introduced.

One native API call is removed when composition is reached, plus two per later
nonempty run in the same group. Actual FPS impact is unmeasured. Both changes
received independent source-only review.

## Recovery record

The VM became unavailable after local commits 58773bb and 08fb631. The source was
recovered from verified remote 7787eb22 with the two exact retained replacements.
Recovered blob ed63fa7b8c73e7939670cfb9b47e9210838fa6cc matches the retained local
diff prefix ed63fa7. This document is reconstructed from the source-review record.
