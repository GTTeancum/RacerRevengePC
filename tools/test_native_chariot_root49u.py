#!/usr/bin/env python3
"""49U complete admitted Chariot-root response and retained graphical/input/body checks."""
import json
from pathlib import Path
import sys
sys.dont_write_bytecode=True
import test_native_chariot_animation49t as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="chariot-root"
evidence="evidence/chariotRoot49U"
root=Path(__file__).resolve().parents[1]
connected=json.loads((root/evidence/"connected/export-report.json").read_text())
authoring=json.loads((root/evidence/"authoring/validation.json").read_text())
new_inputs=("tools/test_native_chariot_root49u.py","cmake/NativeChariotRoot49U.cmake",
    evidence+"/connected/export-report.json",evidence+"/connected/export-report.native.json",
    evidence+"/connected/trace.json",evidence+"/connected/exact-goldens49u.txt")
new_inputs+=tuple(connected["input_sha256"])+tuple(authoring["files"])
new_inputs+=tuple(evidence+"/connected/"+row["mesh_path"] for row in connected["frames"])
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,*new_inputs)))
runner.SOURCES=("src/game/chariot_root49u.cpp","src/game/chariot_scene49u.cpp",
    "src/assets/chariot_root_authoring49u.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "chariot_scene49u":{"source":"tests/native/chariot_scene49u.cpp","fixture":None,"arguments":[]},
    "chariot_root_authoring49u":{"source":"tests/native/chariot_root_authoring49u.cpp","fixture":None,"arguments":["."]},
    "chariot_root49u":{"source":"tests/native/chariot_root49u.cpp","fixture":None,"arguments":["."]},
    "chariot_root_export49u":{"source":"tools/visual/export_chariot_root49u.cpp","fixture":None,"arguments":[]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
