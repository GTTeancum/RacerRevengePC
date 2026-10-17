# Initialize only the live coverage-region scratch prefix

UNTESTED. Independently source-reviewed against coverage blob
8f3dbafea9404ed38d55cc6ce0e36bc4d3e563c4. No builds, syntax checks, tests,
benchmarks or gameplay. No measured FPS benefit claimed.

The local 14-row std::array of four-int rectangles no longer uses {} value
initialization. This removes requested initialization of the unused suffix;
it does not introduce a heap allocation or retain data across calls.

Definite-assignment invariant: regions[0] receives the full initial rectangle
before run=1. The only reads iterate i<run. Each appended row is assigned in
full by regions[run++]=next, reached only under run<maximumRun. Thus every read
addresses the fully initialized prefix. Empty triangles continue before array
creation; full-region singletons skip lookahead with row0 initialized. Every
lookahead break leaves the prior initialized prefix unchanged. Failure paths
do not inspect the unused suffix. The array never escapes or is copied wholesale.

All region arithmetic, conservative rejection, EBO ordering, grouping thresholds,
GPU calls, diagnostics and accounting remain unchanged. The source no longer
requests zeroing 56 int32 values (224 bytes) for every nonempty run. The compiler
may already eliminate some or all dead stores; actual benefit is unmeasured.
