#!/usr/bin/env python3
"""Freeze 49K source/evidence, preserve baseline members and verify reopened bytes."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import zipfile

ROOT=Path(__file__).resolve().parents[1]
BASE=ROOT.parent

def sha(data):return hashlib.sha256(data).hexdigest()

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--baseline',type=Path,required=True)
    ap.add_argument('--output',type=Path,required=True)
    ap.add_argument('--verification',type=Path,required=True)
    args=ap.parse_args()
    if args.output.exists() or args.verification.exists():ap.error('Use new output paths')
    if sha(args.baseline.read_bytes())!='6376d5cd58cec22eaa1661ae003d3a73a463f90502bad4a515a0a2975bdaaf8e':
        raise ValueError('Expected exact verified49J baseline archive')
    validation_bytes=(ROOT/'evidence/bodyAdvancement49K/validation.json').read_bytes()
    validation=json.loads(validation_bytes)
    if validation['status']!='passed':raise ValueError('Final validation not passed')
    for name,digest in validation['source_hashes'].items():
        if sha((ROOT/name).read_bytes())!=digest:raise ValueError('Validated source changed: '+name)
    files={p.relative_to(BASE).as_posix():p for p in BASE.rglob('*') if p.is_file()}
    changes=[]
    expected_hashes={}
    with zipfile.ZipFile(args.baseline) as old:
        baseline_count=sum(not i.is_dir() for i in old.infolist())
        for info in old.infolist():
            if info.is_dir():continue
            if info.filename not in files:raise ValueError('Missing baseline member: '+info.filename)
            before=sha(old.read(info.filename));after=sha(files[info.filename].read_bytes())
            expected_hashes[info.filename]=before
            if before!=after:changes.append({'path':info.filename,'before_sha256':before,'after_sha256':after})
    allowed={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
    if {r['path'] for r in changes}!=allowed:raise ValueError('Unexpected changed baseline members: '+str(changes))
    for name,digest in validation['source_hashes'].items():
        expected_hashes[ROOT.name+'/'+name]=digest
    expected_hashes[ROOT.name+'/evidence/bodyAdvancement49K/validation.json']=sha(validation_bytes)
    if set(files)!=set(expected_hashes):
        raise ValueError('Files added/removed after validation: '+str(sorted(set(files)^set(expected_hashes))))
    records=[{'path':name,'bytes':path.stat().st_size,'sha256':sha(path.read_bytes())} for name,path in sorted(files.items())]
    for row in records:
        if row['sha256']!=expected_hashes[row['path']]:
            raise ValueError('File changed after baseline/validation check: '+row['path'])
    manifest={'checkpoint':'49K-BodyAdvancement','created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
              'baseline_archive':args.baseline.name,'baseline_sha256':sha(args.baseline.read_bytes()),
              'changed_baseline_members':changes,'validation':validation,'files':records,
              'collision_contract':'unresolved; existing grouped-query/detector files unchanged',
              '49c_status':'excluded from this verified49J-derived branch; separate work remains unreleased'}
    manifest_name='CLOUD-MILESTONE-49K-BODYADVANCEMENT-MANIFEST.json'
    manifest_bytes=(json.dumps(manifest,indent=2)+'\n').encode()
    with zipfile.ZipFile(args.output,'x',zipfile.ZIP_DEFLATED,compresslevel=6) as archive:
        for row in records:
            data=files[row['path']].read_bytes()
            if sha(data)!=row['sha256']:raise ValueError('File changed during archive: '+row['path'])
            archive.writestr(row['path'],data)
        archive.writestr(manifest_name,manifest_bytes)
    with zipfile.ZipFile(args.output) as archive:
        if len(archive.namelist())!=len(records)+1:raise ValueError('Member count mismatch')
        for row in records:
            if sha(archive.read(row['path']))!=row['sha256']:raise ValueError('Archive hash mismatch: '+row['path'])
        if archive.read(manifest_name)!=manifest_bytes:raise ValueError('Manifest mismatch')
    result={'status':'verified','path':str(args.output.resolve()),'bytes':args.output.stat().st_size,
            'sha256':sha(args.output.read_bytes()),'members':len(records)+1,'all_members_reopened_and_hashed':True,
            'baseline_members_preserved':True,'changed_baseline_members':changes,
            'new_members':len(records)+1-baseline_count,'validation':validation}
    args.verification.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result))
if __name__=='__main__':main()
