# Type-3 triangle leaf, 48P

This component translates one triangle and one local ray pair in original
`10E080`. It is distinct from type-2 `10B840`. It owns no mesh traversal, target
transforms, callback, registration, world scheduling, saved live binding or game
installation. The production function is allocation-free and uses fixed stack
storage. The caller owns all inputs and the two result-state objects.

## Source and supported contract

`applyType3Triangle48P` takes three float vertices, optional float vertex
normals, a normal mode, local start/direction and a scalar length, plus retained
per-pair and globally selected state. Local direction need not be normalized.
The leaf computes its geometric normal from the ordered cross product of the
vertices. Packed triangle plane/normal fields are not read here; they belong to
traversal decisions elsewhere.

The source instructions are frozen against authorized ELF SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
The immutable blocks and pins are recorded in `evidence/type3Leaf48P` and the
reproducer. The [dispatch map](type3-dispatch-map48p.md) documents register and
structure offsets and the geometric instruction anchors.

- Geometric hit requires a negative direction/face dot product, positive
  length, inclusive fraction `[0,1]`, and the six ordered projected scanline
  branches. An endpoint fraction of exactly one is a geometric hit.
- Pair replacement is strictly `new < prior`, or `prior < 0`. An equal hit keeps
  the previous pair normal, point and fraction.
- Normal interpolation occurs only on a pair replacement. A farther hit whose
  unused interpolation would cross a missing-edge boundary remains a hit.
- Every geometric hit writes the pair hit flag to one and increments the global
  hit count as an unsigned word, including repeated hits and non-improvements.
- Global replacement is strictly `retainedPairFraction < selectedFraction`.
  This comparison uses the retained pair value, even if this triangle did not
  replace it. Global ties preserve the previous selected fields.
- A new correction is `dot((1-fraction)*localDirection, pairNormal)*pairNormal`.
  There is no length multiplier. It uses the retained/interpolated pair normal.
- Flat local normal W is zero; mode-one interpolated local normal W is one.
  New point and correction W are one. Unchanged fields retain every bit.
  World-space W handling belongs to the dispatcher.

No vertex normals, or mode zero, selects the computed face. With a normal array,
mode one executes the ordered two-edge interpolation represented by original
`1106B0`. Other nonzero normal modes invoke distinct original helper `10F580`
and remain explicitly unsupported when an improving hit would use that helper.
Their miss/non-improving paths do not require the unsupported interpolation.
Quantized vertex/normal arrays are outside this native boundary.

The numerical proof models individually rounded ordinary IEEE binary32
operations, with no fused multiply-add. It does not establish every physical
EE/VU exceptional mode. Nonfinite input XYZ, lengths and retained fractions are
rejected. Nonfinite normals are rejected only if used; W bits are opaque when
copied. Overflow/nonfinite intermediate outcomes are outside the supported
native domain.

## Missing interpolation edges

The original mode-one helper reads two edge slots unconditionally. A cut through
an extreme vertex can leave one of those pointer slots unwritten. This is
caller-stack-dependent original behavior, not permission to fill the slot with
zero or return a fabricated normal. The native function returns
`UnsupportedInterpolationBoundary` and leaves both states unchanged. Its
zero-denominator/nonfinite interpolation outcomes are also explicit unsupported
boundaries. All other errors and misses are transactional as well.

## Independent instruction evidence

Run from the repository root:

`python3 tools/gameplay/type3_leaf_fixtures48p.py`

The generator executes unchanged original instructions starting at `10E080`,
with a single ray pair and a leaf-tagged triangle. It runs actual constructors,
vertex fetch, dot, normalization, vector scale and interpolation helpers; it
never calls the type-two oracle or a native implementation for expected values.
The normal completion exits through `10F570` to the interpreter return sentinel.
Missing-edge cases stop at the actual uninitialized pointer load and record the
precise PC/address. They receive no fallback memory and no fabricated output.
Native transactional state is the contract for these unsupported cases.

Every data byte read is supplied by the small explicitly initialized input/result
arrays or written by the executing original instructions. The backing data is
`b''`, source reads are zero, and an audit verifies that triangle plane/packed
normal bytes were not read. No RAM image, patched instruction, intercepted helper
or native expected value is used.

The corpus has 904 cases: 599 complete geometric hits, 290 misses and
15 missing-edge stops, over 525,757 original instructions. Cases cover vertex
permutations and projection axes, interior/edge/vertex cuts, front/back/parallel
rays, endpoints and out-of-range fractions, degenerate triangles, absent and
mode-zero/mode-one normals, random finite 3D geometry, strict retained fraction
ties, negative priors, stale pair selection, opaque fourth lanes and wrapping
hit counts. Additional cases execute non-unit local directions and normal-array
mode-two/UINT_MAX paths that miss or retain an equal/nearer pair, proving those
paths do not call the unsupported interpolation helper.

The native test reads the frozen binary corpus and compares status plus all
24 mutable state words bit-for-bit. It additionally checks transactional
unsupported mode and invalid input handling. Final optimized and ASan/UBSan runs each pass 22,604 checks across all 904
original fixtures, plus 100,000 serial calls with zero allocations. Both builds
are warning-free under `-Wall -Wextra -Wpedantic -ffp-contract=off`. LeakSanitizer
itself cannot run under this ptrace environment; its failure is retained in
`evidence/type3Leaf48P/validation/sanitized.txt`, and ASan/UBSan ran with
`detect_leaks=0`.

Seven deliberate source mutations all fail the corpus: replacing strict pair
or selected ties, adding a correction length factor, changing either flat or
smooth normal W, dropping geometric hit count, and reassociating the point
multiplications. These are separate temporary mutant builds; the production
source remained unchanged. Logs, source hashes and mutation outcomes are under
`evidence/type3Leaf48P/validation`.
