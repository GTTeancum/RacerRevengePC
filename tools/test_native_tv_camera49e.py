#!/usr/bin/env python3
"""49E native authored TV-camera loading and selection with all prior native input regressions."""
import sys
sys.dont_write_bytecode = True
import test_native_spring48k as runner
runner.__doc__ = __doc__
runner.SUITE_NAME = "tv-camera"
runner.EXTRA_INPUT_PATHS = (
    "tools/test_native_tv_camera49e.py", "cmake/NativeTvCamera49E.cmake",
    "evidence/tvCamera49E/asset-provenance.json",
    "tools/gameplay/tv_camera_recover49e.py",
)
runner.SOURCES = (
    "src/assets/tv_camera49e.cpp", "src/game/tv_camera_selection49e.cpp",
    "src/game/control_filter48b.cpp", "src/game/control_shaping48v.cpp",
    "src/game/control_binding48w.cpp", "src/game/player_input48x.cpp",
    "src/game/device_held48y.cpp", "src/game/event_dispatch48z.cpp",
    "src/game/player_events49a.cpp", "src/game/control_presets49d.cpp",
)
runner.INCLUDES = ("src/assets", "src/game",)
runner.TESTS = {
    "tv_camera49e": {"source": "tests/native/tv_camera49e.cpp", "fixture": "evidence/tvCamera49E/authored/taTVCam.pos", "binary": True, "arguments": ["evidence/tvCamera49E/authored/taTVCam.pos"]},
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
