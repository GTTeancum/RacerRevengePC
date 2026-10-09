#!/usr/bin/env python3
"""49L original PodSphereConstraint and retained body/timing/input/camera checks."""
import sys
sys.dont_write_bytecode = True
import test_native_body_advancement49k as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "sphere-constraint"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_sphere_constraint49l.py", "cmake/NativeSphereConstraint49L.cmake",
    "tools/gameplay/sphere_bindings_provenance49l.py",
    "evidence/sphereConstraint49L/binding-source/manifest.json",
)
runner.SOURCES = ("src/game/pod_sphere_constraint49l.cpp",) + tuple(runner.SOURCES)
runner.TESTS = {
    "pod_sphere_constraint_api49l": {"source":"tests/native/pod_sphere_constraint_api49l.cpp", "fixture":None, "arguments":[]},
    "pod_sphere_constraint49l": {"source":"tests/native/pod_sphere_constraint49l.cpp", "fixture":None, "arguments":[]},
    **runner.TESTS,
}
if __name__ == "__main__":
    sys.exit(runner.main())
