#!/usr/bin/env python3
"""Serial48Q supplied-list detector composition and borrowed-context checks.

Original-instruction initialized synthetic fixtures; no live registration,
world ownership, provider return, RAM, or frame/console performance claim.
All runtime buffers are caller-owned; Python/JSON are test tooling only.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="detector-pipeline"
runner.EXTRA_INPUT_PATHS=("tools/test_native_detector_pipeline48q.py","cmake/NativeDetectorPipeline48Q.cmake")
runner.SOURCES=("src/game/spring_records48n.cpp","src/game/spring_callback48n.cpp",
    "src/game/query_prepare48o.cpp","src/game/spring_detector48o.cpp",
    "src/game/spring_type3_callback48p.cpp","src/game/collision_type3_leaf48p.cpp",
    "src/game/collision_type3_node48p.cpp","src/game/type3_prepare48p.cpp","src/game/type3_query48p.cpp",
    "src/game/spring_detector_pipeline48q.cpp")
runner.INCLUDES=("src/game","src/assets","third_party/native-json")
runner.TESTS={
    "type3_context48q":{"source":"tests/native/type3_context48q.cpp","fixture":"evidence/type3Query48P/native-fixtures.json"},
    "spring_detector_pipeline48q":{"source":"tests/native/spring_detector_pipeline48q.cpp","fixture":"evidence/springDetectorPipeline48Q/native-fixtures.json"},
}
if __name__=="__main__":sys.exit(runner.main())
