"""Pack locally converted models, textures and course data into the game's DLC layout."""
import hashlib
import json
import os
from pathlib import Path
import sys

import pod_stem
import pod_ui_addon
import trk_addon
import trk_background
import trk_res3

root = Path(os.environ['RR_CONVERT_ROOT'])
stock = Path(os.environ['RR_CONVERT_STOCK'])
slots = dict(ark='04', bol='15', ebe='23', ela='24', nev='11', rat='17')
preview = dict(eb=.6, lc=.2, ns=.2, nb=0., nv=.2, nr=.22, nc=.2, nu=.2, nd=.2, nm=.2)

def assemble(recipe):
    manifest = recipe['manifest']
    item = manifest['items'][0]
    dest = stock / 'DLC' / manifest['id']
    dest.mkdir(parents=True, exist_ok=True)
    if manifest['kind'] == 'course':
        code = item['code']
        rel = 'TRACKS/' + code.upper() + '.RES'
        raw = (root/'converted'/code/'install'/rel).read_bytes()
        if code == 'nv':
            from update_vengeance import corrected_archive
            raw = corrected_archive(raw, root/'racer-n64/export/145_Vengeance/textures')
        archive = trk_res3.read_res3(raw)
        inherited = {'tracks\\'+code+'.tun', 'particle\\demoprt.prt','particle\\demoprt.pst','particle\\demoprt.psm'}
        packed = trk_res3.write_res3([(n,b) for n,b in archive.items() if n.lower() not in inherited])
        (dest/'TRACKS').mkdir(exist_ok=True)
        (dest/rel).write_bytes(packed)
        art = dest/'UI/TEXTURES'
        art.mkdir(parents=True,exist_ok=True)
        holo, _, _ = trk_addon.hologram_preview(packed, code, preview[code])
        (art/('HOLO_'+code.upper()+'.PST')).write_bytes(holo)
        ui = (stock/'UI/UICOURSE/UICOURSE.RES').read_bytes()
        bg, _, _, _ = trk_background.course_background(packed,code,recipe['view'],ui)
        (art/('BKG_'+code.upper()+'.PST')).write_bytes(bg)
    else:
        stem = item['stem']
        nn = slots[stem.split('_')[-1]]
        pod = 'POD'+nn
        exports = root/'racer-arcade/export/a2r'
        donor = exports/'pods'/pod/'install/PODS'/pod
        pod_stem.convert(str(donor/(pod+'.RES')), nn, stem, str(dest), str(donor/(pod+'.CAM')))
        pod_ui_addon.build(nn,stem,str(dest),[str(exports/'ui/members')])
        for suffix in ('.pod_stem.json','.pod_ui.json'):
            (dest/(stem+suffix)).unlink()
    for row in manifest['files']:
        data = (dest/row['path']).read_bytes()
        row.update(bytes=len(data),sha256=hashlib.sha256(data).hexdigest())
    (dest/'dlc.json').write_text(json.dumps(manifest,indent=1),encoding='utf-8')

for package in sys.argv[1:]:
    assemble(json.loads((root/'recipes'/(package+'.json')).read_text()))
