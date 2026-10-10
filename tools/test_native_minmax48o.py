#!/usr/bin/env python3
"""48O corrective scalar MIN/MAX regression, including historical control filter."""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="scalar-minmax"
runner.EXTRA_INPUT_PATHS=("tools/test_native_minmax48o.py","cmake/NativeMinMax48O.cmake")
runner.SOURCES=("src/game/control_filter48b.cpp","src/game/spring_contact48j.cpp","src/game/spring_force48l.cpp")
runner.INCLUDES=("src/game","third_party/native-json")
runner.TESTS={
    "control_filter48b":{"source":"tests/native/control_filter48b.cpp","fixture":None},
    "scalar_minmax48o":{"source":"tests/native/scalar_minmax48o.cpp","fixture":"evidence/minmaxAudit48O/edge-fixtures.json"},
}
if __name__=="__main__":sys.exit(runner.main())
