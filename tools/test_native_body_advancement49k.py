#!/usr/bin/env python3
"""49K staged main-body advancement plus unchanged timing/input/camera/body checks."""
import sys
sys.dont_write_bytecode = True
import test_native_clock_quantization49j as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "body-advancement"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_body_advancement49k.py", "cmake/NativeBodyAdvancement49K.cmake",
)
# The restored historical test-only SQRT reference uses std::bit_cast.
# Production sources retain C++17-compatible interfaces and implementation.
runner.STRICT_FLAGS = ["-std=c++20" if flag == "-std=c++17" else flag
                       for flag in runner.STRICT_FLAGS]
runner.SOURCES = (
    "src/game/advancement_selectors49k.cpp", "src/game/body_advancement49k.cpp",
    "src/game/body_integration48c.cpp", "src/game/vehicle_forces48d.cpp",
    "src/game/vehicle_aero48e.cpp", "src/game/body_scene48g.cpp",
    "src/game/body_wrapper48h.cpp",
) + tuple(runner.SOURCES)
runner.TESTS = {
    "advancement_selectors49k": {"source":"tests/native/advancement_selectors49k.cpp", "fixture":None, "arguments":[]},
    "body_advancement49k": {"source":"tests/native/body_advancement49k.cpp", "fixture":None, "arguments":[]},
    "physics48c": {"source":"tests/native/physics48c.cpp", "fixture":None, "arguments":[]},
    "physics48d": {"source":"tests/native/physics48d.cpp", "fixture":None, "arguments":[]},
    "physics48e": {"source":"tests/native/physics48e.cpp", "fixture":None, "arguments":[]},
    "body_scene48g": {"source":"tests/native/body_scene48g.cpp", "fixture":None, "arguments":[]},
    "body_wrapper_nearest49k": {"source":"tests/native/body_wrapper_nearest49k.cpp", "fixture":None, "arguments":[]},
    **runner.TESTS,
}
if __name__ == "__main__":
    sys.exit(runner.main())
