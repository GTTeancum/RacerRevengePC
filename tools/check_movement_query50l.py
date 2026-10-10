#!/usr/bin/env python3
"""Focused native movement static candidate seam and original callback leaf."""
import sys
sys.dont_write_bytecode=True
import test_native_type3_query48p as previous
runner=previous.runner
runner.SUITE_NAME='movement-query50l'
runner.SOURCES=(*runner.SOURCES,'src/game/movement_callback50l.cpp','src/game/movement_static_query50l.cpp')
runner.TESTS={
 'movement_callback50l':{'source':'tests/native/movement_callback50l.cpp','fixture':'evidence/movementQuery50L/callback/fixtures.json','arguments':['evidence/movementQuery50L/callback/fixtures.json']},
 'movement_query_reference50l':{'source':'tests/native/movement_query_reference50l.cpp','fixture':'evidence/movementQuery50L/reference/fixtures.json','arguments':['evidence/movementQuery50L/reference/fixtures.json']}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_movement_query50l.py')
if __name__=='__main__':sys.exit(runner.main())
