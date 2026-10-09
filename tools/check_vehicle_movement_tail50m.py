#!/usr/bin/env python3
"""Focused hit-point, hit/miss frame-tail and retained advancing-frame checks."""
import sys
sys.dont_write_bytecode=True
import check_secondary_contact50j as previous
runner=previous.runner
runner.SUITE_NAME='vehicle-movement-tail50m'
runner.TESTS={
 'movement_hit_point50m':{'source':'tests/native/movement_hit_point50m.cpp','fixture':'evidence/movementHitPoint50M/fixtures.json','arguments':['evidence/movementHitPoint50M/fixtures.json']},
 'vehicle_movement_tail50m':{'source':'tests/native/vehicle_movement_tail50m.cpp','fixture':None,'arguments':[]},
 'vehicle_frame49w':{'source':'tests/native/vehicle_frame49w.cpp','fixture':None,'arguments':[]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_vehicle_movement_tail50m.py')
if __name__=='__main__':sys.exit(runner.main())
