r"""Material sound file tracks\<code>.aub, generated from the converted track's own collision objects.
Shared by every track converter (arcade trk_convert.py, N64 n2r_convert.py). One rule for every code.

Format (BINARY parameter blocks, trk_binparam; measured on the 13 stock .aub files, reader = the port's
src\assets\material_audio50hi.cpp, original 271674..271804 / 279B60 / 279E50):
  [Sounds]  NumMaterials M, DefaultMaterial (1 on every stock course), NumSoulTreeSounds,
            NumMaterialDriveOnSounds D, NumMaterialCollisionSounds C   (+ NumAmbientSounds on GB)
  [MaterialCollisionSound1..C] SoundResourceFile                       (sfx\race\..., no extension)
  [MaterialDriveOnSound1..D]   SoundResourceFile + MaterialSampleSpeed, MaterialMinVolume,
            MaterialMinSpeed, MaterialMaxSpeed, MaterialResonantFreqSpeed, MaterialFreqSlope
  [CollisionObjectsMaterialN] NumCollisionObjects, [MaterialN-ObjectK] OBJ = collision basename
            (the reader assigns objects 1..count-1: the original's strict less-than, 2717E4)
  [SoulTreeSoundN] positional sounds placed in the course (position, distances)
Sound rows come in families of 4 (one family per material index; family 0 = the pod-on-pod
"podscrp/pod_coll" rows every stock file starts with). A COL's material is the last matching
assignment, else DefaultMaterial (271728); the material must have a complete collision family
(material < C/4). Blocks are written sorted by name, as every stock file.

The arcade and the N64 sources carry no surface sound types (race-data.md 5: no surface-type field;
n64-loader-re.md 3: vehicle-reaction bits only, one scrape sound for every wall). So every converted
COL takes material 1 = the rock family, the default of 11 of the 13 stock courses [G default]; no
positional sounds (neither source places any, race-data.md 22). A family other than rock is used only
when the converter passes one per COL from its source data (FAMILIES lists those with stock rows).

Port acceptance today (legacy reader, material_audio50hi.cpp:97-103; provider fixture :221):
collision rows C and drive rows D in {8, 12, 20} with D == C, or C in {16, 32} with D == 12;
NumMaterials 1..4; C >= 8, D >= 8, C + D <= 44. Generated files are checked against that here.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import trk_binparam  # noqa: E402
import trk_res3  # noqa: E402

REVENGE = (__import__('os').environ['RR_CONVERT_STOCK']+'/TRACKS/')
BS = chr(92)
# family -> (drive-on stem, collision stem, stock donor whose rows are copied byte for byte [D])
FAMILIES = {
    'pod': ('podscrp', 'pod_coll', 'TA'),
    'rock': ('rckscrp', 'rockhit', 'TA'),
    'metal': ('mtlscrp', 'metlhit', 'RA'),
    'bush': ('bshscrp', 'bushhit', 'GA'),
    'wood': ('wodscrp', 'woodhit', 'GA'),
}
DEFAULT_FAMILY = 'rock'


def _rows(family):
    """(4 drive rows, 4 collision rows) of a family, copied from its donor's stock .aub."""
    drive_stem, coll_stem, donor = FAMILIES[family]
    r = trk_res3.read_res3(open(REVENGE + donor + '.RES', 'rb').read())
    nm = [n for n in r.names() if n.lower() == 'tracks' + BS + donor.lower() + '.aub'][0]
    blocks = dict(trk_binparam.decode(r.get(nm)))
    s = dict(blocks['sounds'])

    def pick(kind, count, stem):
        out = []
        for i in range(1, int(s[count]) + 1):
            kv = blocks.get(kind + str(i))
            if kv and dict(kv)['soundresourcefile'].split(BS)[-1].startswith(stem):
                out.append(kv)
        if stem == 'pod_coll':
            out = out[:4]
        if len(out) != 4:
            raise ValueError('%s.aub has %d %s rows for %s' % (donor, len(out), kind, family))
        return out
    return (pick('materialdriveonsound', 'nummaterialdriveonsounds', drive_stem),
            pick('materialcollisionsound', 'nummaterialcollisionsounds', coll_stem), donor)


def legacy_ok(c, d, m):
    return (((c in (8, 12, 20) and d == c) or (c in (16, 32) and d == 12)) and 1 <= m <= 4
            and c >= 8 and d >= 8 and c + d <= 44)


def generate(cols, family_of=None):
    """cols: collision basenames in the archive (no extension). family_of: {basename: family} from
    the source's own surface data (absent -> DEFAULT_FAMILY). -> (aub bytes, report dict)."""
    family_of = family_of or {}
    fams = [DEFAULT_FAMILY] + sorted({family_of[c] for c in cols if family_of.get(c, DEFAULT_FAMILY) != DEFAULT_FAMILY})
    m = len(fams)
    drive, coll, donors = [], [], []
    for f in ['pod'] + fams:
        dr, co, donor = _rows(f)
        drive += dr; coll += co; donors.append('%s rows from %s.aub' % (f, donor))
    c, d = len(coll), len(drive)
    if not legacy_ok(c, d, m):
        raise ValueError('generated .aub shape %d collision / %d drive rows / %d materials is outside what the port '
                         'reads today (material_audio50hi.cpp:97-103)' % (c, d, m))
    blocks = [('materialcollisionsound%d' % (i + 1), kv) for i, kv in enumerate(coll)]
    blocks += [('materialdriveonsound%d' % (i + 1), kv) for i, kv in enumerate(drive)]
    assigned = {}
    for k, f in enumerate(fams[1:], 2):
        objs = [c_ for c_ in cols if family_of.get(c_) == f]
        assigned[f] = objs
        # count = objects + 1: the reader assigns objects 1..count-1 (strict less-than, 2717E4)
        blocks.append(('collisionobjectsmaterial%d' % k, [('numcollisionobjects', str(len(objs) + 1))]))
        blocks += [('material%d-object%d' % (k, i), [('obj', o)]) for i, o in enumerate(objs, 1)]
    blocks.append(('sounds', [('nummaterials', str(m)), ('defaultmaterial', '1'), ('numsoultreesounds', '0'),
                              ('nummaterialdriveonsounds', str(d)), ('nummaterialcollisionsounds', str(c))]))
    blocks.sort(key=lambda b: b[0])
    data = trk_binparam.encode(blocks)
    back = dict(trk_binparam.decode(data))
    assert dict(back['sounds'])['nummaterialcollisionsounds'] == str(c)
    rep = dict(materials=m, families=fams, collision_rows=c, drive_rows=d, soul_tree_sounds=0,
               default_material=1, cols=len(cols), assigned={f: len(v) for f, v in assigned.items()}, donors=donors)
    return data, rep


def provenance(rep):
    return ('G generated from the converted collision objects: %d COL, material 1 = %s family (default; the source has '
            'no surface sound types)%s; %d+%d sound rows (%s) [D rows]; no positional sounds' % (
                rep['cols'], rep['families'][0],
                ''.join('; material %d = %s (%d COL)' % (k, f, rep['assigned'][f]) for k, f in enumerate(rep['families'][1:], 2)),
                rep['collision_rows'], rep['drive_rows'], ', '.join(rep['donors'])))
