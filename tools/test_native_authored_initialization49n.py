#!/usr/bin/env python3
"""49N authentic Pod01 parameters and connected body/input/timing checks."""
import sys
sys.dont_write_bytecode = True
import test_native_secondary_wrapper49m as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "authored-initialization"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_authored_initialization49n.py", "cmake/NativeAuthoredInitialization49N.cmake",
    "evidence/authoredInitialization49N/asset-provenance.json",
)
runner.SOURCES = (
    "src/assets/pod_physics49n.cpp", "src/game/authored_parameters49n.cpp",
) + tuple(runner.SOURCES)
fixture = "evidence/authoredInitialization49N/authoring-source/Pod01.phy"
runner.TESTS = {
    "pod_physics49n": {"source":"tests/native/pod_physics49n.cpp", "fixture":fixture, "binary":True, "arguments":[fixture]},
    "authored_parameters_api49n": {"source":"tests/native/authored_parameters_api49n.cpp", "fixture":None, "arguments":[]},
    "authored_parameters49n": {"source":"tests/native/authored_parameters49n.cpp", "fixture":None, "arguments":[]},
    "authored_body_diagnostic49n": {"source":"tests/native/authored_body_diagnostic49n.cpp", "fixture":fixture, "binary":True, "arguments":[fixture]},
    **runner.TESTS,
}
if __name__ == "__main__":
    sys.exit(runner.main())
