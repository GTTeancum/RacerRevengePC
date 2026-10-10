#!/usr/bin/env python3
"""Verify frozen original words and run bounded native box-aggregate tests."""
import hashlib,json,os,re,struct,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
EVIDENCE=ROOT/'evidence/authoredGeometry49O/bounds'
ELF=ROOT/'work/native/project/disc/SLUS_202.68'
def main():
    raw=ELF.read_bytes();manifest=json.loads((EVIDENCE/'bounds-manifest.json').read_text())
    assert hashlib.sha256(raw).hexdigest()==manifest['elf_sha256']
    phoff=struct.unpack_from('<I',raw,28)[0];size,count=struct.unpack_from('<HH',raw,42)
    segments=[struct.unpack_from('<8I',raw,phoff+i*size) for i in range(count)]
    frozen=(EVIDENCE/'bounds_words.h').read_text()
    for block in manifest['blocks']:
        a,z=int(block['start'],16),int(block['end_exclusive'],16)
        for kind,off,va,_,filesz,_,_,_ in segments:
            if kind==1 and va<=a<z<=va+filesz:
                data=raw[off+a-va:off+z-va];break
        else:raise AssertionError('non-file-backed source range')
        assert hashlib.sha256(data).hexdigest()==block['sha256']
        match=re.search(r'\b'+block['name']+r'\[\]=\{(.*?)\};',frozen,re.S);assert match
        words=[int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)u',match[1])]
        assert struct.pack('<%dI'%len(words),*words)==data
    motion_path=EVIDENCE.parent/'motion/motion-output.json'
    motion_bytes=motion_path.read_bytes();motion=json.loads(motion_bytes)
    header=(EVIDENCE/'pod01_live_matrices.h').read_text()
    assert hashlib.sha256(motion_bytes).hexdigest()==re.search(r'motionJsonSha256\[\]="([0-9a-f]+)"',header)[1]
    frozen_live=[int(w,16) for w in re.findall(r'0x([0-9a-f]+)u',header)]
    assert [node['index'] for node in motion['nodes']]==list(range(18))
    assert frozen_live==[int(word,16) for node in motion['nodes'] for word in node['local_matrix_bits']]
    fixture=ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg'
    assert hashlib.sha256(fixture.read_bytes()).hexdigest()=='203232e291b5098ddfeb437d59748105f33a4432bad1a9359d96681fc5d03b59'
    with tempfile.TemporaryDirectory(prefix='rr-bounds49o-') as folder:
        exe=Path(folder)/'test'
        command=[os.environ.get('CXX','g++'),'-std=c++20','-O2','-Wall','-Wextra','-Werror','-pedantic','-ffp-contract=off','-fno-fast-math',str(ROOT/'tests/native/model_bounds49o.cpp'),str(ROOT/'src/game/model_bounds49o.cpp'),'-o',str(exe)]
        subprocess.run(command,check=True,cwd=ROOT)
        environment=dict(os.environ);environment['RR_BOUNDS49O_COVERAGE']=str(EVIDENCE/'coverage.json')
        result=subprocess.run([str(exe),str(fixture)],check=True,cwd=ROOT,text=True,capture_output=True,env=environment)
        mutations=[('leaf_extent_add_to_sub','28de28','46000801'),('point_remove_root_translation','28dc98','4bc8a528'),('radius_madd_to_msub','26f804','4600001d'),('extent_half_product_to_sum','26f7c4','46001800'),('invert_node_eligibility','28db30','1460014f'),('leaf_min_to_max','28df34','46010228')]
        sensitivity=[]
        for name,pc,word in mutations:
            r=subprocess.run([str(exe),str(fixture),pc,word],cwd=ROOT,text=True,capture_output=True)
            assert r.returncode!=0,name
            sensitivity.append({'name':name,'pc':pc,'replacement_word':word,'detected':True,'diagnostic':r.stderr.strip()})
    report={'status':'pass','native_result':result.stdout.strip(),'original_word_hashes_verified':len(manifest['blocks']),'native_hot_allocations':0,'sensitivity':sensitivity,'scope':'Source-ready bounded box aggregation and fresh Pod01 post-motion matrices; motion numeric provenance is a separate linked proof.'}
    (EVIDENCE/'native-test-result.json').write_text(json.dumps(report,indent=2)+'\n')
    print(result.stdout.strip());print('Verified original ranges:',len(manifest['blocks']),'sensitivity:',len(sensitivity),'/',len(mutations))
if __name__=='__main__':main()
