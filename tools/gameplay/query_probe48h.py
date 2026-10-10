"""Replay original query words and preserve only snapshot data bytes read."""
from query_extract48h import RAM, digest
from query_oracle48h import *

def execute(data,pc,candidate=None,allowed=None):
    m=Original(data);m.allowed_source_addresses=allowed;m.g[28][0]=0x3dd970
    m.g[4][0]=0xd113f0
    if pc==0x121410:
        # Isolated narrow phase: caller's result initialization.
        m.write(0x3df5b8,0x9512e0);m.storef(0x951354,1);m.write(0x951340,0)
        m.g[5][0]=candidate
    else:
        # 122660 itself performs original reset, filters, tree and callbacks.
        m.g[5][0]=m.read(0xc5b538);m.g[6][0]=0xc5b53c
    hit=m.run(pc)
    addresses=[0x951310+i*4 for i in range(3)]+[0x951300+i*4 for i in range(3)]+[0x951354]
    words=[m.read(a) for a in addresses]
    expected=[struct.unpack_from('<I',data,a)[0] for a in addresses]
    if hit:assert words==expected,(words,expected)
    result={'entry':f'{pc:08X}','candidate':f'{candidate:08X}' if candidate else None,'return':hit,'steps':m.steps,
            'unique_words':len(m.coverage),'output_words':[f'{w:08X}' for w in words],
            'point':m.vec(0x951310),'normal':m.vec(0x951300),'fraction':m.loadf(0x951354),
            'matches_saved_contact':words==expected if hit else None,
            'query_hit_flag':m.read(0xd113f0+0xf8,1),
            'executed_pcs':[f'{a:08X}' for a in sorted(m.coverage)]}
    return m,result

def main():
    saved=RAM.read_bytes();assert digest(saved)=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
    inputs=[(0x121410,0x9e7d30),(0x121410,0x9f7350),(0x122660,None)]
    runs=[];addresses=set()
    for pc,candidate in inputs:
        m,r=execute(saved,pc,candidate);runs.append(r);addresses.update(m.source_reads)
    for base,n in [(0x951300,12),(0x951310,12),(0x951354,4)]:addresses.update(range(base,base+n))
    spans=[]
    for a in sorted(addresses):
        if not spans or a!=spans[-1][1]:spans.append([a,a+1])
        else:spans[-1][1]+=1
    raw=bytearray();records=[];reconstructed=bytearray(len(saved))
    for lo,hi in spans:
        b=saved[lo:hi];records.append({'address':lo,'bytes':len(b),'file_offset':len(raw),'sha256':digest(b)})
        raw.extend(b);reconstructed[lo:hi]=b
    for (pc,candidate),expected in zip(inputs,runs):
        m,result=execute(reconstructed,pc,candidate,addresses)
        assert result==expected
    (OUT/'saved-query-data.bin').write_bytes(raw)
    (OUT/'saved-query-data.json').write_text(json.dumps({'source':RAM.relative_to(ROOT).as_posix(),'source_sha256':digest(saved),'raw_sha256':digest(raw),'slices':records},indent=2)+'\n')
    (OUT/'saved-query-replay.json').write_text(json.dumps({'scope':'Original-word offline execution: static mesh narrowphase and original limited-candidate122660 path. Not an entire simulation tick.','gp_source':'47W native.log:24076 gp=0x3dd970','data_bytes_preserved':len(raw),'data_slices':len(records),'sparse_fixture_replay':'PASS: every original-data read had to belong to a preserved byte address','runs':runs,'prequery_blocker':'Full1B1800 invokes120230 spatial-index update through1202E0/280D40. This proof does not suppress or execute that subsystem; it uses the saved query matrix/endpoints.'},indent=2)+'\n')
    print(json.dumps({'saved_replays':len(runs),'exact_saved_hits':sum(r['matches_saved_contact'] is True for r in runs),'preserved_data_bytes':len(raw),'preserved_data_slices':len(records),'sparse_replay':'PASS'}))

if __name__=='__main__':main()
