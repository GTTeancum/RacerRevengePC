#!/usr/bin/env python3
"""49D native control presets with all prior native input regressions."""
import sys
sys.dont_write_bytecode = True
import test_native_spring48k as runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "control-presets"
runner.EXTRA_INPUT_PATHS = (
    "tools/test_native_control_presets49d.py", "cmake/NativeControlPresets49D.cmake",
    "evidence/controlPresets49D/recovery-manifest.json",
    "evidence/controlPresets49D/original-disassembly.txt",
    "tools/gameplay/control_presets_recover49d.py",
)
runner.SOURCES = (
    "src/game/control_filter48b.cpp", "src/game/control_shaping48v.cpp",
    "src/game/control_binding48w.cpp", "src/game/player_input48x.cpp",
    "src/game/device_held48y.cpp", "src/game/event_dispatch48z.cpp",
    "src/game/player_events49a.cpp", "src/game/control_presets49d.cpp",
)
runner.INCLUDES = ("src/game",)
runner.TESTS = {
    "control_presets49d": {"source": "tests/native/control_presets49d.cpp", "fixture": None},
    "player_events49a": {"source": "tests/native/player_events49a.cpp", "fixture": None},
    "event_dispatch48z": {"source": "tests/native/event_dispatch48z.cpp", "fixture": None},
    "device_held48y": {"source": "tests/native/device_held48y.cpp", "fixture": None},
    "player_input48x": {"source": "tests/native/player_input48x.cpp", "fixture": None},
    "control_binding48w": {"source": "tests/native/control_binding48w.cpp", "fixture": None},
    "control_shaping48v": {"source": "tests/native/control_shaping48v.cpp", "fixture": None},
    "control_filter48b": {"source": "tests/native/control_filter48b.cpp", "fixture": None},
}
if __name__ == "__main__":
    sys.exit(runner.main())
