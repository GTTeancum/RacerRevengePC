#!/usr/bin/env python3
"""Serial48Q borrowed type3 context checks against unchanged original48P evidence.

All six template execution fields are poisoned after context setup. No runtime
filter clones, renderer, downloads, PC/RAM access, or install operations.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="type3-context48q"
runner.EXTRA_INPUT_PATHS=("tools/test_native_type3_context48q.py",)
runner.SOURCES=("src/game/spring_records48n.cpp","src/game/spring_callback48n.cpp",
    "src/game/spring_type3_callback48p.cpp","src/game/collision_type3_leaf48p.cpp",
    "src/game/collision_type3_node48p.cpp","src/game/type3_prepare48p.cpp","src/game/type3_query48p.cpp")
runner.INCLUDES=("src/game","src/assets","third_party/native-json")
runner.TESTS={"type3_context48q":{"source":"tests/native/type3_context48q.cpp",
    "fixture":"evidence/type3Query48P/native-fixtures.json"}}
if __name__=="__main__":sys.exit(runner.main())
