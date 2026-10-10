"""Read-only original constructor/remainder inventory for the native49Z join."""
from pathlib import Path
import argparse,hashlib,json,re,struct
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'work/local49Z/impact-consumers'
RANGES={'vtable-install':(0x20f3c8,0x20f3d8),'constructor-minus-one':(0x20f700,0x20f710),'constructor-timers':(0x20fa90,0x20fb60),'constructor-maxima':(0x2103a8,0x210418),'partner-initialization':(0x212e3c,0x212e80),'damage-reset':(0x217420,0x21743c),'damage-reset-caller':(0x216740,0x216764),'vtable':(0x3d1790,0x3d183c),'base-remainder':(0x2288bc,0x228b90),'repair224370':(0x224370,0x22471c),'effects223540':(0x223540,0x224170),'transition225c50':(0x225c50,0x2261b0),'optional2179d0':(0x2179d0,0x217de0),'extra-model-getters':(0x212ed0,0x212fd0)}
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--elf',type=Path,required=True);ap.add_argument('--generated',type=Path,required=True);a=ap.parse_args()
    data=a.elf.read_bytes();assert hashlib.sha256(data).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    annotations={}
    for prefix in ['20f','210','212','2167','2179','217a','217b','217c','217d','223','224','225','226','228','229','22a']:
        for p in a.generated.glob('*0x'+prefix+'*.cpp'):
            for match in re.finditer(r'// 0x([0-9a-fA-F]+): 0x([0-9a-fA-F]+)\s+([^\r\n]+)',p.read_text()):
                pc=int(match[1],16);w=int(match[2],16)
                if any(lo<=pc<hi for lo,hi in RANGES.values()):
                    assert struct.unpack_from('<I',data,pc-0x100000+0x80)[0]==w
                    annotations[pc]=match[3]
    blocks=[]
    for name,(lo,hi) in RANGES.items():
        raw=data[lo-0x100000+0x80:hi-0x100000+0x80]
        rows=[];calls=[];stores=[]
        for pc in range(lo,hi,4):
            w=struct.unpack_from('<I',raw,pc-lo)[0];row=dict(pc=hex(pc),word=f'{w:08x}',annotation=annotations.get(pc,''));rows.append(row)
            if w>>26==3:calls.append(dict(pc=hex(pc),target=hex((w&0x3ffffff)*4)))
            if w>>26 in (0x28,0x29,0x2b,0x39,0x1f,0x3e):stores.append(dict(pc=hex(pc),base_register=(w>>21)&31,offset=hex(w&65535),word=f'{w:08x}',annotation=annotations.get(pc,'')))
        blocks.append(dict(name=name,start=hex(lo),end=hex(hi),sha256=hashlib.sha256(raw).hexdigest(),instructions=rows,direct_calls=calls,stores=stores))
    (OUT/'integration-source-audit.json').write_text(json.dumps(dict(elf_sha256=hashlib.sha256(data).hexdigest(),blocks=blocks),indent=2)+'\n')
    for b in blocks:
        if b['name']=='vtable':continue
        (OUT/(b['name']+'.txt')).write_text('\n'.join(r['pc']+' '+r['word']+' '+r['annotation'] for r in b['instructions'])+'\n')
    report=json.loads((OUT/'fixtures.json').read_text());branches=report['branches']
    one={p:v for p,v in branches.items() if len(v)==1};print(json.dumps(dict(source_blocks=len(blocks),branches=len(branches),two_sided=sum(len(v)==2 for v in branches.values()),one_sided=one),indent=2))
if __name__=='__main__':main()
