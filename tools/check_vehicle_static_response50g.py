#!/usr/bin/env python3
"""One focused canonical-frame/static-response composition suite."""
import sys
sys.dont_write_bytecode=True
import test_native_impact_frame50a as previous
runner=previous.runner
runner.SUITE_NAME="vehicle-static-response50g"
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,
 "src/game/vehicle_static_response50g.cpp","src/game/static_contact_response50e.cpp",
 "src/game/spring_model48k.cpp","src/game/spring_contact48j.cpp",
 "src/game/spring_impulse48k.cpp","src/game/spring_force48l.cpp")))
runner.TESTS={"vehicle_static_response50g":{"source":"tests/native/vehicle_static_response50g.cpp","fixture":None,"arguments":[]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,"tools/check_vehicle_static_response50g.py")
if __name__=="__main__":sys.exit(runner.main())
