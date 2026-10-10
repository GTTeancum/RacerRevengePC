#!/usr/bin/env python3
"""Serial48R bounded vehicle contact and body partner-list checks.

Independent initialized original-word fixtures; no invented virtual/spatial
results, world ownership, RAM replay, frame or console performance claim.
The runtime is allocation-free; Python and JSON are validation tooling only.
"""
import sys
sys.dont_write_bytecode=True
import test_native_spring48k as runner
runner.__doc__=__doc__
runner.SUITE_NAME="vehicle-contact"
runner.EXTRA_INPUT_PATHS=("tools/test_native_vehicle_contact48r.py","cmake/NativeVehicleContact48R.cmake")
runner.SOURCES=("src/game/spring_records48n.cpp","src/game/body_contact_list48r.cpp","src/game/vehicle_contact48r.cpp")
runner.INCLUDES=("src/game","third_party/native-json")
runner.TESTS={
    "body_contact_list48r":{"source":"tests/native/body_contact_list48r.cpp",
        "fixture":"evidence/bodyContactList48R/body-contact-list-fixtures.bin","shape":(3260,44),
        "extra_fixtures":[{"fixture":"evidence/bodyContactList48R/body-contact-reset-fixtures.bin","shape":(1024,42)}]},
    "vehicle_contact48r":{"source":"tests/native/vehicle_contact48r.cpp","fixture":"evidence/vehicleContact48R/native-fixtures.json"},
}
if __name__=="__main__":sys.exit(runner.main())
