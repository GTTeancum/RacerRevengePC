"""Verify promoted native content against frozen-reference source-hash derivations."""
from pathlib import Path
import argparse, hashlib, json

ROOT=Path(__file__).resolve().parents[1]
if not __debug__: raise RuntimeError('Normal Python execution required')
def digest(data): return hashlib.sha256(data).hexdigest()
def js(p): return json.loads(p.read_text(encoding='utf-8-sig'))
def main():
    ap=argparse.ArgumentParser(); ap.add_argument('--cache',type=Path,required=True); ap.add_argument('--output',type=Path,required=True)
    args=ap.parse_args(); current=js(args.cache/'current.json'); generation=current['generation']
    raw=(args.cache/'generations'/(generation+'.json')).read_bytes(); assert digest(raw)==generation==current['manifest_sha256']
    manifest=json.loads(raw); index_path=ROOT/'evidence/importTexture48E/reference-index.json'; references=js(index_path)['references']
    counts=dict(textures=0,materials=0,mips=0,rgba_texels=0,unsupported_pst_psm=0); checked=[]
    for member in manifest['members']:
        source=member['source_sha256']; kind=member['member'][-4:].lower()
        if kind not in ('.pst','.psm'): continue
        ref=references[source]
        if not ref['accepted']:
            assert member['status']=='unsupported'; counts['unsupported_pst_psm']+=1; continue
        native=member['native']; fields=dict(native); record_sha=fields.pop('record_sha256')
        assert digest(json.dumps(fields,sort_keys=True,separators=(',',':'),ensure_ascii=False).encode())==record_sha
        assert native['source_sha256']==source and native['key']==digest((native['converter']+'\n'+source).encode())
        assert js(args.cache/'objects'/(native['key']+'.json'))==native
        if kind=='.pst':
            assert member['status']=='converted_texture' and len(native['mips'])==len(ref['mips'])
            for actual,expected in zip(native['mips'],ref['mips']):
                path=args.cache/'objects'/f"{native['key']}-mip{actual['index']}.rrtex"
                data=path.read_bytes()
                assert actual['index']==expected['level_smallest_first']
                assert actual['width']==expected['width'] and actual['height']==expected['height']
                assert digest(data)==actual['sha256']==expected['rrtex_sha256']
                assert digest(data[16:])==expected['rgba_sha256']
                counts['mips']+=1; counts['rgba_texels']+=actual['width']*actual['height']
            counts['textures']+=1
        else:
            expected=ref['material']; assert member['status']=='converted_material' and len(native['passes'])==len(expected['passes'])
            def values(fields): return {f['name']:f['value'] for group in ['authored','unspecified'] for f in fields[group]}
            assert values(native['general'])==expected['source_sections']['General']
            for a,b in zip(native['passes'],expected['passes']):
                assert a['pass']==b['pass'] and a['program']==b['microprogram'] and a['textures']==b['textures']
                assert values(a['fields'])==expected['source_sections'][f"ShaderPass_{b['pass']}"]
                assert a['texture_count_defaulted']==('totalTextureMaps' not in expected['source_sections'][f"ShaderPass_{b['pass']}"])
            counts['materials']+=1
        checked.append(dict(container=member['container'],member=member['member'],source_sha256=source,key=native['key']))
    report=dict(passed=True,generation=generation,reference_index_sha256=digest(index_path.read_bytes()),counts=counts,
        scope='Every accepted native texture mip and material field matches independent frozen48D derivation; original source identity is checked separately by full-cache geometry verifier',members=checked)
    args.output.write_text(json.dumps(report,indent=2)+'\n'); print(json.dumps(dict(passed=True,counts=counts)))
if __name__=='__main__': main()
