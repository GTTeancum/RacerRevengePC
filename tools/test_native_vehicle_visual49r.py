#!/usr/bin/env python3
"""49R original container/engine-root response with authored input and retained regressions."""
import sys
sys.dont_write_bytecode=True
import test_native_engine_animation49q as prior
runner=prior.runner
runner.__doc__=__doc__
runner.SUITE_NAME="vehicle-visual"
evidence="evidence/vehicleVisual49R"
runner.EXTRA_INPUT_PATHS=tuple(runner.EXTRA_INPUT_PATHS)+(
    "tools/test_native_vehicle_visual49r.py", "cmake/NativeVehicleVisual49R.cmake",
    evidence+"/connected/input-manifest.json", evidence+"/connected/exact-goldens49r.txt",
)
runner.SOURCES=("src/game/vehicle_visual49r.cpp", "src/game/visual_container49r.cpp",
    "src/game/visual_response49r.cpp", "src/assets/vehicle_visual_authoring49r.cpp")+tuple(runner.SOURCES)
runner.TESTS={
    "visual_container49r":{"source":"tests/native/visual_container49r.cpp","fixture":None,"arguments":[]},
    "visual_response49r":{"source":"tests/native/visual_response49r.cpp","fixture":None,"arguments":[]},
    "vehicle_visual_authoring49r":{"source":"tests/native/vehicle_visual_authoring49r.cpp","fixture":None,"arguments":["."]},
    "vehicle_visual49r":{"source":"tests/native/vehicle_visual49r.cpp","fixture":None,"arguments":["."]},
    "vehicle_visual_diagnostic49r":{"source":"tests/native/vehicle_visual_diagnostic49r.cpp","fixture":None,"arguments":[]},
    **runner.TESTS,
}
if __name__=="__main__":sys.exit(runner.main())
