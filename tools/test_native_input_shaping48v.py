#!/usr/bin/env python3
"""48V exact original input shaping, quantization and corrected rate-limit composition."""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="input-shaping"
runner.EXTRA_INPUT_PATHS=("tools/test_native_input_shaping48v.py","cmake/NativeInputShaping48V.cmake","evidence/inputShaping48V/recovery-manifest.json")
runner.SOURCES=("src/game/control_filter48b.cpp","src/game/control_shaping48v.cpp")
runner.INCLUDES=("src/game",)
runner.TESTS={
    "control_shaping48v":{"source":"tests/native/control_shaping48v.cpp","fixture":None},
    "control_filter48b":{"source":"tests/native/control_filter48b.cpp","fixture":None},
}
if __name__=="__main__":sys.exit(runner.main())
