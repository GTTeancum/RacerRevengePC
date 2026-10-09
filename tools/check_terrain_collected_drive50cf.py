#!/usr/bin/env python3
"""Focused normal source build/run. Explicitly requested broad checks are separate."""
from pathlib import Path
import subprocess,sys,json
from check_terrain_registered_drive50bv import SOURCES,ROOT
if len(sys.argv)!=2:raise SystemExit('usage: check_terrain_collected_drive50cf.py NEW_OUTPUT_DIR')
out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=False)
sources=['tests/native/terrain_collected_drive50cf.cpp'if x=='tests/native/terrain_registered_drive50bv.cpp'else x for x in SOURCES]
sources+=['src/game/'+x+'.cpp'for x in ['indexed_type2_contact50cc','vehicle_indexed_main50cd','vehicle_indexed_secondary50cd','vehicle_candidates50ca','candidate_handles50cb','collection_projection50cf','candidate_collection50bx','candidate_merge48s','type2_bounds50by','collection_bounds48s']]
cmd=['g++','-std=c++20','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(ROOT/'src/assets'),'-I'+str(ROOT/'src/game'),'-I'+str(ROOT/'third_party/native-json')]+[str(ROOT/x)for x in sources]+['-lz','-o',str(out/'drive')]
for name,args in [('build',cmd),('smoke',[str(out/'drive'),str(ROOT/'evidence/staticType2Producer49X/original-word/native-fixtures.json')])]:
 (out/(name+'-command.json')).write_text(json.dumps(args,indent=2))
 p=subprocess.run(args,cwd=ROOT,capture_output=True,text=True);(out/(name+'.stdout.log')).write_text(p.stdout);(out/(name+'.stderr.log')).write_text(p.stderr)
 print(name,p.returncode,flush=True)
 if p.returncode:raise SystemExit(p.returncode)
