#!/usr/bin/env python3
"""Serial48P bounded type3 query, preparation, node, leaf and callback checks.

Explicit original-derived fixtures only; no download, renderer or live world.
Caller-owned work buffers; runtime is serial and allocation-free. Python and
JSON are validation tooling, not shipped runtime dependencies. Host timings
are not Xbox FPS. Fixture-relative arguments adapt standalone kernel tests.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="type3-query"
runner.EXTRA_INPUT_PATHS=("tools/test_native_type3_query48p.py","cmake/NativeType3Query48P.cmake")
runner.SOURCES=("src/game/spring_records48n.cpp","src/game/spring_callback48n.cpp",
    "src/game/spring_type3_callback48p.cpp","src/game/collision_type3_leaf48p.cpp",
    "src/game/collision_type3_node48p.cpp","src/game/type3_prepare48p.cpp","src/game/type3_query48p.cpp")
runner.INCLUDES=("src/game","src/assets","third_party/native-json")
runner.TESTS={
    "spring_type3_callback48p":{"source":"tests/native/spring_type3_callback48p.cpp",
        "fixture":"evidence/springCallback48P/static-type3-callback-fixtures.bin","shape":(768,378)},
    "collision_type3_leaf48p":{"source":"tests/native/collision_type3_leaf48p.cpp",
        "fixture":"evidence/type3Leaf48P/leaf-fixtures.bin","shape":(904,78),
        "arguments":["evidence/type3Leaf48P/leaf-fixtures.bin"]},
    "collision_type3_node48p":{"source":"tests/native/collision_type3_node48p.cpp",
        "fixture":"evidence/type3Node48P/sphere-fixtures.bin","shape":(467,11),
        "extra_fixtures":[{"fixture":"evidence/type3Node48P/ray-fixtures.bin","shape":(3964,16)}],
        "arguments":["evidence/type3Node48P"]},
    "type3_prepare48p":{"source":"tests/native/type3_prepare48p.cpp",
        "fixture":"evidence/type3Query48P/native-fixtures.json"},
    "type3_query48p":{"source":"tests/native/type3_query48p.cpp",
        "fixture":"evidence/type3Query48P/native-fixtures.json"},
}
if __name__=="__main__":sys.exit(runner.main())
