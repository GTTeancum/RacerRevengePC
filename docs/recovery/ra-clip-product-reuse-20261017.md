# RA clip-plane product reuse

UNTESTED. Base60a7a5d. No builds, syntax checks, tests, benchmarks or gameplay.

raTriangleOutsideNativeClip previously evaluates two double products per term
for each of six planes:48 products per complete vertex. Only16 operand pairs
are distinct. The new fixed4x4 local array retains those products and evaluates
each on its first original use: W and X at plane0, Y at plane2, Z at plane4.
This lazy order preserves early nonfinite rejection without touching axes the
original path would not reach. Every referenced array element has been written
by its first plane before reuse; no data survives the current vertex.

The exact original k-order plane accumulation remains:
value += w + sign*a; scale += abs(w) + abs(a).
The sign multiply, finite checks, margin expression, outside bits, common-plane
AND and early return are unchanged. In particular this does not substitute
the differently-associated nativeRasterVertex clip-coordinate calculation.

## Floating-point scope

The matrix entries and positions are float values widened to double (the fourth
position is exactly1). For finite IEEE binary32 operands, the product needs at
most48 significand bits and its exponent range fits binary64, including the
product of two binary32 subnormals. Thus these individual finite products are
exact binary64 values. Accumulations retain their previous explicit order and
rounding; no FMA or reassociation is introduced. Signed zero is retained in the
stored product. Nonfinite inputs retain the existing per-plane false fallback;
first-use computation avoids evaluating future-axis products after rejection.

platform/linux/native/CMakeLists.txt supplies GNU/Clang -fno-fast-math,
-frounding-math and -ffp-contract=off as PUBLIC options on
rr_native_gameplay50dj. The interactive/headless renderer libraries link that
target and compile vehicle_window_gl50ds.cpp under those inherited options.
This source review does not certify unsupported fast-math builds or inspect
compiler output. It does not establish a new bound on GPU GLSL arithmetic.

## Performance limit

The source-level multiplication count falls from48 to16 for a fully reached
vertex, with at most128 bytes of stack products. First-use branches and loads
add overhead, and the compiler may already remove repeated expressions.
No actual speedup is claimed; this outgoing session performed no measurement.
No shader, culling tolerance, material, draw order, asset or AI changes occur.
