#!/usr/bin/env python3
"""Build/run focused48M terrain checks serially with no downloads or CMake.

Uses the same recorded strict compiler/sanitizer workflow as the spring suite.
New runtime paths use caller-owned workspaces; asset loading and legacy wrappers
remain separate setup/regression dependencies. No live registration/world-list claim.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="terrain"
runner.EXTRA_INPUT_PATHS=("tools/test_native_terrain48m.py","cmake/NativeTerrain48M.cmake")
runner.SOURCES=(
    "src/game/collision_query48h.cpp","src/game/collision_tree48i.cpp",
    "src/game/collision_tree_workspace48m.cpp","src/game/terrain_query48j.cpp",
    "src/game/terrain_query_workspace48m.cpp","src/game/terrain_prepare48m.cpp",
    "src/assets/import_collision48g.cpp","src/assets/collision_runtime48h.cpp",
    "src/assets/collision_mesh48i.cpp",
)
runner.INCLUDES=("src/game","src/assets","third_party/native-json")
meshes=[{"fixture":"evidence/query48I/17-ta_colcavetop.rrcol","binary":True},
        {"fixture":"evidence/query48I/18-zone04_col_floor.rrcol","binary":True}]
def mesh_test(source):
    return {"source":"tests/native/"+source,"fixture":"evidence/query48I/cache-binding.json","extra_fixtures":meshes}
runner.TESTS={
    "triangle48h":{"source":"tests/native/collision_query48h.cpp","fixture":None},
    "tree48i":mesh_test("collision_tree48i.cpp"),
    "tree_workspace48m":mesh_test("collision_workspace48m.cpp"),
    "terrain48j":mesh_test("terrain_query48j.cpp"),
    "terrain_workspace48m":mesh_test("terrain_workspace48m.cpp"),
    "terrain_prepare48m":{"source":"tests/native/terrain_prepare48m.cpp","fixture":"evidence/terrainPrepare48M/fixtures.json"},
}
if __name__=="__main__":sys.exit(runner.main())
