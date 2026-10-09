#!/usr/bin/env python3
"""120 native terrain frames plus a genuine empty-candidate support control."""
import sys
sys.dont_write_bytecode=True
import check_secondary_contact50j as previous
runner=previous.runner
runner.SUITE_NAME='terrain-persistent50z'
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,'src/assets/pod_flame50w.cpp','src/assets/import_scene48d.cpp','src/assets/import_texture48e.cpp','src/game/spring_type3_callback48p.cpp','src/game/collision_type3_leaf48p.cpp','src/game/collision_type3_node48p.cpp','src/game/type3_prepare48p.cpp','src/game/type3_query48p.cpp','src/game/movement_callback50l.cpp','src/game/movement_static_query50l.cpp')))
fixture='evidence/staticType2Producer49X/original-word/native-fixtures.json'
runner.TESTS={
 'terrain_persistent50z':{'source':'tests/native/terrain_persistent50z.cpp','fixture':fixture,'arguments':[fixture]},
 'terrain_no_support50z':{'source':'tests/native/terrain_persistent50z.cpp','fixture':fixture,'arguments':[fixture,'--no-support']}}
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,'tools/check_terrain_persistent50z.py','src/game/vehicle_base_tail50x.h','src/game/vehicle_base_finish50y.h','src/game/base_pose_history50y.h','src/assets/pod_flame_runtime50w.h','work/native/project/disc/PODS/POD01/POD01.RES')))
if __name__=='__main__':sys.exit(runner.main())
