#!/usr/bin/env python3
import sys
sys.dont_write_bytecode=True
import check_vehicle_post_impact50p as previous
runner=previous.runner
runner.SUITE_NAME='vehicle-packet50s'
runner.TESTS={
 'plain_effect_tail50q':{'source':'tests/native/plain_effect_tail50q.cpp','fixture':'evidence/plainEffectTail50Q/fixtures.json','arguments':['evidence/plainEffectTail50Q/fixtures.json']},
 'vehicle_packet50s':{'source':'tests/native/vehicle_packet50s.cpp','fixture':'evidence/plainEffectTail50Q/fixtures.json','arguments':['evidence/optionalProbeEffects50P/fixtures.json','evidence/plainEffectTail50Q/fixtures.json']},
 'vehicle_frame49w':{'source':'tests/native/vehicle_frame49w.cpp','fixture':None,'arguments':[]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_vehicle_packet50s.py','evidence/optionalProbeEffects50P/fixtures.json')
if __name__=='__main__':sys.exit(runner.main())
