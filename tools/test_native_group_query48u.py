#!/usr/bin/env python3
"""Serial 48U bounded outer-group limited-list type3 query checks.

Original-word synthetic proof; serial caller-owned runtime storage.
No live-world ownership, full scheduler or console performance claim.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="group-query"
runner.EXTRA_INPUT_PATHS=("tools/test_native_group_query48u.py","cmake/NativeGroupQuery48U.cmake")
runner.SOURCES=('src/game/spring_records48n.cpp', 'src/game/spring_callback48n.cpp', 'src/game/spring_type3_callback48p.cpp', 'src/game/collision_type3_leaf48p.cpp', 'src/game/collision_type3_node48p.cpp', 'src/game/type3_prepare48p.cpp', 'src/game/type3_query48p.cpp', 'src/game/group_shape48t.cpp', 'src/game/group_query48u.cpp')
runner.INCLUDES=("src/game","src/assets","third_party/native-json")
runner.TESTS={
    "group_query48u":{"source":"tests/native/group_query48u.cpp","fixture":"evidence/groupQuery48U/native-fixtures.json","arguments":["evidence/groupQuery48U/native-fixtures.json"]},
}
if __name__=="__main__":sys.exit(runner.main())
