#!/usr/bin/env python3
"""49M original secondary wrapper and persistent two-body/input/timing checks."""
import sys
sys.dont_write_bytecode = True
import test_native_sphere_constraint49l as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "secondary-wrapper"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_secondary_wrapper49m.py", "cmake/NativeSecondaryWrapper49M.cmake",
)
runner.SOURCES = (
    "src/game/gradient_noise49m.cpp", "src/game/secondary_body49m.cpp",
    "src/game/coupled_body_advancement49m.cpp",
) + tuple(runner.SOURCES)
runner.TESTS = {
    "gradient_noise49m": {"source":"tests/native/gradient_noise49m.cpp", "fixture":None, "arguments":[]},
    "secondary_body_api49m": {"source":"tests/native/secondary_body_api49m.cpp", "fixture":None, "arguments":[]},
    "secondary_wrapper49m": {"source":"tests/native/secondary_wrapper49m.cpp", "fixture":None, "arguments":[]},
    "coupled_body_advancement49m": {"source":"tests/native/coupled_body_advancement49m.cpp", "fixture":None, "arguments":[]},
    **runner.TESTS,
}
if __name__ == "__main__":
    sys.exit(runner.main())
