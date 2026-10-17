# Reuse checked source-pair depth comparison

UNTESTED. Two independent source reviews against bf22795. No builds, tests,
syntax checks, benchmarks or gameplay. No measured performance gain claimed.

Remove only composition's repeated gl.depthFunc(GL_LEQUAL) in
native_coverage_storage.inl::drawCoveragePairTriangle. Its sole production caller
is drawCoveragePairGroup, whose sole caller is drawSpecularSpan50LA. Each span
explicitly sets LEQUAL before its first checked base setup. Base draw and map2
setup checks must also succeed before reaching the bridge.

No successful intervening operation changes the stored depth comparison function.
Depth-test enable toggles, depthMask, framebuffer/program/VAO/texture/sample state
and CPU rectangle lookahead do not reset it. Empty/all-empty groups issue no
bridge operations and preserve the established function. Later base groups change
mask, textures and uniforms, not comparison. Payload still disables depth testing;
composition still enables it and uses the original sampled gl_FragDepth.

The rejected-tail depthFunc(GL_LEQUAL), all error gates and every restoration
remain. A failed setup cannot reach composition through a successful setup check;
a failed bridge aborts before group reuse. This removes one redundant native API
call per bridge reaching composition, without changing geometry, draw count,
snapshots or shader math. It does not resolve the dominant per-triangle bridge
architecture by itself; actual benefit remains unknown.
