"""Freeze collision loader/placement evidence with exact ELF word verification."""
from pathlib import Path
import hashlib
import json
import re
import struct

ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/importCollision48G'
RANGES=[
 ('COL1_magic_version_validation',0x11ba74,0x11bafc),
 ('COL_type0_dispatch',0x124fa4,0x125014),
 ('COL1_triangle_tree_body_reader',0x125890,0x126418),
 ('collision_transform_dispatch_and_local_times_instance',0x11f680,0x11f84c),
 ('zone_static_count',0x275f40,0x275f74),
 ('collision_count_fallback_to_static_count',0x276420,0x27652c),
 ('collision_parameter_reader_COL_override_or_SLT',0x27a880,0x27aa60),
 ('collision_class_constructor_defaults',0x27afa0,0x27b03c),
 ('collision_create_flags_load_and_placement',0x27b040,0x27b2c8),
 ('standard_or_physics_class_selection',0x27bae0,0x27bb40),
 ('collision_model_list_uses_static_records_when_count_absent',0x27bca0,0x27be9c),
 ('collision_list_create_each_entry',0x27bea0,0x27bf50),
]
STRINGS=[(0x388958,'col'),(0x3b9e68,'SLT'),(0x3ba650,'NumberOfStaticModels'),
 (0x3ba870,'NumberOfCollisionObjects'),(0x3bb260,'COL'),(0x3bb318,'%s-Model%d'),
 (0x3bb350,'StaticType'),(0x3bb360,'Standard'),(0x3bb3b8,'PhysicsObject')]
DATA=[('collision_transform_type_dispatch',0x388a00,7),('collision_model_virtual_table',0x3d26e0,4),
 ('collision_parameter_virtual_table',0x3d2730,4)]
CARS=[('ObjectExport.cpp',[(326,349),(2421,2428),(2488,2542),(2826,2914),(2932,2951),(3064,3117)]),
 ('NewCollision.h',[(1,45)]),('NewCollision.cpp',[(5,67)]),('BoundingBoxTreeBuild.cpp',[(714,779)])]

def sha(b):return hashlib.sha256(b).hexdigest()

def main():
    if not __debug__:raise RuntimeError('Do not disable evidence assertions')
    elf_path=ROOT/'work/native/project/disc/SLUS_202.68';elf=elf_path.read_bytes()
    assert elf[:6]==b'\x7fELF\x01\x01'
    off=struct.unpack_from('<I',elf,28)[0];stride,count=struct.unpack_from('<HH',elf,42)
    segments=[struct.unpack_from('<8I',elf,off+i*stride) for i in range(count)]
    def at(va,n):
        for kind,o,base,_,size,*_ in segments:
            if kind==1 and base<=va and va+n<=base+size:return o+va-base
        raise ValueError(hex(va))
    words={};hashes={}
    for p in (ROOT/'work/native/project/generated').glob('*_0x*.cpp'):
        start=int(re.search(r'_0x([0-9a-f]+)\.cpp$',p.name)[1],16)
        if not any(low-0x1000<=start<high for _,low,high in RANGES):continue
        data=p.read_bytes();selected=False
        for line,text in enumerate(data.decode('utf8').splitlines(),1):
            m=re.match(r'\s*// (0x[0-9a-f]+): (0x[0-9a-f]+)\s+(.*)',text)
            if not m:continue
            va,word=int(m[1],16),int(m[2],16)
            if not any(low<=va<high for _,low,high in RANGES):continue
            offset=at(va,4);assert struct.unpack_from('<I',elf,offset)[0]==word,(str(p),line,hex(va))
            record=words.setdefault(va,dict(address=hex(va),file_offset=offset,word=f'{word:08x}',disassembly=m[3],original_word_verified=True,sources=[]))
            record['sources'].append(dict(path=p.relative_to(ROOT).as_posix(),line=line));selected=True
        if selected:hashes[p.relative_to(ROOT).as_posix()]=sha(data)
    commented=len(words);groups=[]
    for label,low,high in RANGES:
        group=[]
        for va in range(low,high,4):
            if va not in words:
                offset=at(va,4);word=struct.unpack_from('<I',elf,offset)[0]
                words[va]=dict(address=hex(va),file_offset=offset,word=f'{word:08x}',disassembly='nop' if not word else None,original_word_verified=True,sources=[])
            group.append(words[va])
        groups.append(dict(label=label,start=hex(low),end_exclusive=hex(high),instructions=group))
    strings=[]
    for va,value in STRINGS:
        o=at(va,len(value)+1);assert elf[o:o+len(value)+1]==value.encode()+b'\0'
        strings.append(dict(address=hex(va),text=value,file_offset=o))
    tables=[]
    for label,va,n in DATA:
        o=at(va,n*4);values=struct.unpack_from('<%dI'%n,elf,o)
        tables.append(dict(label=label,address=hex(va),file_offset=o,words=[hex(x) for x in values],sha256=sha(elf[o:o+n*4])))
    assert tables[0]['words'][0]=='0x11f6c8'
    excerpts=[]
    for name,spans in CARS:
        p=ROOT/'third_party/cars-tools-reference/CarsTools/MAX Plugins/ObjectExport'/name
        data=p.read_bytes();lines=data.decode('latin1').splitlines();hashes[p.relative_to(ROOT).as_posix()]=sha(data)
        excerpts.append(dict(path=p.relative_to(ROOT).as_posix(),excerpts=[dict(first_line=a,last_line=b,lines=[f'{i}: {lines[i-1]}' for i in range(a,b+1)]) for a,b in spans]))
    report=dict(schema='collision48G-original-source-proof-v1',original_elf=dict(path=elf_path.relative_to(ROOT).as_posix(),sha256=sha(elf)),
        instruction_count=len(words),generated_comments_verified=commented,groups=groups,strings=strings,data_tables=tables,source_sha256=hashes,cars_excerpts=excerpts,
        conclusions=[
            'Original game explicitly accepts COL1, not Cars default COL2. Material DWORD is conditional on ColVersion>1 and absent in original COL1.',
            'Original type0 body reads 12 affine floats, reverse flag, vertex/node/face counts, quantized flag, vertex array, optional vertex normals, 32-byte file nodes, 22-byte file faces.',
            'Original loader reads source float face normals then quantizes to signed short using32767 and0.5. Native records retain source float planes; no original-query precision claim.',
            'Original type0 setter computes COL local matrix times supplied instance matrix. All38 TA stored COL matrices happen to be identity.',
            'Collision creator normalizes Up and Look independently and takes Up cross Look; its t1=0 bypasses geometry look-orthogonalization. Source-derived scalar output is not VU RSQRT bit-exact.',
            'Missing NumberOfCollisionObjects falls back to static model count/list. Missing StaticType defaults Standard. Missing COL field falls back to SLT basename.',
            'Object material/category masks and user data belong to runtime bindings, not COL1 face records. No gameplay defaults are invented.'],
        limitations=['Generated instruction words are verified against ELF; disassembly text is retained, not re-disassembled.',
            'These anchors establish format/placement, not original ray/sweep traversal or hit tie-break semantics.'])
    OUT.mkdir(parents=True,exist_ok=True);p=OUT/'original-loader-proof.json';p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
    print(json.dumps(dict(instructions=len(words),generated_comments=commented,source_files=len(hashes),sha256=sha(p.read_bytes()))))

if __name__=='__main__':main()
