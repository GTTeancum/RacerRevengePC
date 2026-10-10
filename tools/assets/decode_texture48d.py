"""Bounded original PST1 native RGBA and authored PSM binding recovery.

Original indices are linear; the optional original CPU swizzle prepares a GS
upload and is not part of native image data. Original PSMT8/CPSM32/CSM1/CSA0
binding selects the raw16x16 palette through the bit3/bit4 permutation.
Shader behavior, blend normalization and final visual fidelity are separate.
"""
from pathlib import Path
import argparse,json,re,struct,sys
sys.dont_write_bytecode=True
from decode_static48b import ROOT,FormatError,require,decode_pst1,sha
from decode_scene48c import load_container,decode_psg3_scene

def palette_index(index):
    require(isinstance(index,int) and 0<=index<=255,'palette index out of range')
    return (index&~24)|((index&8)<<1)|((index&16)>>1)

def decode_texture(blob):
    palette,surfaces,structural=decode_pst1(blob)
    decoded=[]
    for surface in surfaces:
        require(surface['width']>=16 and surface['height']>=16,'48D proof covers dimensions at least16')
        rgba=bytes(c for index in surface['indices'] for c in palette[palette_index(index)])
        decoded.append({**{k:v for k,v in surface.items() if k!='indices'},'rgba':rgba,'rgba_sha256':sha(rgba)})
    metadata={'revision':1,'kind':1,'palette_offset':12,'palette_sha256':structural['palette_sha256'],
              'source_row_order':'original row zero corresponds to GS texture coordinate T=0; no vertical flip',
              'index_layout':'linear rows; no fast-upload swizzle applied to native image',
              'palette_layout':'raw16x16 RGBA32 upload, CSM1; swap index bits3 and4',
              'original_binding':{'PSM':19,'CPSM':0,'CSM':0,'CSA':0,'CLD':1},
              'color_policy':'original R,G,B,A bytes preserved; no RGB doubling, alpha doubling, gamma change or premultiplication',
              'status':'source texels resolved by original loader/upload/TEX0 and independent supplied PCSX2 layout evidence',
              'limitations':['Original shader modulation, filtering, mip choice, UV generation and blend behavior remain separate.',
                             'Raw GS alpha bytes are preserved; normalized native blend alpha is not thereby established.']}
    return decoded,metadata

def rrtex_bytes(surface):
    w,h=surface['width'],surface['height'];rgba=surface['rgba']
    require(isinstance(w,int) and isinstance(h,int) and 0<w<=1024 and 0<h<=1024,'invalid output dimensions')
    require(len(rgba)==w*h*4,'RGBA output extent mismatch')
    return struct.pack('<8sII',b'RRTEX001',w,h)+rgba

def parse_material(blob):
    require(0<len(blob)<=64*1024,'material size cap')
    try:text=blob.decode('ascii')
    except UnicodeDecodeError as exc:raise FormatError('non-ASCII material') from exc
    require('\0' not in text,'NUL in material')
    sections={};section=None
    for number,line in enumerate(text.splitlines(),1):
        line=line.split(';',1)[0].strip()
        if not line:continue
        if line.startswith('[') and line.endswith(']'):
            section=line[1:-1].strip();require(section and section not in sections,'duplicate or empty material section')
            sections[section]={};continue
        require(section is not None and '=' in line,f'unrecognized material line{number}')
        key,value=(s.strip() for s in line.split('=',1))
        require(key and key.lower() not in {k.lower() for k in sections[section]},'duplicate material key')
        sections[section][key]=value
    def integer(value,label,low,high):
        require(re.fullmatch(r'[0-9]+',value or '') is not None,'invalid '+label)
        n=int(value);require(low<=n<=high,'out-of-range '+label);return n
    require('General' in sections,'missing General material section')
    count=integer(sections['General'].get('TotalShaderPasses'),'shader pass count',1,8)
    require(set(sections)=={'General'}|{f'ShaderPass_{i}' for i in range(1,count+1)},'unexpected shader sections')
    passes=[]
    for i in range(1,count+1):
        fields=sections[f'ShaderPass_{i}'];program=fields.get('microProgramName','')
        require(re.fullmatch(r'[A-Za-z0-9_]+',program) is not None,'invalid microprogram name')
        maps=integer(fields.get('totalTextureMaps','1'),'texture map count',1,2)
        bindings=[]
        for slot in range(1,maps+1):
            name=fields.get(f'TextureMap_{slot}','');require(re.fullmatch(r'[A-Za-z0-9_-]+',name) is not None,'unsupported texture name/path')
            bindings.append({'slot':slot,'name':name})
        declared={int(k.split('_')[-1]) for k in fields if re.fullmatch(r'TextureMap_[0-9]+',k)}
        require(declared==set(range(1,maps+1)),'texture-map count mismatch')
        passes.append({'pass':i,'microprogram':program,'textures':bindings,
                       'authored_values':{k:v for k,v in fields.items() if v},
                       'blank_values_using_original_defaults':[k for k,v in fields.items() if not v]})
    return {'passes':passes,'source_sections':sections,'status':'authored binding recovered; shader implementation not supplied',
            'limitations':['Blank render-state values are preserved as unspecified; no default shading is invented.',
                           'Dual specular, alpha and animated flame programs require their actual native shader/animation equivalents.']}

def write_checked(path,data):
    path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
    require(path.read_bytes()==data,'output readback mismatch')
    return {'path':str(path.resolve()),'bytes':len(data),'sha256':sha(data)}

def convert_group(container_path,mesh_names,output):
    members,container=load_container(container_path);materials={};textures={};meshes=[]
    for name in mesh_names:
        key=name.lower();require(key in members,'missing original PSG '+name)
        blob,record=members[key];scene=decode_psg3_scene(blob)
        slots=[]
        for lod in scene['lods']:
            for surface_index,surface in enumerate(lod['surfaces']):
                material=surface['material'];material_key='textures\\'+material.lower()+'.psm'
                require(material_key in members,'missing original material '+material_key)
                if material_key not in materials:
                    mb,mr=members[material_key];parsed=parse_material(mb)
                    saved=write_checked(output/'materials'/(material.lower()+'.psm'),mb)
                    materials[material_key]={'source':{**mr,'sha256':sha(mb)},'preserved_source':saved,**parsed}
                    for p in parsed['passes']:
                        for binding in p['textures']:
                            stem='textures\\'+binding['name'].lower();candidates=[x for x in [stem+'.pst',stem+'.tga'] if x in members]
                            require(len(candidates)==1,'texture resolution missing or ambiguous '+stem)
                            selected=candidates[0];tb,tr=members[selected]
                            binding.update({'member':tr['name'],'source_sha256':sha(tb)})
                            if selected in textures:continue
                            dest=output/'textures'/binding['name'].lower()
                            source=write_checked(dest/('original'+Path(selected).suffix),tb)
                            tex={'source':{**tr,'sha256':sha(tb)},'preserved_source':source}
                            if selected.endswith('.pst'):
                                levels,meta=decode_texture(tb);tex.update(meta);exports=[]
                                for level in levels:
                                    result=write_checked(dest/f'mip-{level["width"]}x{level["height"]}.rrtex',rrtex_bytes(level))
                                    exports.append({**{k:v for k,v in level.items() if k!='rgba'},'output':result})
                                tex['mips_smallest_first']=exports
                                tex['base_output']=write_checked(dest/'texture.rrtex',rrtex_bytes(levels[-1]))
                            else:tex.update({'status':'original non-PST resource located and preserved; no48D decoder claimed','format':'TGA'})
                            textures[selected]=tex
                slots.append({'lod':lod['level'],'surface':surface_index,'original_material_index':surface['material_index'],
                              'material':material,'material_member':material_key,'vertices':surface['vertex_count'],'triangles':surface['triangle_count']})
        meshes.append({'source':{**record,'sha256':sha(blob)},'surface_materials':slots})
    report={'container':container,'meshes':meshes,'materials':materials,'textures':textures,
            'converter_sha256':sha(Path(__file__).read_bytes()),
            'status':'original bindings and bounded PST1 source texels; final shader fidelity not accepted'}
    write_checked(output/'material-mapping.json',(json.dumps(report,indent=2)+'\n').encode())
    portable=[]
    for key,material in materials.items():
        passes=[]
        for p in material['passes']:
            maps=[]
            for binding in p['textures']:
                texture=textures[binding['member'].lower()];native=texture.get('base_output')
                maps.append({**binding,'rrtex':str(Path(native['path']).relative_to(output.resolve())).replace('\\','/') if native else None,
                             'rrtex_sha256':native['sha256'] if native else None})
            passes.append({'pass':p['pass'],'microprogram':p['microprogram'],'textures':maps})
        portable.append({'material_member':key,'material_source_sha256':material['source']['sha256'],'passes':passes})
    write_checked(output/'base-texture-bindings.json',(json.dumps({'path_base':'directory containing this JSON','materials':portable,
        'status':'source texture binding only; original shader and blend fidelity remain separate'},indent=2)+'\n').encode())
    return report

def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--disc',type=Path,default=ROOT/'work/native/project/disc')
    parser.add_argument('--output',type=Path,default=ROOT/'evidence/textures48D');args=parser.parse_args()
    groups=[('TA','TRACKS/TA.RES',[r'geometry\zone01groundstrips16.psg',r'geometry\ta_bigrocka.psg']),
            ('POD01','PODS/POD01/POD01.RES',[r'pods\pod01\chariot.psg',r'pods\pod01\englt1.psg',r'pods\pod01\engrt1.psg'])]
    summary={}
    for label,rel,meshes in groups:
        report=convert_group(args.disc/rel,meshes,args.output/label)
        summary[label]={'meshes':len(report['meshes']),'materials':len(report['materials']),
                        'textures':len(report['textures']),'PST1_decoded':sum('base_output' in t for t in report['textures'].values()),
                        'non_PST_preserved':[t['source']['name'] for t in report['textures'].values() if 'base_output' not in t]}
    write_checked(args.output/'summary.json',(json.dumps(summary,indent=2)+'\n').encode());print(json.dumps(summary))

if __name__=='__main__':main()
