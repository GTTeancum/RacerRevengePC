#!/usr/bin/env python3
"""Focused effects frame connection and retained source-proven post-impact leaves."""
import sys
sys.dont_write_bytecode=True
import check_vehicle_movement_tail50m as previous
runner=previous.runner
runner.SUITE_NAME='vehicle-impact-effects50o'
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,'src/game/impact_transitions50c.cpp')))
runner.TESTS={
 'impact_effects50b':{'source':'tests/native/impact_effects50b.cpp','fixture':'work/local50B/impact-effects/fixtures.bin','arguments':['work/local50B/impact-effects/fixtures.bin']},
 'impact_transitions50c':{'source':'tests/native/impact_transitions50c.cpp','fixture':'work/local50C/impact-transitions/fixtures.bin','arguments':['work/local50C/impact-transitions/fixtures.bin']},
 'vehicle_impact_effects50o':{'source':'tests/native/vehicle_impact_effects50o.cpp','fixture':None,'arguments':[]},
 'vehicle_frame49w':{'source':'tests/native/vehicle_frame49w.cpp','fixture':None,'arguments':[]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_vehicle_impact_effects50o.py')
if __name__=='__main__':sys.exit(runner.main())
