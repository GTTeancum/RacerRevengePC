"""Audit full-disc COL nonfinite source fields without repairing original data."""
from pathlib import Path
from collections import Counter
import hashlib
import json
import math
import struct
from decode_scene48c import load_container

ROOT=Path(__file__).resolve().parents[2]

def main():
    out=ROOT/'evidence/importCollision48G/nonfinite-originals';out.mkdir(parents=True,exist_ok=True)
    counts=Counter();bad=[]
    for path in sorted((ROOT/'work/native/project/disc').rglob('*.RES')):
        members,container=load_container(path);counts['containers']+=1
        for name,(data,record) in members.items():
            if not name.endswith('.col'):continue
            counts['members']+=1
            if len(data)<84 or data[:16]!=struct.pack('<I4sII',1234,b'col\0',1,0):
                counts['other_header']+=1;continue
            reverse,nv,nn,nf,q=struct.unpack_from('<5I',data,64)
            if q:counts['quantized']+=1;continue
            offset=84+12*nv
            assert offset+4<=len(data),(path,name,'truncated vertices')
            normals,=struct.unpack_from('<I',data,offset);offset+=4
            assert normals in (0,1) and offset+12*nv*normals+32*nn+22*nf==len(data),(path,name,'extent')
            regions=[('matrix',16,12),('vertices',84,3*nv)]
            if normals:regions.append(('vertex_normals',offset,3*nv));offset+=12*nv
            regions.extend((f'node{i}',offset+32*i,7) for i in range(nn));offset+=32*nn
            regions.extend((f'face{i}',offset+22*i,4) for i in range(nf))
            fields=[]
            for label,start,n in regions:
                for i in range(n):
                    value,=struct.unpack_from('<f',data,start+4*i)
                    if not math.isfinite(value):fields.append(dict(record=label,field=i,offset=start+4*i,bits=f'{struct.unpack_from("<I",data,start+4*i)[0]:08x}'))
            counts['float_type0']+=1
            if not fields:continue
            source_sha=hashlib.sha256(data).hexdigest();fixture=out/(source_sha+'.col');fixture.write_bytes(data);assert fixture.read_bytes()==data
            triangles=[]
            for face in sorted({int(x['record'][4:]) for x in fields if x['record'].startswith('face')}):
                at=offset+22*face;indices=struct.unpack_from('<3H',data,at+16)
                vertices=[struct.unpack_from('<3f',data,84+12*i) for i in indices]
                a,b,c=vertices;e=[b[i]-a[i] for i in range(3)];f=[c[i]-a[i] for i in range(3)]
                cross=[e[1]*f[2]-e[2]*f[1],e[2]*f[0]-e[0]*f[2],e[0]*f[1]-e[1]*f[0]]
                triangles.append(dict(face=face,source_offset=at,indices=indices,vertices=vertices,cross=cross,plane_bits=[f'{x:08x}' for x in struct.unpack_from('<4I',data,at)]))
            bad.append(dict(container=path.relative_to(ROOT).as_posix(),container_sha256=container['sha256'],member=record['name'],logical_offset=record['logical_offset'],source_bytes=len(data),source_sha256=source_sha,fixture=fixture.relative_to(ROOT).as_posix(),fields=fields,triangles=triangles))
    report=dict(schema='collision48G-original-nonfinite-audit-v1',counts=dict(counts),nonfinite_members=bad)
    (out/'audit.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(counts=dict(counts),nonfinite_occurrences=len(bad),nonfinite_unique=len({x['source_sha256'] for x in bad}),members=[(x['container'],x['member']) for x in bad])))

if __name__=='__main__':main()
