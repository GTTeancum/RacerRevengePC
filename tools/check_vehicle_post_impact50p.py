#!/usr/bin/env python3
"""Focused optional-probe/transition frame connection; strict retained regression."""
import sys
sys.dont_write_bytecode=True
import check_vehicle_impact_effects50o as previous
runner=previous.runner
runner.SUITE_NAME='vehicle-post-impact50p'
runner.TESTS={
 'optional_probe_effects50p':{'source':'tests/native/optional_probe_effects50p.cpp','fixture':'evidence/optionalProbeEffects50P/fixtures.json','arguments':['evidence/optionalProbeEffects50P/fixtures.json']},
 'vehicle_post_impact50p':{'source':'tests/native/vehicle_post_impact50p.cpp','fixture':'evidence/optionalProbeEffects50P/fixtures.json','arguments':['evidence/optionalProbeEffects50P/fixtures.json']},
 'vehicle_frame49w':{'source':'tests/native/vehicle_frame49w.cpp','fixture':None,'arguments':[]}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_vehicle_post_impact50p.py')
if __name__=='__main__':sys.exit(runner.main())
