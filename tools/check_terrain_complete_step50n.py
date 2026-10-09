#!/usr/bin/env python3
"""One actual-terrain two-body physics/query/tail cycle; supplied world boundary."""
import sys
sys.dont_write_bytecode=True
import check_vehicle_movement_tail50m as previous
runner=previous.runner
runner.SUITE_NAME='terrain-complete-step50n'
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,'src/game/spring_type3_callback48p.cpp','src/game/collision_type3_leaf48p.cpp','src/game/collision_type3_node48p.cpp','src/game/type3_prepare48p.cpp','src/game/type3_query48p.cpp','src/game/movement_callback50l.cpp','src/game/movement_static_query50l.cpp')))
runner.TESTS={'terrain_complete_step50n':{'source':'tests/native/terrain_complete_step50n.cpp','fixture':'evidence/staticType2Producer49X/original-word/native-fixtures.json','arguments':['evidence/staticType2Producer49X/original-word/native-fixtures.json']]}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_terrain_complete_step50n.py')
if __name__=='__main__':sys.exit(runner.main())
