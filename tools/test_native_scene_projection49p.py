#!/usr/bin/env python3
"""49P fixed-node scene/Chariot projection with retained body/input regressions."""
import sys
sys.dont_write_bytecode=True
import test_native_authored_geometry49o as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="scene-projection"
evidence="evidence/sceneProjection49P"
psg="evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg"
runner.EXTRA_INPUT_PATHS=tuple(runner.EXTRA_INPUT_PATHS)+(
    "tools/test_native_scene_projection49p.py","cmake/NativeSceneProjection49P.cmake",
    evidence+"/connected/supplied-physical-scenes49p.bin",
    evidence+"/connected/exact-goldens49p.txt",evidence+"/connected/input-manifest.json",
)
runner.SOURCES=("src/game/scene_graph49p.cpp","src/game/chariot_projection49p.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "scene_graph_api49p":{"source":"tests/native/scene_graph_api49p.cpp","fixture":None,"arguments":[]},
    "scene_graph49p":{"source":"tests/native/scene_graph49p.cpp","fixture":psg,"binary":True,"arguments":[psg]},
    "chariot_projection49p":{"source":"tests/native/chariot_projection49p.cpp","fixture":None,"arguments":[]},
    "scene_frame_diagnostic49p":{"source":"tests/native/scene_frame_diagnostic49p.cpp","fixture":evidence+"/connected/supplied-physical-scenes49p.bin","binary":True,"arguments":[evidence+"/connected/supplied-physical-scenes49p.bin",psg,evidence+"/connected/exact-goldens49p.txt"]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
