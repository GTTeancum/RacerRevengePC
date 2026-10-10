#!/usr/bin/env python3
"""Write a portable source-only build command. Does not build or run anything.
Usage: prepare_submit_build50cw.py OUTPUT_COMMAND_JSON OUTPUT_EXECUTABLE
"""
from pathlib import Path
import json,sys
from check_terrain_registered_drive50bv import ROOT,SOURCES

def main():
 if len(sys.argv)!=3:raise SystemExit(__doc__)
 sources=['tests/native/terrain_present_drive50db.cpp'if x=='tests/native/terrain_registered_drive50bv.cpp'else x for x in SOURCES]
 sources+=['src/game/'+n+'.cpp'for n in ['candidate_collection50bx','candidate_merge48s','type2_bounds50by','collection_bounds48s','vehicle_candidates50ca','candidate_handles50cb','indexed_type2_contact50cc','vehicle_indexed_main50cd','vehicle_indexed_secondary50cd','collection_projection50cf','zone_search_views50cg','zone_registration50cj','global_zone_chain50ck','zone_lifecycle50cl','vehicle_draw50cm','vehicle_draw_batch50co','zone_registration_events50cq','vehicle_draw_submit50cs','controller_ingress50cx','vehicle_present50da']]
 sources+=['src/assets/zone_search_authoring50cn.cpp','src/assets/vehicle_draw_assets50cr.cpp']
 sources+=['platform/linux/native/vehicle_draw_gl50cv.cpp']
 sources=list(dict.fromkeys(sources))
 command=['g++','-std=c++20','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(ROOT/'src/assets'),'-I'+str(ROOT/'src/game'),'-I'+str(ROOT/'third_party/native-json')]+[str(ROOT/p)for p in sources]+['-lz','-o',str(Path(sys.argv[2]).resolve())]
 Path(sys.argv[1]).write_text(json.dumps(command,indent=2))
if __name__=='__main__':main()
