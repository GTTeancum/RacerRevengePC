#!/usr/bin/env python3
"""Reject deliberate 48V semantic mistakes using unmodified original-word tests.
Only temporary copies are changed; production files are never edited.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output-dir',type=Path,required=True)
    args=ap.parse_args()
    out=args.output_dir.resolve()
    if out==ROOT or ROOT in out.parents:ap.error('Output must be outside source tree')
    out.mkdir(parents=True,exist_ok=True)
    if any(out.iterdir()):ap.error('Output directory must be empty')
    production=ROOT/'src/game/control_shaping48v.cpp'
    source=production.read_text()
    mutations={
        'bypass_all_nonlinear':('mode == ControlShapeMode48V::Nonlinear','false'),
        'wrong_cosine_coefficient':('0xbab60b61u','0xbab60b60u'),
        'host_cosine_substitution':('originalCosine(mul(fromBits(0x3fc90fdbu), value))','::cosf(mul(fromBits(0x3fc90fdbu), value))'),
        'skip_quantization':('result.targets.values[i] = mul(fromBits(0x3c23d70au), integerAsFloat);','result.targets.values[i] = shaped.values[i];'),
        'missing_positive_half_bias':('add(0.5f, scaled)','scaled'),
        'symmetric_round_instead_of_truncate':('static_cast<std::int32_t>(biased)','static_cast<std::int32_t>(::roundf(scaled))'),
        'history_weight_wrong':('0x3f666666u','0x3f000000u'),
        'D28_uses_quantized_input':('    out = result;\n    return true;\n}\n\nbool stepControlInputs48V','    result.smoothedChannel0D28 = add(mul(fromBits(0x3dcccccdu), result.targets.values[0]), history);\n    out = result;\n    return true;\n}\n\nbool stepControlInputs48V'),
        'skip_rate_limiter':('stepControlRateFilter48B(result.shaping.targets, previousFiltered, rates,\n                                 elapsedFactor, result.filtered)','((result.filtered = result.shaping.targets), true)'),
    }
    records=[]
    for label,(old,new) in mutations.items():
        if source.count(old)!=1:raise ValueError('Mutation anchor not unique: '+label)
        candidate=out/(label+'.cpp')
        candidate.write_text('#include <cmath>\n'+source.replace(old,new))
        binary=out/label
        command=['g++','-std=c++17','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off',
                 '-I',str(ROOT/'src/game'),str(ROOT/'tests/native/control_shaping48v.cpp'),
                 str(ROOT/'src/game/control_filter48b.cpp'),str(candidate),'-o',str(binary)]
        build=subprocess.run(command,capture_output=True,text=True,timeout=180)
        (out/(label+'-build.log')).write_text(build.stdout+build.stderr)
        record={'name':label,'compile_returncode':build.returncode,'detected':False}
        if build.returncode==0:
            run=subprocess.run([str(binary)],capture_output=True,text=True,timeout=120)
            record.update(returncode=run.returncode,output=run.stdout+run.stderr,
                          detected=run.returncode==1 and 'control_shaping48v after' in run.stderr)
            (out/(label+'-test.log')).write_text(run.stdout+run.stderr)
        records.append(record)
        print(label+': '+('detected' if record['detected'] else 'FAILED'),flush=True)
    if production.read_text()!=source:raise ValueError('Production changed during sensitivity run')
    result={'status':'passed' if all(r['detected'] for r in records) else 'failed',
            'production_sha256':hashlib.sha256(production.read_bytes()).hexdigest(),'controls':records}
    (out/'sensitivity.json').write_text(json.dumps(result,indent=2)+'\n')
    return 0 if result['status']=='passed' else 1
if __name__=='__main__':raise SystemExit(main())
