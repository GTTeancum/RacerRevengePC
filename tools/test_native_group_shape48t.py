#!/usr/bin/env python3
"""Serial 48T group shape-zero helper and original candidate-collection checks.

Explicit source-derived spatial inputs; no live world, RAM or console FPS claim.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="group-shape"
runner.EXTRA_INPUT_PATHS=("tools/test_native_group_shape48t.py","cmake/NativeGroupShape48T.cmake")
runner.SOURCES=("src/game/collection_bounds48s.cpp","src/game/candidate_collection48s.cpp","src/game/group_shape48t.cpp")
runner.INCLUDES=("src/game","third_party/native-json")
runner.TESTS={
    "group_shape48t":{"source":"tests/native/group_shape48t.cpp","fixture":"evidence/groupShape48T/helper-fixtures.json","arguments":["evidence/groupShape48T/helper-fixtures.json"]},
    "candidate_collection48t":{"source":"tests/native/candidate_collection48t.cpp","fixture":"evidence/groupShape48T/collector-fixtures.json","arguments":["evidence/groupShape48T/collector-fixtures.json"]},
}
if __name__=="__main__":sys.exit(runner.main())
