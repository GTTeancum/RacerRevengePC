#!/usr/bin/env python3
"""49J supplied-sample original clock plus connected and retained timing/input/camera checks."""
import sys
sys.dont_write_bytecode = True
import test_native_frame_timing49i as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "clock-quantization"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_clock_quantization49j.py", "cmake/NativeClockQuantization49J.cmake",
    "evidence/clockQuantization49J/conversion-source-investigation.json",
    "evidence/clockQuantization49J/sample-source-investigation.json",
)
runner.SOURCES = ("src/game/clock_quantization49j.cpp",) + tuple(runner.SOURCES)
runner.TESTS = {
    "clock_quantization49j": {"source":"tests/native/clock_quantization49j.cpp", "fixture":None, "arguments":[]},
    "clock_timing_diagnostics49j": {"source":"tests/native/clock_timing_diagnostics49j.cpp", "fixture":"evidence/tvCamera49E/authored/taTVCam.pos", "binary":True, "arguments":["evidence/tvCamera49E/authored/taTVCam.pos"]},
    **runner.TESTS,
}
if __name__ == "__main__":
    sys.exit(runner.main())
