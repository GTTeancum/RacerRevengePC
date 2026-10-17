# Uniform guard for generic normal preparation

UNTESTED. Source-only independent design and implementation review accepted shader blob bc5e18352f7beaa80a24b5ad98955d652b224d20 against base dd273724ccbf9e3c0c7c1ed2514f650d74f5ca3b. No builds, syntax checks, tests, benchmarks, or gameplay accompanied this change.

## Narrow change

The generic fragment path keeps its initialized outer normal and computes its derivatives, cross product, lengths, authored normalization, and fallback facing adjustment only when the existing uniform useLighting is nonzero. The block remains before the existing suppressDraw discard. All lit arithmetic expressions, dependencies, and arithmetic ordering are retained; the independent constant normal initializer moves before the guarded calculations.

The only later consumers of normal are inside the existing useLighting branch: selected-light dot products and the diagnostic hemisphere/directional-light calculation. With useLighting zero, the removed computations cannot affect color, alpha, depth, or discard. With it nonzero, those computations and consumers remain. No vertex inputs/outputs, interpolation qualifiers, fragment outputs, uniform declarations, texture operations, material decisions, GL checks, or existing shader variants change.

## Primary specification

The [official GLSL 1.50 specification](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.1.50.pdf) supports this control-flow reasoning:

- Section 4.3.5, printed page 32: uniform variables have the same value across a primitive and are read-only to the shader.
- Section 8.8, printed pages 101-102: derivatives are undefined in nonuniform control flow; their implementation may use approximations.
- Section 6.4, printed page 68: nonuniform discard can make subsequent derivatives undefined.

Here useLighting != 0 compares a uniform integer with a constant, so fragments of a primitive take the same added branch. No varying-derived condition encloses the derivative calls. Keeping the block before the existing generic discard preserves the prior ordering. Existing source-color paths return before this block.

## Scope and limits

This is low-priority generic-only work. Every source-diffuse, source-pair base/map-2, and coverage-payload path already returns above the normal block and is unchanged. It does not reduce coverage bridge count or the dominant source-pair submission workload.

The driver compiler may already sink or eliminate output-independent calculations. No instruction-count, GPU-time, or FPS improvement is established. This is a source-semantic preservation argument, not a guarantee of bit-identical derivatives or floating-point output across separately compiled shaders; the specification permits implementation-dependent derivative approximations. Runtime validation remains outstanding.
