#!/usr/bin/env python3
"""Focused actual-terrain to canonical-main-body step; stops at late query."""
import sys
sys.dont_write_bytecode=True
import check_vehicle_static_response50g as frame
frame_runner=frame.runner
frame_sources=tuple(frame_runner.SOURCES)
frame_extra=tuple(frame_runner.EXTRA_INPUT_PATHS)
import check_vehicle_static_query50h as query
runner=query.runner
runner.SUITE_NAME="main-ground-step50i"
runner.SOURCES=tuple(dict.fromkeys((*frame_sources,*runner.SOURCES)))
runner.TESTS={"main_ground_step50i":{"source":"tests/native/main_ground_step50i.cpp","fixture":"evidence/staticType2Producer49X/original-word/native-fixtures.json","arguments":["evidence/staticType2Producer49X/original-word/native-fixtures.json"]}}
runner.EXTRA_INPUT_PATHS=tuple(dict.fromkeys((*frame_extra,*runner.EXTRA_INPUT_PATHS,"tools/check_main_ground_step50i.py","work/impactAuthoring49Y/Pod01.phy","work/impactAuthoring49Y/ta.tun")))
if __name__=="__main__":sys.exit(runner.main())
