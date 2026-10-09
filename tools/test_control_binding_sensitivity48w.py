#!/usr/bin/env python3
"""Independent semantic controls for48W: candidate-only mutations, unchanged oracle."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output-dir',type=Path,required=True);args=ap.parse_args()
    out=args.output_dir.resolve()
    if out==ROOT or ROOT in out.parents:ap.error('Output must be outside source tree')
    out.mkdir(exist_ok=True,parents=True)
    if any(out.iterdir()):ap.error('Output must be empty')
    production=ROOT/'src/game/control_binding48w.cpp';source=production.read_text()
    mutations={
     'ignore_mode_selector':('!in.mode58Nonzero&&in.configurationEntryNonzero','in.configurationEntryNonzero'),
     'ignore_configuration_selector':('!in.mode58Nonzero&&in.configurationEntryNonzero','!in.mode58Nonzero'),
     'swap_selected_channels':('const float left=in.filtered.values[1],right=in.filtered.values[3];','const float left=in.filtered.values[3],right=in.filtered.values[1];'),
     'clear_retained_EC':('// Original leaves EC and auxiliary50 untouched on this route.','result.aeroBrakeEC=0.0f; // Deliberate semantic mutation'),
     'clear_retained_aux50':('// Original leaves EC and auxiliary50 untouched on this route.','result.aeroAux50=0.0f; // Deliberate semantic mutation'),
     'strict_dual_threshold':('left>=0.9f&&right>=0.9f','left>0.9f&&right>0.9f'),
     'wrong_state_bias_side':('if(state==1)result.aeroDrive[1]=f.add(p.bias4346C0,result.aeroDrive[1]);','if(state==1)result.aeroDrive[0]=f.add(p.bias4346C0,result.aeroDrive[0]);'),
     'equal_speed_scale_one':('float scale=0.0f;','float scale=1.0f;'),
     'wrong_cached_speed':('f.sub(in.bodyCachedSpeed34,p.speed1B84)','f.sub(0.0f,p.speed1B84)'),
     'wrong_negative_steering_engine':('result.engineDrive[0]=f.mul(result.engineDrive[0],steering);','result.engineDrive[1]=f.mul(result.engineDrive[1],steering);'),
     'skip_brake_floor':('result.aeroDrive[0]=detail::scalarMax48O(result.aeroDrive[0],0.5f);','result.aeroDrive[0]=result.aeroDrive[0];'),
     'arithmetic_shift_instead_of_logical':('result.aeroTriggerD8=(in.flagsB9C&4u)?1:0;','result.aeroTriggerD8=(in.flagsB9C&4u)?-1:0;'),
     'swap_auxiliary_sources':('result.aeroAuxF0=in.source7D4;','result.aeroAuxF0=in.source7D0;'),
     'adapter_swap_engine':('engine.drive[0]=bindings.engineDrive[0];','engine.drive[0]=bindings.engineDrive[1];'),
     'drop_connected_new_filter':('auto input=context;input.filtered=result.control.filtered;','auto input=context;'),
     'remove_rounding_operand_barriers':('volatile float left=a,right=b;','const float left=a,right=b;'),
    }
    records=[]
    for label,(old,new) in mutations.items():
        expectedCount={'arithmetic_shift_instead_of_logical':2,'remove_rounding_operand_barriers':4}.get(label,1)
        if source.count(old)!=expectedCount:raise ValueError('Mutation anchor count: '+label)
        candidate=out/(label+'.cpp');candidate.write_text(source.replace(old,new));binary=out/label
        command=['g++','-std=c++17','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I',str(ROOT/'src/game'),
                 str(ROOT/'tests/native/control_binding48w.cpp'),str(ROOT/'src/game/control_filter48b.cpp'),
                 str(ROOT/'src/game/control_shaping48v.cpp'),str(candidate),'-o',str(binary)]
        build=subprocess.run(command,capture_output=True,text=True,timeout=180);(out/(label+'-build.log')).write_text(build.stdout+build.stderr)
        row={'name':label,'compile_returncode':build.returncode,'detected':False}
        if build.returncode==0:
            run=subprocess.run([str(binary)],capture_output=True,text=True,timeout=120)
            row.update(returncode=run.returncode,output=run.stdout+run.stderr,detected=run.returncode==1 and 'control_binding48w after' in run.stderr)
            (out/(label+'-test.log')).write_text(run.stdout+run.stderr)
        records.append(row);print(label+': '+('detected' if row['detected'] else 'FAILED'),flush=True)
    if production.read_text()!=source:raise ValueError('Production changed during sensitivity')
    result={'status':'passed' if all(r['detected'] for r in records) else 'failed','production_sha256':hashlib.sha256(production.read_bytes()).hexdigest(),'controls':records}
    (out/'sensitivity.json').write_text(json.dumps(result,indent=2)+'\n');return 0 if result['status']=='passed' else 1
if __name__=='__main__':raise SystemExit(main())
