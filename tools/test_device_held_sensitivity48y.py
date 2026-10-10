#!/usr/bin/env python3
"""Candidate-only48Y semantic controls; original-word references stay unchanged."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output-dir',type=Path,required=True);a=ap.parse_args();out=a.output_dir.resolve()
 if out==ROOT or ROOT in out.parents:ap.error('Output must be outside source tree')
 out.mkdir(exist_ok=True,parents=True)
 if any(out.iterdir()):ap.error('Output must be empty')
 production=ROOT/'src/game/device_held48y.cpp';source=production.read_text()
 mutations={
 'held_nonzero_instead_of_one':('record.held==1u','record.held!=0u'),
 'drop_record_payload_copy':('if(hit&&copyOut)std::memcpy(copyOut,&record,sizeof record);','if(hit&&copyOut)copyOut->held=record.held;'),
 'no_context_reject_zero':('mask==0u||mask==0x80000000u||mask==0x3fu','mask==0x80000000u||mask==0x3fu'),
 'skip_root_mask80_gate':('if(!rootBit&&(mask&0x80u))return false;','if(false)return false;'),
 'wrong_special_context_test':('context.context518==1u','context.context518!=0u'),
 'wrong_exact_context_mask':('(context.context1898|0x40u)==mask','(context.context1898&mask)!=0u'),
 'wrong_empty_context_mask':('return context.context1898==0u;','return context.context1898!=0u;'),
 'pressure_threshold_off_by_one':('if(value>=26u)','if(value>=25u)'),
 'missing_pressure_plus_one':('static_cast<float>(value+1u)','static_cast<float>(value)'),
 'pressure_uses_wrong_flag':('if(descriptor.flags&2u)','if(descriptor.flags&1u)'),
 'pressure_falls_back_to_digital':('record.value=pressure;record.held=1;','record.value=pressure;record.held=1;\n            } else if(active&descriptor.digitalMask) {record.value=1.0f;record.held=1;'),
 'reverse_digital_packet_bytes':('(std::uint32_t(packet.bytes[2])<<8)|packet.bytes[3]','(std::uint32_t(packet.bytes[3])<<8)|packet.bytes[2]'),
 'clear_inactive_records':('for(unsigned i=0;i<count;++i){result.buttons.records[i].held=0;','for(unsigned i=0;i<32;++i){result.buttons.records[i].held=0;'),
 'failure_preserves_old_bank':('if(!readSucceeded||packet.bytes[0]!=0){std::memcpy(&out,&result,sizeof result);return true;}','if(!readSucceeded||packet.bytes[0]!=0){result.buttons=previous;std::memcpy(&out,&result,sizeof result);return true;}'),
 'emit_unchanged_transitions':('if(record.held==previous.records[i].held)continue;','if(false)continue;'),
 'wrong_release_reset':('record.auxiliary10=0;record.auxiliary08=0;','record.auxiliary14=0;record.auxiliary08=0;'),
 'wrong_press_reset':('record.auxiliary14=0;record.auxiliary0C=0;','record.auxiliary14=0;record.auxiliary08=0;'),
 'wrong_event_category':('{i,2u,deviceId,record.held,0x3fu}','{i,1u,deviceId,record.held,0x3fu}'),
 'query_previous_frame':('DeviceHeldProvider48Y provider{&result.device.buttons,queryContext};','DeviceHeldProvider48Y provider{&previousButtons,queryContext};'),
 'wrong_initial_profile_mask':('{3,12,0x0010}','{3,12,0x0001}'),
 }
 rows=[]
 for name,(old,new) in mutations.items():
  if source.count(old)!=1:raise ValueError('Mutation anchor: '+name)
  candidate=out/(name+'.cpp');candidate.write_text(source.replace(old,new));binary=out/name
  command=['g++','-std=c++17','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I',str(ROOT/'src/game'),str(ROOT/'tests/native/device_held48y.cpp')]+[str(ROOT/'src/game'/n) for n in ['control_filter48b.cpp','control_shaping48v.cpp','control_binding48w.cpp','player_input48x.cpp']]+[str(candidate),'-o',str(binary)]
  build=subprocess.run(command,capture_output=True,text=True,timeout=180);(out/(name+'-build.log')).write_text(build.stdout+build.stderr);row={'name':name,'compile_returncode':build.returncode,'detected':False}
  if build.returncode==0:
   run=subprocess.run([str(binary)],capture_output=True,text=True,timeout=120);row.update(returncode=run.returncode,output=run.stdout+run.stderr,detected=run.returncode==1 and 'device_held48y after' in run.stderr);(out/(name+'-test.log')).write_text(run.stdout+run.stderr)
  rows.append(row);print(name+': '+('detected' if row['detected'] else 'FAILED'),flush=True)
 if production.read_text()!=source:raise ValueError('Production changed during controls')
 result={'status':'passed' if all(r['detected'] for r in rows) else 'failed','production_sha256':hashlib.sha256(production.read_bytes()).hexdigest(),'controls':rows};(out/'sensitivity.json').write_text(json.dumps(result,indent=2)+'\n');return 0 if result['status']=='passed' else 1
if __name__=='__main__':raise SystemExit(main())
