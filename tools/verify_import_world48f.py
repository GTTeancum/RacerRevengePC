"""Verify native world cache against independent references and frozen native fixtures."""
from pathlib import Path
import argparse, hashlib, json
ROOT=Path(__file__).resolve().parents[1]
def js(p):return json.loads(p.read_text(encoding='utf-8-sig'))
def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def main():
    if not __debug__:raise RuntimeError('Verification requires assertions')
    ap=argparse.ArgumentParser();ap.add_argument('--build',type=Path,required=True);args=ap.parse_args()
    out=ROOT/'evidence/importFull48F-20261003';cache=out/'native-cache'
    current=js(cache/'current.json');generation=current['generation'];path=cache/'generations'/(generation+'.json')
    assert sha(path)==generation==current['manifest_sha256'];manifest=js(path)
    references=js(ROOT/'evidence/worldImport48F/reference-index.json')
    world_reports={arch:js(args.build/(arch+'-worlds.json')) for arch in ('x64','x86')}
    for report in world_reports.values():assert report['status']=='PASS' and report['failures']==0 and report['accepted']==12 and report['instances']==15340
    members={(m['container'],m['member']):m for m in manifest['members']};verified=[]
    for ref in references:
        m=members[(ref['container'],ref['member'])];assert m['source_sha256']==ref['source_sha256']==sha(ROOT/ref['file'])
        if not ref['accepted']:
            assert m['status']=='unsupported' and m['reason'];continue
        native=m['native'];assert m['status']=='converted_world'
        assert native['converter']=='rr-native-world-48F-v1'
        assert native['key']==hashlib.sha256((native['converter']+'\n'+ref['source_sha256']).encode()).hexdigest()
        for field in ('zones','portals','instances'):assert native[field]==ref[field]
        actual=cache/'objects'/(native['key']+'.rrworld');digest=sha(actual)
        assert digest==native['world_sha256']
        for arch,report in world_reports.items():
            expected=next(w for w in report['worlds'] if w['container']==ref['container'])
            assert digest==expected['rrworld_sha256']
            frozen=args.build/(arch+'-world-fixtures')/Path(expected['cache_fixture']).name
            assert actual.read_bytes()==frozen.read_bytes()
        verified.append(dict(container=ref['container'],member=ref['member'],source_sha256=ref['source_sha256'],rrworld_sha256=digest,instances=ref['instances']))
    dependency=js(ROOT/'outputs/RacerRevenge-Checkpoint48E-NativeMaterialsWorldAndAero-Manifest.json')
    prefix='evidence/importFull48E-20261003/native-cache/objects/'
    old_objects=[f for f in dependency['Evidence']['files'] if f['path'].startswith(prefix)]
    assert old_objects
    for f in old_objects:assert sha(cache/'objects'/f['path'][len(prefix):])==f['sha256'],f['path']
    result=dict(passed=True,generation=generation,accepted_worlds=len(verified),unsupported_worlds=len(references)-len(verified),
        instances=sum(r['instances'] for r in verified),frozen48e_objects_unchanged=len(old_objects),worlds=verified,
        scope='Original field/matrix comparison is in both frozen native checker reports; this verifies promoted bytes against those fixtures and old content against the48E archive manifest. No gameplay or shader fidelity claim.')
    (out/'world-verification.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k!='worlds'}))
if __name__=='__main__':main()
