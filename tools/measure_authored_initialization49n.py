#!/usr/bin/env python3
"""Measure strict-C++17 authored parameter code and caller-owned records."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SOURCES=['assets/pod_physics49n','game/authored_parameters49n']
FLAGS=['-std=c++17','-O2','-Wall','-Wextra','-Wpedantic','-Werror','-fno-fast-math','-frounding-math','-ffp-contract=off','-fstack-usage']
def run(args):
    p=subprocess.run(args,cwd=ROOT,text=True,capture_output=True)
    if p.returncode:raise RuntimeError(str(args)+'\n'+p.stdout+p.stderr)
    return p.stdout,p.stderr

def main():
    p=argparse.ArgumentParser();p.add_argument('--build-dir',type=Path,required=True);a=p.parse_args()
    b=a.build_dir.resolve();assert not b.exists();b.mkdir(parents=True)
    e=ROOT/'evidence/authoredInitialization49N/footprint';e.mkdir(parents=True,exist_ok=True)
    objects={};sizes=[];frames=[]
    for name in SOURCES:
        source=ROOT/'src'/f'{name}.cpp';stem=Path(name).name;obj=b/f'{stem}.o'
        _,stderr=run(['g++',*FLAGS,'-c',str(source),'-o',str(obj)]);assert not stderr,stderr
        size,_=run(['size','-B',str(obj)]);sizes.append(size);row=size.splitlines()[-1].split()
        detailed,_=run(['size','-A',str(obj)]);(e/f'{stem}-sections.txt').write_text(detailed)
        objects[name]={'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'object_sha256':hashlib.sha256(obj.read_bytes()).hexdigest(),'text':int(row[0]),'data':int(row[1]),'bss':int(row[2])}
        su=(b/f'{stem}.su').read_text();(e/f'{stem}.su').write_text(su)
        for line in su.splitlines():
            f=line.split('\t');frames.append({'function':f[0],'bytes':int(f[1]),'kind':f[2]})
    types=['AuthoredValue49N','AuthoredParameters49N','AuthoredSelectionContext49N','AuthoredBinding49N','PodPhysicsProvenance49N','PodPhysicsDiagnostic49N','CoupledBodyParameters49M','CoupledBodyState49M','CoupledNoiseContext49M']
    code='#include "pod_physics49n.h"\n#include <iostream>\nusing namespace rr::game;using namespace rr::assets;\nint main(){\n'
    for typ in types:code+=f'std::cout<<"{typ} "<<sizeof({typ})<<"\\n";\n'
    code+='}\n';(b/'sizes.cpp').write_text(code)
    run(['g++',*FLAGS,'-I',str(ROOT/'src/assets'),str(b/'sizes.cpp'),'-o',str(b/'sizes')])
    output,_=run([str(b/'sizes')]);records={s.split()[0]:int(s.split()[1]) for s in output.splitlines()}
    (e/'record-sizes.cpp').write_text(code);(e/'record-sizes.stdout').write_text(output);(e/'gnu-size.txt').write_text(''.join(sizes))
    result={'status':'measured','compiler':run(['g++','--version'])[0].splitlines()[0],'flags':FLAGS,'objects':objects,'record_bytes':records,'sum_new_text_bytes':sum(r['text'] for r in objects.values()),'sum_new_data_bytes':sum(r['data'] for r in objects.values()),'sum_new_bss_bytes':sum(r['bss'] for r in objects.values()),'maximum_individual_compiler_frame_bytes':max(r['bytes'] for r in frames),'frames':frames,'limits':['Object sections are host compiler measurements, not target console output.','Data includes constant schema tables requiring pointer relocations; see individual sections.','Frames are individual compiler estimates; nested calls add to live stack.','Caller retains original PHY input bytes separately; no whole-game footprint or FPS claim.']}
    (e/'summary.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
