#!/usr/bin/env python3
"""49O authentic model motion/bounds/secondary geometry and retained regressions."""
import sys
from pathlib import Path
sys.dont_write_bytecode = True
import test_native_authored_initialization49n as prior
runner = prior.runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "authored-geometry"
root = Path(__file__).resolve().parents[1]
fixture_dir = "evidence/authoredGeometry49O/asset-reader/fixtures"
motion_dir = "evidence/authoredGeometry49O/motion"
runner.EXTRA_INPUT_PATHS = tuple(runner.EXTRA_INPUT_PATHS) + (
    "tools/test_native_authored_geometry49o.py", "cmake/NativeAuthoredGeometry49O.cmake",
    motion_dir + "/fixture.bin", motion_dir + "/trig/sin-table.bin",
    motion_dir + "/trig/cos-table.bin", motion_dir + "/trig/acos-table.bin",
) + tuple(p.relative_to(root).as_posix() for p in sorted((root/fixture_dir).iterdir()) if p.is_file())
runner.SOURCES = (
    "src/assets/model_geometry49o.cpp", "src/assets/model_bindings49o.cpp",
    "src/game/model_motion49o.cpp", "src/game/model_bounds49o.cpp",
    "src/game/authored_geometry49o.cpp", "src/game/pod_model_geometry49o.cpp",
) + tuple(runner.SOURCES)
runner.TESTS = {
    "model_geometry49o": {"source":"tests/native/model_geometry49o.cpp", "fixture":None, "arguments":[fixture_dir]},
    "model_bindings49o": {"source":"tests/native/model_bindings49o.cpp", "fixture":None, "arguments":[]},
    "model_motion49o": {"source":"tests/native/model_motion49o.cpp", "fixture":None, "arguments":[motion_dir]},
    "model_bounds49o": {"source":"tests/native/model_bounds49o.cpp", "fixture":fixture_dir+"/52-chariot.psg", "binary":True, "arguments":[fixture_dir+"/52-chariot.psg"]},
    "authored_geometry49o": {"source":"tests/native/authored_geometry49o.cpp", "fixture":None, "arguments":[]},
    "pod_model_geometry49o": {"source":"tests/native/pod_model_geometry49o.cpp", "fixture":None, "arguments":["evidence/authoredGeometry49O"]},
    "authored_body_diagnostic49o": {"source":"tests/native/authored_body_diagnostic49o.cpp", "fixture":prior.fixture, "binary":True, "arguments":[prior.fixture,fixture_dir]},
    **runner.TESTS,
}
if __name__ == "__main__":
    sys.exit(runner.main())
