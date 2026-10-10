"""Attribute wall-sampled IPs; never equate sample frequency to CPU cost."""
from pathlib import Path
import argparse,bisect,collections,hashlib,json,re,struct
p=argparse.ArgumentParser();p.add_argument('evidence',type=Path);p.add_argument('--output',type=Path,required=True)
p.add_argument('--map',type=Path);p.add_argument('--mapped-binary',type=Path);a=p.parse_args()
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
class PE:
    def __init__(self,path):
        self.data=path.read_bytes();d=self.data;pe=struct.unpack_from('<I',d,60)[0];assert d[pe:pe+4]==b'PE\0\0'
        n=struct.unpack_from('<H',d,pe+6)[0];sz=struct.unpack_from('<H',d,pe+20)[0];op=pe+24
        magic=struct.unpack_from('<H',d,op)[0];assert magic==0x20b
        self.base=struct.unpack_from('<Q',d,op+24)[0];self.sections=[]
        self.directories=[struct.unpack_from('<II',d,op+112+i*8) for i in range(16)]
        for i in range(n):
            x=op+sz+40*i;name=d[x:x+8].rstrip(b'\0').decode();vs,va,rs,rp=struct.unpack_from('<IIII',d,x+8)
            self.sections.append(dict(name=name,virtual_size=vs,rva=va,raw_size=rs,raw_pointer=rp,sha256=hashlib.sha256(d[rp:rp+rs]).hexdigest()))
    def off(self,rva):
        for s in self.sections:
            if s['rva']<=rva<s['rva']+s['raw_size']:return s['raw_pointer']+rva-s['rva']
        raise ValueError(hex(rva))
    def exports(self):
        rva,size=self.directories[0]
        if not rva:return []
        x=self.off(rva);d=self.data;n=struct.unpack_from('<I',d,x+24)[0]
        funcs,names,ords=struct.unpack_from('<III',d,x+28);rows=[]
        for i in range(n):
            nr=struct.unpack_from('<I',d,self.off(names)+4*i)[0];ns=self.off(nr)
            name=d[ns:d.index(b'\0',ns)].decode();ordinal=struct.unpack_from('<H',d,self.off(ords)+2*i)[0]
            address=struct.unpack_from('<I',d,self.off(funcs)+4*ordinal)[0]
            if not rva<=address<rva+size:rows.append((address,name))
        return sorted(rows)
    def function_range(self,rva):
        start,size=self.directories[3]
        if not start:return None
        x=self.off(start)
        for at in range(x,x+size,12):
            begin,end,unwind=struct.unpack_from('<III',self.data,at)
            if begin<=rva<end:return dict(begin_rva=begin,end_rva=end,unwind_rva=unwind)
        return None
mods=json.loads((a.evidence/'modules.json').read_text());rows=[json.loads(s) for s in (a.evidence/'samples.jsonl').read_text().splitlines()]
manifest=json.loads((a.evidence/'observer.json').read_text());exe=Path(manifest['run'])/'racer_native_probe.exe'
assert sha(exe)==manifest['binary_sha256'].lower()
maps=[];mapcheck=None
if a.map:
    assert a.mapped_binary
    original=PE(exe);mapped=PE(a.mapped_binary)
    # Linker timestamps/import timestamps may differ in .rdata, but .text and
    # virtual layout must match exactly to use the map for instruction pointers.
    layout=lambda pe:[(s['name'],s['rva'],s['virtual_size']) for s in pe.sections]
    assert layout(original)==layout(mapped),'Section layout differs'
    ot=next(s for s in original.sections if s['name']=='.text');mt=next(s for s in mapped.sections if s['name']=='.text')
    assert ot==mt,'Instruction bytes differ'
    for m in re.finditer(r'^\s+0001:[0-9A-Fa-f]+\s+(\S+)\s+([0-9A-Fa-f]{16})\s+f\s+(.+)$',a.map.read_text(),re.M):
        maps.append((int(m[2],16)-mapped.base,m[1],m[3]))
    maps.sort();assert maps
    mapcheck=dict(map_sha256=sha(a.map),mapped_binary_sha256=sha(a.mapped_binary),original_binary_sha256=sha(exe),identical_text=ot,identical_virtual_section_layout=True)
cache={};threadrows=[]
for identity in sorted({(t['tid'],t['created_100ns']) for r in rows for t in r['threads']}):
    ts=[t for r in rows for t in r['threads'] if (t['tid'],t['created_100ns'])==identity]
    cpu=lambda t:t['user_100ns']+t['kernel_100ns']
    counts=collections.Counter(t['ip'] for t in ts);attribution=[];mc=collections.Counter()
    for ip,count in counts.most_common():
        mod=next((m for m in mods if m['base']<=ip<m['base']+m['bytes']),None)
        entry=dict(ip=hex(ip),samples=count,module=mod['name'] if mod else None)
        mc[entry['module'] or 'unmapped']+=count
        if mod:
            rva=ip-mod['base'];entry['rva']=hex(rva);path=Path(mod['path'])
            if str(path) not in cache:
                pe=PE(path);cache[str(path)]=(pe,pe.exports(),sha(path))
            pe,exports,digest=cache[str(path)];entry['module_sha256']=digest
            entry['unwind_function_range']=pe.function_range(rva)
            if mod['name'].lower()=='racer_native_probe.exe' and maps:
                candidates=[m for m in maps if m[0]<=rva]
                if candidates:
                    addr,name,obj=candidates[-1];entry['nearest_link_symbol']=dict(name=name,offset=rva-addr,object=obj)
            index=bisect.bisect_right(exports,(rva,chr(0x10ffff)))-1
            if index>=0:
                addr,name=exports[index];entry['nearest_export']=dict(name=name,offset=rva-addr)
                entry['bytes_at_export_hex']=pe.data[pe.off(addr):pe.off(addr)+32].hex()
        attribution.append(entry)
    threadrows.append(dict(tid=identity[0],created_100ns=identity[1],samples=len(ts),cpu_seconds=(cpu(ts[-1])-cpu(ts[0]))/1e7,modules=dict(mc),ips=attribution))
threadrows.sort(key=lambda t:t['cpu_seconds'],reverse=True)
report=dict(observer=manifest,terminal=json.loads((a.evidence/'end.json').read_text()),samples_sha256=sha(a.evidence/'samples.jsonl'),map_validation=mapcheck,threads=threadrows,
    caveats=['IP frequency is wall-sampled, not exclusive CPU weighting.','Nearest export or link symbol is not a verified function boundary. Large offsets must not be described as executing that exported function.','System modules were hashed during analysis; module inventory does not carry capture-time hashes.','Sampling perturbs the run and was not present in reference throughput measurements.'])
with a.output.open('x') as f:json.dump(report,f,indent=2)
print(json.dumps(dict(tid=threadrows[0]['tid'],cpu_seconds=threadrows[0]['cpu_seconds'],modules=threadrows[0]['modules'],map_validation=mapcheck),indent=2))
