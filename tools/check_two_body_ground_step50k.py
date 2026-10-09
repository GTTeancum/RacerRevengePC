#!/usr/bin/env python3
"""Focused low-speed two-body actual-terrain step ending at the late query cut."""
import sys
sys.dont_write_bytecode=True
import check_secondary_contact50j as previous
runner=previous.runner
runner.SUITE_NAME='two-body-ground-step50k'
runner.TESTS={'two_body_ground_step50k':{'source':'tests/native/two_body_ground_step50k.cpp','fixture':'evidence/staticType2Producer49X/original-word/native-fixtures.json','arguments':['evidence/staticType2Producer49X/original-word/native-fixtures.json']}}
runner.EXTRA_INPUT_PATHS=(*runner.EXTRA_INPUT_PATHS,'tools/check_two_body_ground_step50k.py')
if __name__=='__main__':sys.exit(runner.main())
