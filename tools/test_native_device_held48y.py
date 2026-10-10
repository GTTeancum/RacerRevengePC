#!/usr/bin/env python3
"""48Y held-state queries and ready-packet transition production with input regressions."""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="device-held"
runner.EXTRA_INPUT_PATHS=("tools/test_native_device_held48y.py","cmake/NativeDeviceHeld48Y.cmake","evidence/deviceHeld48Y/recovery-manifest.json","evidence/deviceHeld48Y/provenance-blocks.json","tools/gameplay/device_held_recover48y.py","tools/gameplay/device_held_provenance48y.py")
runner.SOURCES=("src/game/control_filter48b.cpp","src/game/control_shaping48v.cpp","src/game/control_binding48w.cpp","src/game/player_input48x.cpp","src/game/device_held48y.cpp")
runner.INCLUDES=("src/game",)
runner.TESTS={
 "device_held48y":{"source":"tests/native/device_held48y.cpp","fixture":None},
 "player_input48x":{"source":"tests/native/player_input48x.cpp","fixture":None},
 "control_binding48w":{"source":"tests/native/control_binding48w.cpp","fixture":None},
 "control_shaping48v":{"source":"tests/native/control_shaping48v.cpp","fixture":None},
 "control_filter48b":{"source":"tests/native/control_filter48b.cpp","fixture":None},
}
if __name__=="__main__":sys.exit(runner.main())
