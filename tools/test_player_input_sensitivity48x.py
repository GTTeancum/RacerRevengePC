#!/usr/bin/env python3
"""48X candidate-only semantic controls with unchanged original-word references."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output-dir',type=Path,required=True);a=ap.parse_args();out=a.output_dir.resolve()
 if out==ROOT or ROOT in out.parents:ap.error('Output must be outside source')
 out.mkdir(exist_ok=True,parents=True)
 if any(out.iterdir()):ap.error('Output must be empty')
 production=ROOT/'src/game/player_input48x.cpp';source=production.read_text()
 mutations={
  'wrong_authored_deadband':('fp(0x3e4ccccdu),fp(0xbe4ccccdu)','0.01f,-0.01f'),
  'wrong_packet_axis0':('float(sample.packet6),float(sample.packet7)','float(sample.packet4),float(sample.packet7)'),
  'missing_axis2_inversion':('f.sub(255.0f,float(sample.packet4))','float(sample.packet4)'),
  'strict_upper_deadband_edge':('if(!(value<p.upperDeadband))','if(value>p.upperDeadband)'),
  'strict_lower_deadband_edge':('else if(value<=p.lowerDeadband)','else if(value<p.lowerDeadband)'),
  'invent_axis_output_clamp':('out=value;return true;','out=value>1.0f?1.0f:(value < -1.0f?-1.0f:value);return true;'),
  'wrong_steering_priority':('query(15,match)','query(13,match)'),
  'ignore_action_sentinel':('codes.action694!=-1&&!query(codes.action694,match)','!query(codes.action694,match)'),
  'invent_flag_sentinel':('if(!query(codes.flag698,match))return false;','if(codes.flag698!=-1&&!query(codes.flag698,match))return false;'),
  'wrong_query_mask':('code,0x3fu','code,0x1u'),
  'wrong_state_gate':('(in.gateByte58&3u)==2u','(in.gateByte58&2u)!=0u'),
  'clear_unrelated_flags':('else result.flagsB9C=static_cast<std::uint8_t>(result.flagsB9C&~7u);','else result.flagsB9C=0;'),
  'ignore_positive_state_gate':('const bool flag1=match&&','const bool flag1=match||'),
  'reject_authored_endpoint':('const float limit=fp(0x3f800001u);','const float limit=1.0f;'),
  'omit_positive_cosine_reflection':('if(!(argument<=fp(0x3fc90fdbu)))','if(false)'),
  'wrong_cosine_reflection_sign':('sign=-1.0f','sign=1.0f'),
  'drop_connected_action':('binding.actionB7C=result.player.actionB7C;','binding.actionB7C=bindingContext.actionB7C;'),
  'drop_connected_flags':('binding.flagsB9C=result.player.flagsB9C;','binding.flagsB9C=bindingContext.flagsB9C;'),
  'swap_connected_aux_source':('binding.source7D4=playerContext.source7D4;','binding.source7D4=playerContext.source7D0;'),
 }
 rows=[]
 for name,(old,new) in mutations.items():
  count=2 if name=='wrong_cosine_reflection_sign' else 1
  if source.count(old)!=count:raise ValueError('Mutation anchor count: '+name)
  candidate=out/(name+'.cpp');candidate.write_text(source.replace(old,new));binary=out/name
  cmd=['g++','-std=c++17','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I',str(ROOT/'src/game'),str(ROOT/'tests/native/player_input48x.cpp')]+[str(ROOT/'src/game'/file) for file in ['control_filter48b.cpp','control_shaping48v.cpp','control_binding48w.cpp']]+[str(candidate),'-o',str(binary)]
  build=subprocess.run(cmd,capture_output=True,text=True,timeout=180);(out/(name+'-build.log')).write_text(build.stdout+build.stderr)
  row={'name':name,'compile_returncode':build.returncode,'detected':False}
  if build.returncode==0:
   run=subprocess.run([str(binary)],capture_output=True,text=True,timeout=120);row.update(returncode=run.returncode,output=run.stdout+run.stderr,detected=run.returncode==1 and 'player_input48x after' in run.stderr);(out/(name+'-test.log')).write_text(run.stdout+run.stderr)
  rows.append(row);print(name+': '+('detected' if row['detected'] else 'FAILED'),flush=True)
 if production.read_text()!=source:raise ValueError('Production changed during controls')
 result={'status':'passed' if all(r['detected'] for r in rows) else 'failed','production_sha256':hashlib.sha256(production.read_bytes()).hexdigest(),'controls':rows};(out/'sensitivity.json').write_text(json.dumps(result,indent=2)+'\n');return 0 if result['status']=='passed' else 1
if __name__=='__main__':raise SystemExit(main())
