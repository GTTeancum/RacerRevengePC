#!/usr/bin/env python3
"""Measure new49M record sizes, strict-C++17 object sections and compiler-reported bounded frames."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SOURCES=('gradient_noise49m','secondary_body49m','coupled_body_advancement49m')
FLAGS=['-std=c++17','-O2','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off','-fstack-usage']
def run(args):
    p=subprocess.run(args,cwd=ROOT,text=True,capture_output=True)
    if p.returncode:raise RuntimeError(str(args)+'\n'+p.stdout+p.stderr)
    return p.stdout,p.stderr

def main():
    p=argparse.ArgumentParser();p.add_argument('--build-dir',type=Path,required=True);a=p.parse_args()
    b=a.build_dir.resolve();assert not b.exists();b.mkdir(parents=True)
    e=ROOT/'evidence/secondaryWrapper49M/footprint';e.mkdir(parents=True,exist_ok=True)
    objects={};sizes=[];frames=[]
    for name in SOURCES:
        source=ROOT/'src/game'/f'{name}.cpp';obj=b/f'{name}.o'
        _,stderr=run(['g++',*FLAGS,'-c',str(source),'-o',str(obj)])
        assert not stderr,stderr
        size,_=run(['size','-B',str(obj)]);sizes.append(size)
        row=size.splitlines()[-1].split()
        objects[name]={'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'object_sha256':hashlib.sha256(obj.read_bytes()).hexdigest(),'text':int(row[0]),'data':int(row[1]),'bss':int(row[2])}
        su=(b/f'{name}.su').read_text();(e/f'{name}.su').write_text(su)
        for line in su.splitlines():
            fields=line.split('\t');frames.append({'function':fields[0],'bytes':int(fields[1]),'kind':fields[2]})
    code='#include "coupled_body_advancement49m.h"\n#include <iostream>\nusing namespace rr::game;\nint main(){\n'
    types=['GradientNoiseState49M','SecondaryBodyState49M','SecondaryBodyParameters49M','SecondaryForceContext49M','CoupledBodyState49M','CoupledBodyParameters49M','CoupledComponents49M','CoupledNoiseContext49M','PostBase49M','PostWorld49M','PostResponse354_49M','SecondaryInputs49M']
    for typ in types:code+=f'std::cout<<"{typ} "<<sizeof({typ})<<"\\n";\n'
    code+='}\n';(b/'sizes.cpp').write_text(code)
    run(['g++',*FLAGS,'-I',str(ROOT/'src/game'),str(b/'sizes.cpp'),'-o',str(b/'sizes')])
    output,_=run([str(b/'sizes')]);records={s.split()[0]:int(s.split()[1]) for s in output.splitlines()}
    (e/'record-sizes.cpp').write_text(code);(e/'record-sizes.stdout').write_text(output)
    (e/'gnu-size.txt').write_text(''.join(sizes))
    result={'status':'measured','compiler':run(['g++','--version'])[0].splitlines()[0],'flags':FLAGS,'objects':objects,'record_bytes':records,'sum_new_text_bytes':sum(r['text'] for r in objects.values()),'sum_new_data_bytes':sum(r['data'] for r in objects.values()),'sum_new_bss_bytes':sum(r['bss'] for r in objects.values()),'maximum_individual_compiler_frame_bytes':max(r['bytes'] for r in frames),'frames':frames,'limits':['Object sections are host compiler measurements, not target console link output.','Frames are per-function compiler estimates, including dynamic,bounded call-argument areas; nested calls add to the live stack.','No whole-game resident-memory or gameplay-FPS claim.']}
    (e/'summary.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
