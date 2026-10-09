#!/usr/bin/env python3
"""49T full internal Chariot animation, original IK and all-three posed palettes.

The source-acos elementary test API stays confined to its separately verified
normal/sanitizer proof; this aggregate compiles ordinary production units.
"""
import json
from pathlib import Path
import sys
sys.dont_write_bytecode=True
import test_native_animated_view49s as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="chariot-animation"
evidence="evidence/chariotAnimation49T"
root=Path(__file__).resolve().parents[1]
connected=json.loads((root/evidence/"connected/export-report.json").read_text())
ik=json.loads((root/evidence/"ik/validation/validation.json").read_text())
new_inputs=("tools/test_native_chariot_animation49t.py","cmake/NativeChariotAnimation49T.cmake",
    evidence+"/connected/export-report.json",evidence+"/connected/export-report.native.json",
    evidence+"/connected/trace.json",evidence+"/connected/exact-goldens49t.txt")
new_inputs+=tuple(connected["input_sha256"])+tuple(ik["files"])
new_inputs+=tuple(evidence+"/connected/"+row["mesh_path"] for row in connected["frames"])
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,*new_inputs)))
runner.SOURCES=("src/game/source_acos49t.cpp","src/game/model_motion49t.cpp",
    "src/game/chariot_ik49t.cpp","src/game/vehicle_animation49t.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "model_motion49t":{"source":"tests/native/model_motion49t.cpp","fixture":None,"arguments":[]},
    "chariot_ik49t":{"source":"tests/native/chariot_ik49t.cpp","fixture":None,"arguments":[]},
    "vehicle_animation49t":{"source":"tests/native/vehicle_animation49t.cpp","fixture":None,"arguments":["."]},
    "chariot_export49t":{"source":"tools/visual/export_chariot49t.cpp","fixture":None,"arguments":[]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
