#!/usr/bin/env python3
"""Serial48S explicit-spatial candidate collection, bounds and ordered merge.

Initialized original-instruction fixtures only, with authenticated methods.
Runtime storage is caller-owned; no live-world/RAM or console FPS claim.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="candidate-collection"
runner.EXTRA_INPUT_PATHS=("tools/test_native_candidate_collection48s.py","cmake/NativeCandidateCollection48S.cmake")
runner.SOURCES=("src/game/candidate_merge48s.cpp","src/game/collection_bounds48s.cpp","src/game/candidate_collection48s.cpp","src/game/group_shape48t.cpp")
runner.INCLUDES=("src/game","third_party/native-json")
runner.TESTS={
    "candidate_merge48s":{"source":"tests/native/candidate_merge48s.cpp","fixture":"evidence/candidateMerge48S/native-fixtures.json"},
    "collection_bounds48s":{"source":"tests/native/collection_bounds48s.cpp","fixture":"evidence/collectionBounds48S/fixtures.json","arguments":["evidence/collectionBounds48S/fixtures.json"]},
    "candidate_collection48s":{"source":"tests/native/candidate_collection48s.cpp","fixture":"evidence/candidateCollection48S/native-fixtures.json","arguments":["evidence/candidateCollection48S/native-fixtures.json"]},
}
if __name__=="__main__":sys.exit(runner.main())
