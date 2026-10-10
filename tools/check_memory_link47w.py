"""Require all three replacement units and exclude original archive members."""
from pathlib import Path
import argparse,hashlib,json,re
p=argparse.ArgumentParser();p.add_argument('map',type=Path);p.add_argument('output',type=Path);a=p.parse_args()
t=a.map.read_text();records=[]
for name in ['ps2_memory.cpp','ps2_vif1_interpreter.cpp','ps2_vu1_core.cpp']:
    assert 'ps2_runtime:'+name+'.obj' not in t,'Original runtime object also linked: '+name
    matches=re.findall(r'\s'+re.escape(name)+r'\.obj\s*$',t,re.M)
    assert matches,'Missing replacement object: '+name
    records.append(dict(object=name+'.obj',mapped_symbols=len(matches),original_archive_member_absent=True))
a.output.write_text(json.dumps(dict(map_sha256=hashlib.sha256(a.map.read_bytes()).hexdigest(),objects=records),indent=2))
print('All three isolated replacement objects mapped; originals absent.')
