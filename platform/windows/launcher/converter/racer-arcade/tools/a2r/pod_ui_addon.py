r"""One add-on pod's OWN small menu archive (UI\<STEM>_UI.RES), built from the menu members a UI converter
wrote for stock slot NN, instead of edited copies of the nine stock menu archives. One rule for every pod.

    python -I pod_ui_addon.py NN STEM OUT_DIR --members DIR [--members DIR ...]
        DIR = a UI converter's loose-member folder (members\<ARCHIVE>\<member path>, e.g.
              racer-arcade\export\a2r\ui\members or demolition\export\ui\members)
        writes OUT_DIR\UI\<STEM>_UI.RES and OUT_DIR\<stem>.pod_ui.json (report: role and source per member)

NOT ORIGINAL BEHAVIOUR: add-on pods are a PC enhancement (work\scratch\dlc\DESIGN.md 2.3 'pod' ui, 4.3). What the
stock menus use per pod (notes\ui-convert.md 1) and where it goes:

  role            stock archive / member (slot NN)                  own archive member (stem S)
  rider_model     UIRIDER ui\geometry\NN.psg                        ui\geometry\S.psg
  character       UIRIDER ui\anim\NNc.psg/.mcf/.vut + 6 .mot        ui\anim\Sc.*  (MCF names follow)
  character pages UIRIDER ui\textures\NNc_1/_2.pst (+ .psm)         ui\textures\Sc_1/_2.*
  pod pages       UIPODTEX UI\Textures\NN_1/2/3.pst; UIRIDER NN_1/2/3.psm   UI\Textures\S_1/2/3.pst, ui\textures\S_1/2/3.psm
  shadow          UIRIDER ui\textures\NN_shdw.pst/.psm              ui\textures\S_shdw.*
  portrait/flag   UIRIDER ui\textures\NN_flrg.pst (64x64)           ui\textures\S_flrg.pst
  background      UIBKG ui\textures\NN_bkg.pst; UIRIDER NN_bkg.psm  ui\textures\S_bkg.*
  results_icon    OVERLAYS ui\textures\NN_fsm.pst (32x32)           ui\textures\S_fsm.pst
  garage_model    UIGARAGE\NNE.RES ui\geometry\NNe.psg, NNe_1.pst/.psm, spcgarge.pst   ui\geometry\Se.psg ...
  stats           PODPHYS pods\podNN\PodNN.phy (= race PHY)         none: read from the pod archive's own PHY
Members the converter did not write (the stock .psm material definitions) are taken from the stock archive of
slot NN with their TextureMap names changed by the rule (flagged 'stock material definition, names changed').
spcgarge.pst is a shared name (byte-identical in every NNE.RES) and is shipped as is.
Name rule: a basename or name that starts with NN followed by '_', 'c', 'e' or '.' gets S instead of NN; the UI
pod's root node (named by the pod number, '4' for pod 04) and the garage root node ('04e') follow.
Not built: UITOURN / UIUNLOCK copies (tournament and unlock pages are not part of the free-play add-on phases).
"""
import argparse
import hashlib
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import geo_psg  # noqa: E402
import trk_res3  # noqa: E402
import pod_stem  # noqa: E402

BS = chr(92)
UI = (__import__('os').environ['RR_CONVERT_STOCK']+'/UI')

# (role, stock archive (game-relative), member template with NN) in the order written
LAYOUT = [
    ('rider_model', 'UIRIDER/UIRIDER.RES', r'ui\geometry\NN.psg'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc.psg'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc.mcf'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc.vut'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc_ent.mot'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc_idl1.mot'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc_idl2.mot'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc_idl3.mot'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc_breathe.mot'),
    ('character', 'UIRIDER/UIRIDER.RES', r'ui\anim\NNc_sel.mot'),
    ('character_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NNc_1.pst'),
    ('character_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NNc_1.psm'),
    ('character_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NNc_2.pst'),
    ('character_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NNc_2.psm'),
    ('pod_pages', 'UIPODTEX.RES', r'UI\Textures\NN_1.pst'),
    ('pod_pages', 'UIPODTEX.RES', r'UI\Textures\NN_2.pst'),
    ('pod_pages', 'UIPODTEX.RES', r'UI\Textures\NN_3.pst'),
    ('pod_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_1.psm'),
    ('pod_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_2.psm'),
    ('pod_pages', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_3.psm'),
    ('shadow', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_shdw.pst'),
    ('shadow', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_shdw.psm'),
    ('portrait', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_flrg.pst'),
    ('background', 'UIBKG.RES', r'ui\textures\NN_bkg.pst'),
    ('background', 'UIRIDER/UIRIDER.RES', r'ui\textures\NN_bkg.psm'),
    ('results_icon', 'TEXTURES/OVERLAYS.RES', r'ui\textures\NN_fsm.pst'),
    ('garage_model', 'UIGARAGE/NNE.RES', r'ui\geometry\NNe.psg'),
    ('garage_model', 'UIGARAGE/NNE.RES', r'ui\textures\NNe_1.pst'),
    ('garage_model', 'UIGARAGE/NNE.RES', r'ui\textures\NNe_1.psm'),
    ('garage_model', 'UIGARAGE/NNE.RES', r'ui\textures\spcgarge.pst'),
]
# material / texture names the stock menus provide globally (loaded with UIRIDER for every pod)
GLOBAL_UI_NAMES = ('cable', 'specmap1', 'specmap2', 'pod_wndw')


def sha(b):
    return hashlib.sha256(b).hexdigest()


def ren(name, nn, stem):
    return re.sub(r'^' + nn + r'(?=[_ce.]|$)', stem, name)


def ren_path(path, nn, stem):
    parts = path.split(BS)
    return BS.join(parts[:-1] + [ren(parts[-1], nn, stem)])


def member_dirs(dirs):
    r"""{(archive key, lower member path): file path} from members\<ARCHIVE_KEY>\... folders."""
    out = {}
    for d in dirs:
        for arch in os.listdir(d):
            root = os.path.join(d, arch)
            for dp, _, fs in os.walk(root):
                for f in fs:
                    rel = os.path.relpath(os.path.join(dp, f), root).replace('/', BS)
                    out[(arch.upper(), rel.lower())] = os.path.join(dp, f)
    return out


def arch_key(stock_rel):
    return 'UI_' + stock_rel.replace('/', '_').upper()


def build(nn, stem, out_dir, dirs):
    pod_stem.check_stem(stem)
    conv = member_dirs(dirs)
    stock_cache = {}

    def stock(rel):
        if rel not in stock_cache:
            stock_cache[rel] = trk_res3.read_res3(open(os.path.join(UI, *rel.split('/')), 'rb').read())
        return stock_cache[rel]
    members, rows = [], []
    for role, srel, tmpl in LAYOUT:
        srel = srel.replace('NN', nn)
        src_name = tmpl.replace('NN', nn)
        key = (arch_key(srel), src_name.lower())
        new = ren_path(src_name, nn, stem)
        if key in conv:
            data = open(conv[key], 'rb').read()
            source = 'converter: ' + conv[key]
        else:
            r = stock(srel)
            real = [x for x in r.names() if x.lower() == src_name.lower()]
            if not real:
                raise SystemExit('no converter member and no stock member for %s (%s)' % (src_name, srel))
            data = r.get(real[0])
            source = 'STOCK %s %s' % (srel, real[0])
        changed = False
        low = new.lower()
        if low.endswith('.psm'):
            t = data.decode('latin-1')
            t2 = re.sub(r'(?im)^(\s*TextureMap_\d+\s*=\s*)(\S+)', lambda m: m.group(1) + ren(m.group(2), nn, stem), t)
            changed = t2 != t
            data = t2.encode('latin-1')
        elif low.endswith('.mcf'):
            t = data.decode('latin-1')
            t2 = re.sub(r'(?im)^((?:SLTFile|VUTfilename|NAME|VUE)\s*=\s*)(\S+)', lambda m: m.group(1) + ren(m.group(2), nn, stem), t)
            changed = t2 != t
            data = t2.encode('latin-1')
        elif low.endswith('.psg'):
            m = geo_psg.parse(data)
            if geo_psg.write(m) != data:
                raise ValueError('%s: PSG does not re-write identically' % src_name)
            mats = [ren(x, nn, stem) for x in m['materials']]
            if mats != m['materials']:
                m['materials_raw'] = [None if a != b else r_ for a, b, r_ in zip(mats, m['materials'], m['materials_raw'])]
                m['materials'] = mats
                changed = True
            for nd in m['nodes']:
                if nd['name'] in (nn, str(int(nn)), nn + 'e'):
                    nd['name'] = stem + nd['name'][len(nn):] if nd['name'].startswith(nn) else stem
                    nd['name_raw'] = None
                    changed = True
            d2 = geo_psg.write(m)
            m2 = geo_psg.parse(d2)
            if geo_psg.write(m2) != d2 or m2['materials'] != mats:
                raise ValueError('%s: renamed PSG does not round-trip' % new)
            data = d2
        members.append((new, data))
        rows.append(dict(role=role, member=new, from_member=src_name, from_archive=srel, source=source,
                         stock_bytes=source.startswith('STOCK') and not changed,
                         stock_definition_renamed=source.startswith('STOCK') and changed,
                         bytes=len(data), sha256=sha(data)))
    # checks: names unique, no slot number left, every psm TextureMap / PSG material resolves
    names = [x for x, _ in members]
    if len({x.lower() for x in names}) != len(names):
        raise ValueError('duplicate member names')
    left = [x for x in names if re.match(nn + r'(?=[_ce.]|$)', x.split(BS)[-1])]
    if left:
        raise ValueError('slot number left: %s' % left)
    bases = {x.split(BS)[-1].lower().rsplit('.', 1)[0] for x in names}
    unresolved = []
    for x, d in members:
        if x.lower().endswith('.psm'):
            for t in pod_stem.texture_maps(d):
                if t.lower() not in bases and t.lower() not in GLOBAL_UI_NAMES:
                    unresolved.append('%s TextureMap %s' % (x, t))
        if x.lower().endswith('.psg'):
            for mt in geo_psg.parse(d)['materials']:
                if mt.lower() not in bases and mt.lower() not in GLOBAL_UI_NAMES:
                    unresolved.append('%s material %s' % (x, mt))
    out = trk_res3.write_res3(members)
    back = trk_res3.read_res3(out)
    if list(back.names()) != names or any(back.get(x) != d for x, d in members):
        raise ValueError('written archive does not read back')
    up = stem.upper()
    os.makedirs(os.path.join(out_dir, 'UI'), exist_ok=True)
    open(os.path.join(out_dir, 'UI', up + '_UI.RES'), 'wb').write(out)
    roles = {}
    for r_ in rows:
        roles.setdefault(r_['role'], []).append(r_['member'])
    rep = dict(slot=nn, stem=stem, archive='UI/%s_UI.RES' % up, bytes=len(out), sha256=sha(out), members=len(members),
               roles=roles, unresolved_names=unresolved, rows=rows)
    json.dump(rep, open(os.path.join(out_dir, stem + '.pod_ui.json'), 'w'), indent=1)
    return rep


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('nn')
    ap.add_argument('stem')
    ap.add_argument('out_dir')
    ap.add_argument('--members', action='append', required=True)
    a = ap.parse_args()
    rep = build(a.nn, a.stem, a.out_dir, a.members)
    print('%s: %d members, %d bytes, sha %s; unresolved %s' % (rep['archive'], rep['members'], rep['bytes'],
                                                             rep['sha256'][:16], rep['unresolved_names']))


if __name__ == '__main__':
    main()
