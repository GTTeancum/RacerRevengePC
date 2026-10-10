#!/usr/bin/env python3
"""49A native player-event sequencing with all prior native input regressions."""
import sys
sys.dont_write_bytecode = True
import test_native_spring48k as runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "player-events"
runner.EXTRA_INPUT_PATHS = (
    "tools/test_native_player_events49a.py", "cmake/NativePlayerEvents49A.cmake",
    "evidence/playerEvent49A/recovery-manifest.json",
    "evidence/playerEvent49A/original-disassembly.txt",
    "tools/gameplay/player_event_recover49a.py",
)
runner.SOURCES = (
    "src/game/control_filter48b.cpp", "src/game/control_shaping48v.cpp",
    "src/game/control_binding48w.cpp", "src/game/player_input48x.cpp",
    "src/game/device_held48y.cpp", "src/game/event_dispatch48z.cpp",
    "src/game/player_events49a.cpp",
)
runner.INCLUDES = ("src/game",)
runner.TESTS = {
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
