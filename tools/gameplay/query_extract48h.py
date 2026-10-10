"""Bounded immutable-ELF terrain-query recovery. Does not execute the game."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'evidence/query48H'
ELF = ROOT / 'work/native/project/disc/SLUS_202.68'
RAM = ROOT / 'work/native-run47w-1-1-1-1-1-0-1-1-1-1-20260930-181624-483/ee.bin'
RANGES = [
    (0x106790, 0x1067b0), (0x1067b0, 0x106b50),
    (0x106e70, 0x106ed0), (0x107140, 0x1089f0),
    (0x1089f0, 0x108cf8), (0x10b840, 0x10e080),
    (0x112d30, 0x113b40), (0x11c3f0, 0x11c710),
    (0x11dd00, 0x11e630), (0x11f680, 0x120290),
    (0x120e70, 0x121290), (0x121410, 0x122350),
    (0x124f70, 0x124f80), (0x126eb0, 0x126f40),
    (0x122660, 0x122ba4), (0x1b0e50, 0x1b0ed8),
    (0x1b1240, 0x1b14b0), (0x1b1800, 0x1b1a30),
]

def digest(data): return hashlib.sha256(data).hexdigest()

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--show', nargs=2, type=lambda s: int(s, 16))
    args = ap.parse_args()
    data = ELF.read_bytes()
    assert digest(data) == 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    sources = {}
    for f in (ROOT/'work/native/project/generated').glob('*.cpp'):
        m = re.search(r'_0x([0-9a-f]+)\.cpp$', f.name)
        if not m or not any(lo-0x400 <= int(m[1],16)<hi for lo,hi in RANGES): continue
        for n,line in enumerate(f.read_text().splitlines(),1):
            m = re.match(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+(.*)',line)
            if not m: continue
            a,w = int(m[1],16),int(m[2],16)
            if not any(lo <= a < hi for lo,hi in RANGES): continue
            assert struct.unpack_from('<I', data, a-0x100000+0x80)[0] == w, (f,n,hex(a))
            sources.setdefault(a, {'file':f.relative_to(ROOT).as_posix(),'line':n,'asm':m[3]})
    records=[]
    for lo,hi in RANGES:
        raw=data[lo-0x100000+0x80:hi-0x100000+0x80]
        words=[]
        for a in range(lo,hi,4):
            w=struct.unpack_from('<I',data,a-0x100000+0x80)[0]
            words.append({'pc':a,'word':w,**sources.get(a,{})})
        records.append({'start':lo,'end_exclusive':hi,'sha256':digest(raw),'words':words})
    OUT.mkdir(parents=True,exist_ok=True)
    result={'format':'original-query-words48h-v1','elf':{'path':ELF.relative_to(ROOT).as_posix(),'sha256':digest(data)},'address_mapping':'ELF file offset = VA - 0x100000 + 0x80','ranges':records}
    (OUT/'original-words.json').write_text(json.dumps(result,indent=2)+'\n')
    lines=[]
    for r in records:
        lines.append(f"\n# {r['start']:08X}..{r['end_exclusive']:08X} sha256={r['sha256']}")
        for w in r['words']:
            lines.append(f"{w['pc']:08X} {w['word']:08X} {w.get('asm','[no generated annotation]')} | {w.get('file','')}:{w.get('line','')}")
    (OUT/'source-anchors.txt').write_text('\n'.join(lines)+'\n')
    if args.show:
        for r in records:
            for w in r['words']:
                if args.show[0]<=w['pc']<args.show[1]:print(f"{w['pc']:08X} {w['word']:08X} {w.get('asm','')}")
    else: print(json.dumps({'ranges':len(records),'words':sum(len(r['words']) for r in records),'annotated_words':len(sources),'elf_sha256':digest(data)}))

if __name__ == '__main__': main()
