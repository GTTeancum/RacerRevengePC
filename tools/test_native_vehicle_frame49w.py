#!/usr/bin/env python3
"""49W graph-aware advancing frames and all preserved related69 suites."""
import json
from pathlib import Path
import sys
sys.dont_write_bytecode=True
import test_native_chariot_lifecycle49v as previous
import native_frame_runner49w as runner
for name in ("INCLUDES","SOURCES","TESTS","STRICT_FLAGS","EXTRA_INPUT_PATHS"):
    setattr(runner,name,getattr(previous.runner,name))
runner.__doc__=__doc__
runner.SUITE_NAME="vehicle-frame"
runner.INCLUDES=tuple(dict.fromkeys((*runner.INCLUDES,"third_party/native-json")))
root=Path(__file__).resolve().parents[1]
evidence="evidence/frameBridge49W"
new_inputs=["tools/test_native_vehicle_frame49w.py","tools/native_frame_runner49w.py","cmake/NativeVehicleFrame49W.cmake"]
for scenario in ("connected","connected/no-support"):
    report=json.loads((root/evidence/scenario/"export-report.json").read_text())
    new_inputs += [evidence+"/"+scenario+"/"+x for x in ("export-report.json","export-report.native.json","trace.json","exact-goldens49w.txt")]

    new_inputs += [evidence+"/"+scenario+"/"+row["mesh_path"] for row in report["frames"]]
freeze=json.loads((root/evidence/"connected/freeze.json").read_text())
new_inputs += list(freeze["input_sha256"])+list(freeze["source_sha256"])+[evidence+"/connected/freeze.json",evidence+"/shape/podboxf.col"]
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,*new_inputs)))
runner.SOURCES=("src/game/base_pose_history50y.cpp","src/game/engine_tail50t.cpp","src/game/flame_bindings50v.cpp","src/game/packed_vehicle_tail50r.cpp","src/game/float_unsigned50r.cpp","src/game/plain_effect_tail50q.cpp","src/game/optional_probe_effects50p.cpp","src/game/impact_transitions50c.cpp","src/game/impact_effects50b.cpp","src/game/movement_hit_point50m.cpp","src/game/graph_body49w.cpp","src/game/pod_shape49w.cpp","src/assets/pod_shape49w.cpp","src/game/vehicle_frame49w.cpp","src/assets/import_collision48g.cpp","src/assets/import_decode48c.cpp")+tuple(runner.SOURCES)
runner.TESTS={
 "graph_body49w":{"source":"tests/native/graph_body49w.cpp","fixture":None,"arguments":[]},
 "pod_shape49w":{"source":"tests/native/pod_shape49w.cpp","fixture":evidence+"/shape/verification/fixtures.json","arguments":[evidence+"/shape/verification/fixtures.json",evidence+"/shape/podboxf.col","work/native/project/disc/PODS/POD01/POD01.RES"]},
 "vehicle_frame49w":{"source":"tests/native/vehicle_frame49w.cpp","fixture":None,"arguments":[]},
 "vehicle_frame_export49w":{"source":"tools/visual/export_vehicle_frame49w.cpp","fixture":None,"arguments":[]},
 "vehicle_frame_no_support49w":{"source":"tools/visual/export_vehicle_frame49w.cpp","fixture":None,"literal_arguments":["--verify-no-support"]},
 **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
