"""Combine the compact registry with current accepted IOP/GS/leaf candidates."""
from pathlib import Path
import hashlib,json,re
root=Path(__file__).resolve().parents[1];out=root/'build/memory47N';out.mkdir(exist_ok=True)
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
provenance=json.loads((root/'build/compact47E/provenance.json').read_text())
for entry in provenance['inputs']:assert sha(root/entry['path'])==entry['sha256'],entry['path']
for entry in provenance['outputs']:
    assert sha(root/'build/compact47E'/entry['path'])==entry['sha256'],entry['path']
for variant,source in [('registry','work/native/project/src/probe.cpp'),('memory','build/leaf47M/probe.cpp')]:
    text=(root/source).read_text().replace('#include "ps2_runtime.h"','#include "ps2_runtime.h"\n#include "registry_adapter47e.h"')
    needle='        rr_observe::report();';assert text.count(needle)==1
    text=text.replace(needle,'''        std::cerr<<"[rr:compact-registry] pages="<<rr47e::allocatedPages()
                 <<" directory_bytes="<<rr47e::directoryBytes()
                 <<" replacement_bytes="<<rr47e::replacementBytes()<<"\\n";
'''+needle)
    (out/(variant+'-probe.cpp')).write_text(text)
inventory=json.loads((root/'evidence/47M/leaf-eligibility.json').read_text())
selected={e['symbol'] for e in inventory['functions'] if not set(e['operations'])&{'MOVN','MOVZ'}}
mapfile=root/'evidence/47E/compact-probe.map';rows=[]
for m in re.finditer(r'^ 0001:([0-9a-fA-F]+)\s+(\S+)\s+[0-9a-fA-F]+\s+f\s+(.+)$',mapfile.read_text(),re.M):
    rows.append((int(m[1],16),m[2],m[3]))
rows.sort();spans=[]
for (begin,symbol,owner),(end,_,_) in zip(rows,rows[1:]):
    name=symbol.split('@@')[0][1:] if symbol.startswith('?') else symbol
    if name in selected:spans.append(dict(symbol=name,span_bytes=end-begin,object=owner))
assert len(spans)==283
report=dict(original_sources_modified=False,registry_source_inputs_reverified=True,
    leaf_count=283,leaf_linked_spans_sum=sum(x['span_bytes'] for x in spans),leaf_spans=spans,
    map_sha256=sha(mapfile),size_caveat='Adjacent linked-symbol spans include padding and may include aliases; not independently removable bytes. Tiny leaves are retained because replacement executor cost must be included.',
    outputs=[dict(path=p.name,sha256=sha(p)) for p in sorted(out.glob('*.cpp'))])
(out/'provenance.json').write_text(json.dumps(report,indent=2))
print('Prepared compact-registry integration; original leaf span estimate:',report['leaf_linked_spans_sum'])
