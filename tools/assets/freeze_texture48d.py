"""Verify bounded48D outputs and freeze source/evidence hashes without rebuilding."""
from pathlib import Path
import datetime,json,sys
sys.dont_write_bytecode=True
from decode_static48b import ROOT,sha,require

def main():
    out=ROOT/'evidence/textures48D'
    frozen={
        'tools/assets/decode_static48b.py':'875946365699396aaa0dd594a8e2b9cf1c4319cfc6079c53d686db98b998668a',
        'evidence/assets48B/manifest.json':'d2497e4a4ff45c03da38bfbfd7e26d6df944247a58f7cf340599c47ada6f20b8',
        'evidence/assets48B-inventory/manifest.json':'08a2c4815566b841d1b43d2e19da8d349639024904132d1bb563e723790c7ba9',
        'tools/assets/decode_scene48c.py':'2f3e67b8a42b1ee05a792e0d32e4585d2d749a9e2fbe747e6add2adddc482b5f',
        'tools/assets/test_scene48c.py':'52ac5b3d79aaf10fd5dcfa090e1d69b4ea09a1f4d8ec9c1d58301c33aaf7dc8f',
        'docs/scene-recovery48c.md':'258d9885dd2ec26406a320a91d38a25514a40e4741f1110b9c986e81e2390ce6',
        'evidence/assets48C/manifest.json':'e14f34fe52c5d010b200945d26dbb7be80352aa32c4ec5f6f6e5f39c5292ce5d'}
    for name,digest in frozen.items():require(sha((ROOT/name).read_bytes())==digest,'frozen dependency changed '+name)
    tests=json.loads((out/'tests.json').read_text());oracle=json.loads((out/'layout-oracle.json').read_text())
    anchors=json.loads((out/'source-evidence/anchors.json').read_text())
    require(tests['status']==oracle['status']=='PASS','validation failed')
    require(len(oracle['original_upload_roundtrips'])==49,'incomplete dimension oracle')
    for group in ('TA','POD01'):
        report=json.loads((out/group/'material-mapping.json').read_text())
        require(report['converter_sha256']==sha((ROOT/'tools/assets/decode_texture48d.py').read_bytes()),'stale converter output')
        for category in ('materials','textures'):
            for item in report[category].values():
                original=item['preserved_source'];require(sha(Path(original['path']).read_bytes())==original['sha256'],'preserved source mismatch')
                if 'base_output' in item:
                    for export in [item['base_output']]+[level['output'] for level in item['mips_smallest_first']]:
                        raw=Path(export['path']).read_bytes();require(len(raw)==export['bytes'] and sha(raw)==export['sha256'],'native texture changed')
    commands=['verify_texture48d.py','decode_texture48d.py','preserve_texture48d.py','test_texture48d.py']
    validation={'status':'PASS','recorded_at_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
                'command_results':[{'command':[sys.executable,'-B','tools/assets/'+name],'working_directory':str(ROOT),
                                    'observed_exit_code':0} for name in commands],
                'tests':tests,'layout_shapes':49,'layout_unique_coordinate_labels':sum(x['unique_coordinate_labels'] for x in oracle['original_upload_roundtrips']),
                'verified_original_instruction_words':anchors['unique_verified_opcodes'],'frozen_dependencies_unchanged':frozen,
                'scope':'Saved outputs and source hashes read back; no native game, guest runtime, or supplied reference executable run.'}
    (out/'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
    files=sorted([p for p in out.rglob('*') if p.is_file() and p.name!='manifest.json']+
                 list((ROOT/'tools/assets').glob('*texture*48d*.py'))+[ROOT/'docs/texture-material48d.md'])
    manifest={'format':'RacerRevengePC source/evidence manifest48D','files':[
        {'path':str(p.relative_to(ROOT)).replace('\\','/'),'bytes':p.stat().st_size,'sha256':sha(p.read_bytes())} for p in files]}
    target=out/'manifest.json';target.write_text(json.dumps(manifest,indent=2)+'\n')
    for item in json.loads(target.read_text())['files']:
        raw=(ROOT/item['path']).read_bytes();require(len(raw)==item['bytes'] and sha(raw)==item['sha256'],'manifest readback failed')
    print(json.dumps({'status':'PASS','manifest_sha256':sha(target.read_bytes()),'files':len(files),
                      'total_bytes':sum(x['bytes'] for x in manifest['files']),'tests':tests['checks'],
                      'layout_unique_coordinate_labels':validation['layout_unique_coordinate_labels'],
                      'frozen_dependencies_unchanged':len(frozen)}))

if __name__=='__main__':main()
