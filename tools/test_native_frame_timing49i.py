#!/usr/bin/env python3
"""49I original post-clock timing/state plus all retained camera/input checks."""
import sys
sys.dont_write_bytecode = True
import test_native_camera_view49f as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "frame-timing"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_frame_timing49i.py", "cmake/NativeFrameTiming49I.cmake",
    "tools/gameplay/frame_timing_recover49i.py",
    "tests/native/frame_timing_oracle49i.h",
    "evidence/frameTiming49I/clock-source-investigation.json",
    "evidence/frameTiming49I/clock-state-defaults.json",
    "evidence/frameTiming49I/virtual-dispatch-investigation.json",
)
runner.SOURCES = ("src/game/frame_timing49i.cpp",) + tuple(runner.SOURCES)
runner.TESTS = {"frame_timing49i": {"source":"tests/native/frame_timing49i.cpp", "fixture":None, "arguments":[]}, **runner.TESTS}
if __name__ == "__main__":
    sys.exit(runner.main())
