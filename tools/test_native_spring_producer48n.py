#!/usr/bin/env python3
"""Serial48N static spring producer checks, no downloads or CMake required.

Original-derived synthetic/frozen evidence only. This does not register a live
world or schedule vehicles. Runtime is bounded and allocation-free; JSON fixture
loading, compiler and test diagnostics are host-only tooling.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="spring-producer"
runner.EXTRA_INPUT_PATHS=("tools/test_native_spring_producer48n.py","cmake/NativeSpringProducer48N.cmake")
runner.SOURCES=(
    "src/game/collision_query48h.cpp","src/game/collision_tree_workspace48m.cpp",
    "src/game/spring_records48n.cpp","src/game/spring_callback48n.cpp","src/game/spring_query48n.cpp",
)
runner.INCLUDES=("src/game","src/assets","third_party/native-json")
runner.TESTS={
    "spring_records48n":{"source":"tests/native/spring_records48n.cpp","fixture":"evidence/springRecords48N/native-fixtures.json"},
    "spring_callback48n":{"source":"tests/native/spring_callback48n.cpp","fixture":"evidence/springCallback48N/static-callback-fixtures.bin","binary":True},
    "spring_query48n":{"source":"tests/native/spring_query48n.cpp","fixture":"evidence/springQuery48N/native-fixtures.json"},
}
if __name__=="__main__":sys.exit(runner.main())
