#!/usr/bin/env python3
"""Focused ready-main detector preparation and actual-resource query check."""
import sys
sys.dont_write_bytecode=True
import check_static_contact_response50e as previous
runner=previous.runner
runner.SUITE_NAME="vehicle-static-query50h"
runner.SOURCES=(*runner.SOURCES,"src/game/query_prepare48o.cpp","src/game/vehicle_static_query50h.cpp")
runner.TESTS={"vehicle_static_query50h":{"source":"tests/native/vehicle_static_query50h.cpp","fixture":"evidence/staticContactResponse50E/actual-query-subset.json","arguments":["evidence/staticContactResponse50E/actual-query-subset.json"]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,"tools/check_vehicle_static_query50h.py")
if __name__=="__main__":sys.exit(runner.main())
