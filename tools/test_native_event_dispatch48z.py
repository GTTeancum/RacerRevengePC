#!/usr/bin/env python3
"""48Z supplied-event manager/child dispatch with all prior native input regressions."""
import sys
sys.dont_write_bytecode = True
import test_native_spring48k as runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "event-dispatch"
runner.EXTRA_INPUT_PATHS = (
    "tools/test_native_event_dispatch48z.py", "cmake/NativeEventDispatch48Z.cmake",
    "evidence/eventDispatch48Z/recovery-manifest.json",
    "evidence/eventDispatch48Z/original-disassembly.txt",
    "tools/gameplay/event_dispatch_recover48z.py",
)
runner.SOURCES = (
    "src/game/control_filter48b.cpp", "src/game/control_shaping48v.cpp",
    "src/game/control_binding48w.cpp", "src/game/player_input48x.cpp",
    "src/game/device_held48y.cpp", "src/game/event_dispatch48z.cpp",
)
runner.INCLUDES = ("src/game",)
runner.TESTS = {
    "event_dispatch48z": {"source": "tests/native/event_dispatch48z.cpp", "fixture": None},
    "device_held48y": {"source": "tests/native/device_held48y.cpp", "fixture": None},
    "player_input48x": {"source": "tests/native/player_input48x.cpp", "fixture": None},
    "control_binding48w": {"source": "tests/native/control_binding48w.cpp", "fixture": None},
    "control_shaping48v": {"source": "tests/native/control_shaping48v.cpp", "fixture": None},
    "control_filter48b": {"source": "tests/native/control_filter48b.cpp", "fixture": None},
}
if __name__ == "__main__":
    sys.exit(runner.main())
