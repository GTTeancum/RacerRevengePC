#!/usr/bin/env python3
"""49S original model palettes, rigid positions and rendered-engine producer regressions."""
import json
from pathlib import Path
import sys
sys.dont_write_bytecode=True
import test_native_vehicle_visual49r as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="animated-view"
evidence="evidence/modelPalette49S"
root=Path(__file__).resolve().parents[1]
connected=json.loads((root/evidence/"connected/export-report.json").read_text())
new_inputs=("tools/test_native_animated_view49s.py", "cmake/NativeAnimatedView49S.cmake",
    evidence+"/rigid/normal/fixture.bin", evidence+"/rigid/normal/fixture-manifest.json",
    evidence+"/connected/export-report.json", evidence+"/connected/export-report.native.json",
    evidence+"/connected/trace.json", evidence+"/connected/exact-goldens49s.txt")
new_inputs+=tuple(connected["input_sha256"])
new_inputs+=tuple(evidence+"/connected/"+row["mesh_path"] for row in connected["frames"])
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,*new_inputs)))
runner.SOURCES=("src/game/model_palette49s.cpp", "src/game/rigid_vertices49s.cpp",
    "src/assets/scene_asset48d.cpp", "src/assets/native_asset48b.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "model_palette49s":{"source":"tests/native/model_palette49s.cpp","fixture":None,"arguments":[]},
    "rigid_vertices49s":{"source":"tests/native/rigid_vertices49s.cpp","fixture":evidence+"/rigid/normal/fixture.bin","binary":True,"arguments":[evidence+"/rigid/normal/fixture.bin"]},
    "animated_export49s":{"source":"tools/visual/export_animated49s.cpp","fixture":None,"arguments":[]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
