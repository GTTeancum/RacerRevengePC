#!/usr/bin/env python3
"""Source-derived center-grid startup, balanced/differential throttle and braking."""
import sys
sys.dont_write_bytecode=True
import check_terrain_persistent50z as previous
runner=previous.runner
runner.SUITE_NAME='grid-driving50ae'
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,'src/assets/track_start50ad.cpp','src/game/grid_center50ac.cpp')))
fixture='evidence/staticType2Producer49X/original-word/native-fixtures.json'
runner.TESTS={
 'grid_differential50ae':{'source':'tests/native/terrain_grid_drive50ae.cpp','fixture':fixture,'arguments':[fixture]},
 'grid_balanced50ae':{'source':'tests/native/terrain_grid_drive50ae.cpp','fixture':fixture,'arguments':[fixture,'--balanced']}}
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,'tools/check_grid_driving50ae.py','src/assets/track_start50ad.h','src/game/grid_center50ac.h','work/native/project/disc/TRACKS/TA.RES')))
if __name__=='__main__':sys.exit(runner.main())
