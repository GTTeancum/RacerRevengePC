#!/usr/bin/env python3
"""Focused early secondary getter, static query and secondary response checks."""
import sys
sys.dont_write_bytecode=True
import check_main_ground_step50i as previous
runner=previous.runner
runner.SUITE_NAME="secondary-contact50j"
runner.SOURCES=(*runner.SOURCES,"src/game/secondary_query_scene50j.cpp","src/game/vehicle_secondary_query50j.cpp","src/game/vehicle_secondary_response50j.cpp")
runner.TESTS={
 "secondary_query_scene50j":{"source":"tests/native/secondary_query_scene50j.cpp","fixture":None,"arguments":[]},
 "vehicle_secondary_query50j":{"source":"tests/native/vehicle_secondary_query50j.cpp","fixture":"evidence/staticContactResponse50E/actual-query-subset.json","arguments":["evidence/staticContactResponse50E/actual-query-subset.json"]},
 "vehicle_secondary_response50j":{"source":"tests/native/vehicle_secondary_response50j.cpp","fixture":None,"arguments":[]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,"tools/check_secondary_contact50j.py")
if __name__=="__main__":sys.exit(runner.main())
