"""Real PST/PSM bounds tests and independently sourced GS layout differential."""
import json,struct,sys
from pathlib import Path
sys.dont_write_bytecode=True
from decode_texture48d import ROOT,FormatError,sha,decode_texture,rrtex_bytes,parse_material,palette_index
from decode_static48b import decode_pst1
from verify_texture48d import ReferenceLayout
checks=0
def check(value,label):
    global checks
    checks+=1
    if not value:raise AssertionError(label)
def rejects(fn,data,label):
    try:fn(data)
    except FormatError:check(True,label);return
    raise AssertionError('accepted invalid '+label)
def change(data,offset,fmt,value):
    b=bytearray(data);struct.pack_into(fmt,b,offset,value);return bytes(b)

def main():
    out=ROOT/'evidence/textures48D';ref=ReferenceLayout();mapping=ref.palette_mapping();tested=[];mips=0;pixels=0
    check(mapping==[palette_index(i) for i in range(256)],'independent physical CSM1 palette mapping')
    check(mapping!=list(range(256)) and len(set(mapping))==256,'permutation is nontrivial and bijective')
    # Original opcodes establish the exact binding conditions of the oracle.
    for va,word in [(0x242284,0xa220000a),(0x242288,0xa220000b),(0x24228c,0xa220000c),
                    (0x242294,0xa223000d),(0x242820,0x00031dfc),(0x242828,0x0002163c)]:
        check(ref.elf.words(va,1)[0]==word,'original CPSM/CSM/CSA/CLD field evidence')
    for label in ['TA','POD01']:
        report=json.loads((out/label/'material-mapping.json').read_text())
        portable=json.loads((out/label/'base-texture-bindings.json').read_text())
        check(len(portable['materials'])==len(report['materials']),'portable material count')
        for material in portable['materials']:
            original=report['materials'][material['material_member']]
            check(material['material_source_sha256']==original['source']['sha256'],'portable material provenance')
            check(len(material['passes'])==len(original['passes']),'portable pass count')
            for p in material['passes']:
                for bind in p['textures']:
                    source=report['textures'][bind['member'].lower()]
                    check(bind['source_sha256']==source['source']['sha256'],'portable texture provenance')
                    if bind['rrtex'] is None:
                        check('base_output' not in source,'unsupported texture remains explicit')
                    else:
                        check(sha((out/label/bind['rrtex']).read_bytes())==bind['rrtex_sha256']==source['base_output']['sha256'],'portable native texture hash')
        for key,item in report['textures'].items():
            if 'base_output' not in item:continue
            original=Path(item['preserved_source']['path']).read_bytes();check(sha(original)==item['source']['sha256'],'original source hash')
            palette,surfaces,_=decode_pst1(original);decoded,metadata=decode_texture(original)
            for source,actual in zip(surfaces,decoded):
                w,h=source['width'],source['height'];indices=source['indices']
                # Game lookup tables and literal upload algorithm are independent
                # from the converter; PCS X2 memory+CLUT tables provide sampling.
                recovered=ref.upload32_read8(ref.original_upload(list(indices),w,h),w,h)
                check(bytes(recovered)==indices,'real original upload→GS sample index identity')
                expected=bytes(c for i in recovered for c in palette[mapping[i]])
                check(expected==actual['rgba'],'full sampled RGBA matches native conversion')
                check(len(expected)==w*h*4,'native RGBA bounds')
                check(actual['rgba'][3::4]==bytes(palette[mapping[i]][3] for i in indices),'raw alpha preserved without rescaling')
                check(rrtex_bytes(actual)==Path(item['preserved_source']['path']).parent.joinpath(f'mip-{w}x{h}.rrtex').read_bytes(),'saved mip deterministic')
                mips+=1;pixels+=w*h
            check(rrtex_bytes(decoded[-1])==Path(item['base_output']['path']).read_bytes(),'base output deterministic')
            cutoffs=set(range(0,20))|{1035,len(original)-1,len(original)//2}
            for s in surfaces:
                at=s['header_offset'];cutoffs.update([at,at+1,at+15,at+16,at+16+s['bytes']-1])
            for cut in sorted(cutoffs):rejects(decode_texture,original[:cut],f'{key} prefix{cut}')
            for at,fmt,value in [(0,'<H',2),(2,'<H',1),(4,'<I',0),(8,'<I',0xffffffff),
                                  (1036,'<I',0),(1040,'<I',0xffffffff),(1044,'<I',1)]:
                rejects(decode_texture,change(original,at,fmt,value),'unsupported/truncated PST layout')
            rejects(decode_texture,original+b'\0','PST trailing bytes')
            tested.append({'source':key,'sha256':sha(original),'mips':len(surfaces),'base_output_sha256':item['base_output']['sha256']})
        for key,material in report['materials'].items():
            blob=Path(material['preserved_source']['path']).read_bytes();p=parse_material(blob)
            check(p['passes']==[{k:v for k,v in row.items() if k!='textures'}|{'textures':[{'slot':x['slot'],'name':x['name']} for x in row['textures']]} for row in material['passes']],'material names/slots and authored state exact')
            for p in material['passes']:
                for bind in p['textures']:check(bind['member'].lower() in report['textures'],'bound original texture located')
    fixture=b'[General]\nTotalShaderPasses=1\n[ShaderPass_1]\nmicroProgramName=PreLitCompressed\nTextureMap_1=ta_sand8\n'
    check(parse_material(fixture)['passes'][0]['textures']==[{'slot':1,'name':'ta_sand8'}],'original default map count1')
    for bad in [fixture.replace(b'TotalShaderPasses=1',b'TotalShaderPasses=99'),fixture+b'TextureMap_1=other\n',
                fixture.replace(b'ta_sand8',b'../ta_sand8'),fixture+b'totalTextureMaps=2\n',fixture+b'[General]\n',
                fixture.replace(b'PreLitCompressed',b''),fixture+b'\xff',fixture+b'\0',fixture+b'TextureMap_2=other\n']:
        rejects(parse_material,bad,'invalid/ambiguous PSM binding')
    for bad in [-1,256,'1']:rejects(palette_index,bad,'palette bounds/type')
    result={'status':'PASS','checks':checks,'textures':tested,'mip_surfaces':mips,'texels_compared_RGBA':pixels,
            'limits':['No original shader/rasterizer run; proof resolves source texture data and authored bindings.',
                      'Only PST1 flags0 type1, dimensions16..1024 and accepted original mip structure supported.']}
    (out/'tests.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:v for k,v in result.items() if k!='textures'}))

if __name__=='__main__':main()
