#!/usr/bin/env python3
"""48W original input-to-engine/aero binding plus complete retained input regressions."""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="control-binding"
runner.EXTRA_INPUT_PATHS=("tools/test_native_control_binding48w.py","cmake/NativeControlBinding48W.cmake","evidence/controlBinding48W/recovery-manifest.json","evidence/controlBinding48W/provenance-blocks.json","tools/gameplay/control_binding_recover48w.py")
runner.SOURCES=("src/game/control_filter48b.cpp","src/game/control_shaping48v.cpp","src/game/control_binding48w.cpp")
runner.INCLUDES=("src/game",)
runner.TESTS={
 "control_binding48w":{"source":"tests/native/control_binding48w.cpp","fixture":None},
 "control_shaping48v":{"source":"tests/native/control_shaping48v.cpp","fixture":None},
 "control_filter48b":{"source":"tests/native/control_filter48b.cpp","fixture":None},
}
if __name__=="__main__":sys.exit(runner.main())
