"""Update the private Oovo IV course using the existing Racer conversion tools."""
import argparse
import hashlib
import json
import tempfile
import re
import sys
from pathlib import Path

def corrected_archive(original, textures):
    """Apply the shaft and electrical-band corrections to a freshly converted Vengeance archive."""
    import geo_pst
    import trk_binparam
    import trk_res3
    from PIL import Image
    import numpy as np

    members = dict(trk_res3.read_res3(original).items())
    by_name = {name.lower(): name for name in members}
    def member(name):
        return by_name[name.replace('/', chr(92))]
    tob = member('tracks/nv.tob')
    rows = trk_binparam.decode(members[tob])
    def numbered(section, key, value):
        return next(int(name[len(section):]) for name, fields in rows
                    if re.fullmatch(section + r'\d+', name) and dict(fields).get(key) == value)
    asteroid = 'nv_ast_4c390'
    rock = numbered('animation', 'referencename', asteroid)
    trigger = numbered('trigger', 'name', 'n64_102_49718')
    if dict(dict(rows)[f'trigger{trigger}action1'])['animationnameref'] != asteroid:
        raise ValueError('Unexpected break trigger for the shaft boulder')
    result = []
    for name, fields in rows:
        match = re.fullmatch(r'(animation|trigger)(\d+)(action\d+)?', name)
        if match:
            kind, index, suffix = match.groups()
            removed = rock if kind == 'animation' else trigger
            if int(index) == removed:
                continue
            if int(index) > removed:
                name = f'{kind}{int(index)-1}{suffix or ""}'
        if name in ('animations','triggers'):
            key = 'numberof' + name
            fields = [(k, str(int(v)-1) if k == key else v) for k,v in fields]
        result.append((name,fields))
    members[tob] = trk_binparam.encode(result)
    for name in list(members):
        if name.lower().startswith('anim' + chr(92) + asteroid + '.'):
            del members[name]
    scb = member('tracks/nv.scb')
    sections = trk_binparam.decode(members[scb])
    dict(sections)['fog'].append(('drawdistancescale','2'))
    members[scb] = trk_binparam.encode(sections)
    # Both electrical textures have a type 0xB U-scroll from 0 to 1 in 1/3 s.
    # The source textures carry varying opacity, including a transparent border on the arcs.
    for material, texture in [('nv_tex0340','tex0340_RGBA32_64x16.png'),('nv_tex1078_b','tex1078_RGBA32_32x32.png')]:
        im = np.array(Image.open(textures / texture).convert('RGBA'))
        im[...,3] = np.rint(im[...,3].astype(float)*128/255).astype(np.uint8)
        members[member('textures/'+material+'.pst')] = geo_pst.encode_rgba(im)[0]
        name = member('textures/'+material+'.psm')
        source = members[name].decode('ascii')
        source = re.sub(r'(?im)^(microProgramName\s*=\s*)PreLitCompressed',r'\g<1>DiffuseTexTransCompressed',source)
        keys = {'alphaBlendingOnOff':'1','inputColorA':'0','inputColorB':'1','inputAlphaC':'0','inputAlphaD':'1',
                'zWriteDisabled':'1','alphaTestOnOff':'0','VertexAnimation':'unique','uvs_animate':'1',
                'TexGenMatrixRow1':'1,0,0,0','TexGenMatrixRow2':'0,1,0,0','TexGenMatrixRow3':'0,0,1,0',
                'TexGenMatrixRow4':'0,0,0,1','u_velocity':'0.05','v_velocity':'0'}
        for key,value in keys.items():
            regex = re.compile(r'(?im)^'+key+r'[ \t]*=[^\r\n]*')
            line = f'{key:<42}= {value}'
            if regex.search(source):
                source = regex.sub(line,source)
            else:
                source = source.rstrip()+'\r\n'+line+'\r\n'
        members[name] = source.encode('ascii')
    encoded = trk_res3.write_res3(list(members.items()))
    if dict(trk_res3.read_res3(encoded).items()) != members:
        raise ValueError('Updated archive failed readback')
    return encoded


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('package', type=Path)
    parser.add_argument('--authoring-tools', type=Path, required=True, help='a2r converter module directory')
    parser.add_argument('--textures', type=Path, required=True, help='145_Vengeance/textures from the N64 export')
    args = parser.parse_args()
    sys.path.insert(0, str(args.authoring_tools.resolve()))
    manifest_path = args.package / 'dlc.json'
    if not manifest_path.exists():
        manifest_path = args.package / 'addon-manifest.json'
    manifest = json.loads(manifest_path.read_text())
    package = manifest.get('proposed_package', manifest)
    if package['id'] != 'n64-vengeance':
        raise ValueError('Expected the n64-vengeance course')
    path = args.package / 'TRACKS/NV.RES'
    original = path.read_bytes()
    original_hash = hashlib.sha256(original).hexdigest()
    row = next(f for f in manifest['files'] if f['path'].lower() == 'tracks/nv.res')
    if (row['sha256'].lower(), row['bytes']) != (original_hash, len(original)):
        raise ValueError('Course archive does not match its package manifest')
    if package['version'] == '1.0.1' and original_hash == 'dcbd55a980be84b9769f98f603140e687a1f31334b22aeefefd15f2afe70769c':
        return
    if original_hash != '2ba2419d3bfe2aff148cea5b499479382402272eaec810a48f22df422c5d5d26':
        raise ValueError('Expected the original 1.0.0 course archive')
    encoded = corrected_archive(original, args.textures)
    row.update(bytes=len(encoded), sha256=hashlib.sha256(encoded).hexdigest())
    package['version'] = '1.0.1'
    with tempfile.TemporaryDirectory(dir=args.package.parent) as temp:
        staging = Path(temp)
        new_archive = staging / 'NV.RES'
        new_manifest = staging / 'dlc.json'
        new_archive.write_bytes(encoded)
        new_manifest.write_text(json.dumps(manifest, indent=1, ensure_ascii=False)+'\n', encoding='utf-8')
        new_archive.replace(path)
        try:
            new_manifest.replace(manifest_path)
        except OSError:
            new_archive.write_bytes(original)
            new_archive.replace(path)
            raise

if __name__ == '__main__':
    main()
