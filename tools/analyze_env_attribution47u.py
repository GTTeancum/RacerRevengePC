"""Name/callsite counts; never infer elapsed cost or FPS from these totals."""
from pathlib import Path
import argparse,bisect,collections,csv,hashlib,json,re,struct
p=argparse.ArgumentParser();p.add_argument('run',type=Path);p.add_argument('--map',type=Path,required=True);p.add_argument('--binary',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
def sha(f):
    with f.open('rb') as stream:return hashlib.file_digest(stream,'sha256').hexdigest()
exe=a.run/'racer_native_probe.exe';assert sha(exe)==sha(a.binary),'Attribution map must use the exact executable'
d=a.binary.read_bytes();pe=struct.unpack_from('<I',d,60)[0];assert d[pe:pe+4]==b'PE\0\0'
op=pe+24;assert struct.unpack_from('<H',d,op)[0]==0x20b
base=struct.unpack_from('<Q',d,op+24)[0];maps=[]
for m in re.finditer(r'^\s+0001:[0-9A-Fa-f]+\s+(\S+)\s+([0-9A-Fa-f]{16})\s+f\s+(.+)$',a.map.read_text(),re.M):maps.append((int(m[2],16)-base,m[1],m[3]))
maps.sort();assert maps;starts=[m[0] for m in maps]
counts=collections.Counter();names=collections.Counter();records=[];overflow=0
for path in sorted(a.run.glob('env-sites-*.tsv')):
    rows=list(csv.DictReader(path.open(),delimiter='\t'))
    for r in rows:
        n=int(r['calls']);rva=int(r['caller_rva'],16)
        if r['name']=='<overflow>':overflow+=n;continue
        counts[rva,r['name']]+=n;names[r['name']]+=n
    records.append(dict(path=path.name,sha256=sha(path),sites=len(rows)-1))
assert records,'No attribution files found'
sites=[]
for (rva,name),calls in counts.most_common():
    i=bisect.bisect_right(starts,rva)-1
    near=dict(rva=hex(maps[i][0]),name=maps[i][1],object=maps[i][2],offset=rva-maps[i][0]) if i>=0 else None
    sites.append(dict(caller_rva=hex(rva),setting=name,calls=calls,nearest_link_symbol=near))
report=dict(run=str(a.run),binary_sha256=sha(exe),map_sha256=sha(a.map),files=records,total_recorded_calls=sum(counts.values()),overflow=overflow,
            names=names.most_common(),sites=sites,caveats=['Only calls through this executable getenv import pointer are observed; DLL-internal getenv is excluded.','Names and caller return addresses only; no values logged.','Counts aggregate the whole instrumented run, not a fixed race window, and do not measure exclusive cost.','Nearest link symbol is a lookup aid, not a proven function boundary.','Instrumentation adds per-call overhead; do not use this run for a performance claim.'])
with a.output.open('x') as f:json.dump(report,f,indent=2)
print(json.dumps(dict(total=report['total_recorded_calls'],overflow=overflow,names=report['names'][:12],sites=sites[:6]),indent=2))
