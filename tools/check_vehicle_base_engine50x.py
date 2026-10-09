#!/usr/bin/env python3
"""Source-correct local position, cold flame ownership and engine frame join."""
import sys
sys.dont_write_bytecode=True
import check_vehicle_packet50s as previous
runner=previous.runner
runner.SUITE_NAME='vehicle-base-engine50x'
runner.SOURCES=tuple(dict.fromkeys((*runner.SOURCES,'src/assets/pod_flame50w.cpp','src/assets/import_scene48d.cpp','src/assets/import_texture48e.cpp')))
runner.TESTS={
 'vehicle_base_engine50x':{'source':'tests/native/vehicle_base_engine50x.cpp','fixture':'evidence/baseTail50T/fixtures.json','arguments':['evidence/optionalProbeEffects50P/fixtures.json','evidence/plainEffectTail50Q/fixtures.json']},
 'vehicle_frame49w':{'source':'tests/native/vehicle_frame49w.cpp','fixture':None,'arguments':[]}}
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*runner.EXTRA_INPUT_PATHS,'tools/check_vehicle_base_engine50x.py','src/game/vehicle_base_tail50x.h','src/assets/pod_flame_runtime50w.h','src/assets/pod_flame50w.h','evidence/baseTail50T/fixtures.json','work/native/project/disc/PODS/POD01/POD01.RES')))
if __name__=='__main__':sys.exit(runner.main())
