#!/usr/bin/env python3
"""Archive an explicitly sealed49V tree, preserving every verified49U member.

Read-only release gates run again first. The output and verification paths must
be new and outside the archived tree. No C++ diagnostic or renderer is run.
"""
import argparse
import contextlib
import datetime
import io
import json
from pathlib import Path
import sys
import zipfile
sys.dont_write_bytecode=True
import validate_chariot_lifecycle49v as validation
ROOT=Path(__file__).resolve().parents[1]
BASE=ROOT.parent

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--baseline',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--verification',type=Path,required=True)
    a=p.parse_args()
    for path in (a.output,a.verification):
        validation.require(not path.exists() and not path.is_symlink() and not path.resolve().is_relative_to(BASE.resolve()),'Use new output paths outside the archived tree')
    validation.require(a.output.resolve()!=a.verification.resolve(),'Archive and verification paths must differ')
    old_argv=sys.argv
    try:
        sys.argv=[str(Path(validation.__file__)),'--baseline',str(a.baseline),'--check-only']
        with contextlib.redirect_stdout(io.StringIO()):validation.main()
    finally:sys.argv=old_argv
    seal_path=ROOT/'evidence/chariotLifecycle49V/checkpoint-validation.json'
    seal_bytes=seal_path.read_bytes();seal=json.loads(seal_bytes)
    validation.require(seal['status']=='passed' and seal['checkpoint']=='49V-ChariotLifecycle' and seal['baseline_sha256']==validation.BASELINE_SHA,'Complete49V release seal')
    files={}
    for path in sorted(BASE.rglob('*')):
        validation.require(not path.is_symlink(),'Symlink archive member')
        if path.is_file():files[path.relative_to(BASE).as_posix()]=path
    validation.safe_names(list(files));expected={};changes=[]
    with zipfile.ZipFile(a.baseline)as old:
        names=[r.filename for r in old.infolist()if not r.is_dir()]
        validation.safe_names(names)
        validation.require(len(names)==validation.BASELINE_MEMBERS and validation.sha(a.baseline)==validation.BASELINE_SHA,'Exact verified49U baseline')
        for name in names:
            validation.require(name in files,'Missing baseline member: '+name)
            before=validation.digest(old.read(name));after=validation.sha(files[name]);expected[name]=before
            if before!=after:changes.append({'path':name,'before_sha256':before,'after_sha256':after})
    validation.require({r['path']for r in changes}==validation.ALLOWED,'Only inherited README and CMake may change')
    for name,digest in seal['source_hashes'].items():expected[ROOT.name+'/'+name]=digest
    expected[ROOT.name+'/evidence/chariotLifecycle49V/checkpoint-validation.json']=validation.digest(seal_bytes)
    validation.require(set(files)==set(expected),'Files added or removed after validation')
    records=[]
    for name,path in sorted(files.items()):
        digest=validation.sha(path);validation.require(digest==expected[name],'File changed after seal: '+name)
        records.append({'path':name,'bytes':path.stat().st_size,'sha256':digest})
    manifest={'checkpoint':'49V-ChariotLifecycle','created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'baseline_archive':a.baseline.name,'baseline_sha256':validation.BASELINE_SHA,'baseline_bytes':validation.BASELINE_BYTES,
        'baseline_members':validation.BASELINE_MEMBERS,'changed_baseline_members':changes,'validation':seal,'files':records,
        'scope':'Ordinary source-authenticated Chariot lifecycle with isolated later getter. Full49M frame bridge, integrated physics, reset/spawn/FrameReady, original VU/material/visibility and whole game remain unclaimed.',
        'excluded_work':'No PC/RAM/install or excluded historical capacity/ownership diagnostic was executed.'}
    manifest_name='CLOUD-MILESTONE-49V-CHARIOTLIFECYCLE-MANIFEST.json'
    validation.require(manifest_name not in files,'Release manifest already exists')
    manifest_bytes=(json.dumps(manifest,indent=2)+'\n').encode()
    with zipfile.ZipFile(a.output,'x',zipfile.ZIP_DEFLATED,compresslevel=6)as archive:
        for row in records:
            data=files[row['path']].read_bytes()
            validation.require(validation.digest(data)==row['sha256'] and len(data)==row['bytes'],'File changed during archive: '+row['path'])
            archive.writestr(row['path'],data)
        archive.writestr(manifest_name,manifest_bytes)
    with zipfile.ZipFile(a.output)as archive:
        validation.safe_names(archive.namelist())
        validation.require(set(archive.namelist())==set(files)|{manifest_name} and archive.testzip()is None,'Reopened archive membership/CRC')
        for row in records:
            data=archive.read(row['path']);validation.require(validation.digest(data)==row['sha256'] and len(data)==row['bytes'],'Reopened archive hash/size: '+row['path'])
        validation.require(archive.read(manifest_name)==manifest_bytes,'Reopened manifest bytes')
    result={'status':'verified','path':str(a.output.resolve()),'bytes':a.output.stat().st_size,'sha256':validation.sha(a.output),
        'members':len(records)+1,'all_members_reopened_and_hashed':True,'baseline_members_preserved':True,
        'baseline_members':validation.BASELINE_MEMBERS,'changed_baseline_members':changes,'new_members':len(records)+1-validation.BASELINE_MEMBERS,
        'validation':seal}
    with a.verification.open('x')as out:out.write(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items()if k!='validation'}))

if __name__=='__main__':
    try:main()
    except (validation.GateError,KeyError,TypeError,OSError,ValueError,IndexError,zipfile.BadZipFile)as error:
        print('49V archive failed: '+str(error),file=sys.stderr);raise SystemExit(1)
