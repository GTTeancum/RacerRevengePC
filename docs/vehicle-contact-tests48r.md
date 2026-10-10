# 48R native vehicle-contact checks

## Test boundary and oracle

`tests/native/vehicle_contact48r.cpp` calls only the public bounded
`runVehicleContacts48R` API. Fixtures are produced by independently executing
frozen original219420 and the reached original helpers, including real165430.
The test does not generate expected arithmetic by calling native helpers.
`expected.residual_v0` is deliberately not consumed as a semantic result: the
original handler does not define a Boolean return.

On Complete, the test checks exact binary32/raw words for all sixteen world and
local matrix lanes, scene flags and parent presence, body identity and callback
metadata, all ten body-list slots including inactive tails, all eleven spring
state words, both raw static words/flags, and every byte of every96-byte record
slot. It snapshots all source arrays and all input/buffer descriptors to detect
input mutation. Entire inactive source slots are projected; destination spare
capacity and outer canaries remain unchanged.

Stopped original-prefix fixtures are classified by their actual reached
boundary; native output must remain completely unchanged. The original prefix
is evidence, not a partial native commit. Negative reached spring capacities
are separately classified as native guard divergences: the malformed original
can fall through its signed saturation gate, while the existing48N append seam
requires nonnegative metadata. Three zero-lateral cases exercise the inherited
finite VRSQRT model's max-finite reciprocal followed by multiplication. Those
model-specific probes are retained and labelled; they do not establish physical
EE/VU exceptional-number equivalence. Nonfinite copy-only lanes are supported;
reached nonfinite arithmetic is independently rejected transactionally.

## Adversarial native checks

- Full declared mutable ranges, including unused tail capacity, must be disjoint
  from each other, the result, every source array, and every descriptor. The
  harness covers all mutable pairs and all mutable/read combinations at both
  beginning and final aligned word. Inline body slots are included in those
  complete body spans. Read-only ranges may overlap.
- Null positive spans, misalignment, arithmetic-size overflow, address wrapping,
  insufficient capacities, invalid scene/body/candidate indices, and duplicate
  identity labels are tested before any unsafe storage access.
- Late unknown bounds/callbacks, bad body counts, local dirty-parent scenes,
  and nonfinite arithmetic preserve the entire output transaction, including
  earlier staged body/scene changes. Scratch is unspecified on rejection, but
  outer scratch canaries may not be touched.
- Both callbacks still execute when spring records saturate. Body duplicates,
  full and signed-overfull lists, shared/self bodies, supplied candidate order,
  and preexisting spring counts are preserved. Neither contact list nor spring
  count is reset inside219420.
- Zero identity is allowed by the standalone165430 helper but is the
  absent/static sentinel in spring records. The full handler rejects reached
  zero primary/other identities; skipped and missed zero labels remain lazy.
- Nonpositive signed candidate counts still prepare the position scene and
  initialize both statics, without touching latent bounds, body, local scene,
  radius or spring fields. Category gates and geometric misses retain their
  separate lazy boundaries.
- Cached candidate radius, opaque vector W lanes and the second static raw word
  are never interpreted as consumed numeric inputs. Initialized first static
  values other than0.5 remain meaningful.
- Position and inverse-point scene bindings are independently supplied. Shared
  scene indices coalesce; prepared caches ignore opaque parent presence.

## Instrumentation and reproducible runs

`tools/test_native_vehicle_contact48r.py` builds and runs serially. It records
compiler flags, source and fixture hashes, commands, return codes, diagnostics,
and optional per-function `.su` estimates. The suite uses strict binary32
rounding, disables contraction and fast math, and sets FE_TONEAREST.

```
python3 tools/test_native_vehicle_contact48r.py --output-dir /tmp/rr48r-release --tests vehicle_contact48r --stack-usage
python3 tools/test_native_vehicle_contact48r.py --output-dir /tmp/rr48r-asan --tests vehicle_contact48r --sanitizers --no-leak-check --stack-usage
python3 tools/test_vehicle_contact_mutations48r.py --output-dir /tmp/rr48r-mutations
```

The fixture parser, storage setup, snapshots and diagnostics allocate only
outside watched API calls. Ordinary, array and aligned C++ allocations are
instrumented around the public API. A bounded17500-call serial reuse sample
reports median and maximum batch-mean host nanoseconds, caller backing bytes,
input/result/descriptor sizes, and allocation count. This is not an Xbox frame
rate, whole-game frame time or hardware memory-budget validation.

The final338-case corpus passes both release and ASan/UBSan runs with425478
checks:314 exact Complete projections,14 stopped-original boundaries,10 native
guard divergences,305 rejection cases including168 aliases, and19 synthetic
wiring cases. All754 original callback invocations and377 reached append attempts
in the Complete group are represented in the complete output comparisons. The
forty added up-axis cases cover twenty hits and twenty misses with tilted,
nonunit, mixed-sign and cancellation-sensitive axes, prepared/dirty-parentless
caches, and shared/separate scene bindings. The initial298 case objects remain
unchanged in the proof archive.

No watched allocations or compiler warnings occurred. Both runners report no
changed inputs. LeakSanitizer was explicitly disabled; no leak-check claim is
made. These are focused headless runner results; CMake configuration and a full
PC/rendering/runtime integration were not executed as part of this test lane.

Retained reports and exact logs are under
`evidence/vehicleContact48R/native-validation/release338/`, `sanitized338/`, and
`mutations338/`, with SHA-256 checksums in `native-validation/manifest.json`.
The release host timing sample has median477.934ns and maximum batch mean
495.729ns; the sanitizer sample has median3585.68ns. These numbers describe this
bounded repeated dataset on this host, including the transactional copy/guards;
they are not transferable console performance estimates.

For the three-scene, three-body, four-candidate, twelve-record timing dataset,
declared scratch and destination backing each occupy2284 bytes, including two
spare slots per array. Input/result/source/buffer descriptors are80/56/80/48
bytes on the checked x86-64 host. GCC's optimized handler frame estimate is864
bytes; helper estimates are additional, and `.su` is not a measured maximum
live stack. Earlier48P/48Q sources and fixture proofs remain unchanged.

## Source-sensitive controls

`tools/test_vehicle_contact_mutations48r.py` builds temporary altered source
copies outside the checkout and reruns the same fixture/guard executable. The
baseline must pass; a mutation is killed only by normal exit1 assertion failure,
not a compiler error, timeout, or crash. Source and fixture hashes must be
unchanged afterward. Controls target cached radius/center substitution, overlap
ties, reciprocal-vs-lane division, static scale, body/spring count resets,
callbacks skipped on saturation, reordered candidates, conflated scene bindings,
an invented zero normal, stale record clearing, dot-product reassociation,
normalized up axes and a Y-only projection. All fifteen mutated copies were
killed by normal assertion exit1, while the unmodified source control passed.
The original source/test/fixture hashes stayed unchanged. Mutation logs retain
the concrete first failing fixture/assertions, not just a count of killed tests.

## Validation history

An early test-only address-wrap probe used an order pointer at UINTPTR_MAX-15
with an eight-byte range; it did not actually wrap and was outside caller-backed
storage. That harness error faulted the first trial, was changed to an aligned
UINTPTR_MAX-3 wrapping range, and is not counted as a runtime validation result.
A subsequent baseline reported three assertions from an outdated synthetic
zero-identity success expectation during the deliberate full-handler contract
clarification. The corrected reached-rejection/lazy-acceptance tests passed.
