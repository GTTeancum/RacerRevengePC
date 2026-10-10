#!/usr/bin/env python3
"""49V complete ordinary Chariot mode lifecycle and retained related checks."""
import json
from pathlib import Path
import sys
sys.dont_write_bytecode=True
import test_native_chariot_root49u as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="chariot-lifecycle"
evidence="evidence/chariotLifecycle49V"
root=Path(__file__).resolve().parents[1]
connected=json.loads((root/evidence/"connected/export-report.json").read_text())
new_inputs=("tools/test_native_chariot_lifecycle49v.py","cmake/NativeChariotLifecycle49V.cmake",
    evidence+"/connected/export-report.json",evidence+"/connected/export-report.native.json",
    evidence+"/connected/trace.json",evidence+"/connected/exact-goldens49v.txt")
new_inputs+=tuple(connected["input_sha256"])
new_inputs+=tuple(evidence+"/connected/"+row["mesh_path"] for row in connected["frames"])
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,*new_inputs)))
runner.SOURCES=("src/game/secondary_activation49v.cpp","src/game/chariot_blend49v.cpp",
    "src/game/chariot_lifecycle49v.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "secondary_activation49v":{"source":"tests/native/secondary_activation49v.cpp","fixture":None,"arguments":[]},
    "chariot_blend49v":{"source":"tests/native/chariot_blend49v.cpp","fixture":None,"arguments":[]},
    "chariot_lifecycle49v":{"source":"tests/native/chariot_lifecycle49v.cpp","fixture":None,"arguments":["."]},
    "chariot_lifecycle_export49v":{"source":"tools/visual/export_chariot_lifecycle49v.cpp","fixture":None,"arguments":[]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
