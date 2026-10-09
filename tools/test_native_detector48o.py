#!/usr/bin/env python3
"""Serial48O detector/query numeric preparation checks, no downloads or CMake.

Explicit original-instruction fixtures only. Type3 staging is distinct from the
48N type2 query. Neither live registration nor full detector scheduling is run.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="detector"
runner.EXTRA_INPUT_PATHS=("tools/test_native_detector48o.py","cmake/NativeDetector48O.cmake")
runner.SOURCES=("src/game/spring_records48n.cpp","src/game/query_prepare48o.cpp","src/game/spring_detector48o.cpp")
runner.INCLUDES=("src/game","third_party/native-json")
runner.TESTS={
    "query_prepare48o":{"source":"tests/native/query_prepare48o.cpp","fixture":"evidence/queryPrepare48O/fixtures.json"},
    "spring_detector48o":{"source":"tests/native/spring_detector48o.cpp","fixture":"evidence/springDetector48O/fixtures.json","extra_fixtures":[{"fixture":"evidence/springDetector48O/fixtures.bin","binary":True}]},
}
if __name__=="__main__":sys.exit(runner.main())
