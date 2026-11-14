import json, os
from collections import Counter, defaultdict
HERE = os.path.dirname(os.path.abspath(__file__))
res = json.load(open(os.path.join(HERE, 'mesh_states.json')))
glob = Counter()
for code, rows in res.items():
    agg = defaultdict(Counter)
    for r in rows:
        a = agg[(r['lod'], r['role'][:6] if r['role'].startswith('engine') else r['role'])]
        a['tris'] += r['tris']; a['nc'] += r['nocull']
        if r['polytype'] != 1: a['pt%d' % r['polytype']] += r['tris']
        if r['poly_alpha'] != 255: a['pa<255'] += r['tris']
        if r['vtx_alpha'][0] != 255: a['va<255'] += r['tris']
        md = r['mat_dec']
        if md.get('alpha_mode'): a['amode'] += r['tris']
        if md.get('blend'): a['blend%d' % md['blend']] += r['tris']
        if r['texmode']: a['texmode%d' % r['texmode']] += r['tris']
        if r['bias']: a['bias'] += r['tris']
        if r['pattern']: a['pat'] += r['tris']
        for k in (r['hdr'] or {}):
            if k.startswith('slot') and k != 'slot0': a[k] += r['hdr'][k]
        if r['polytype'] == 6 and r['tex_alpha'] and r['tex_alpha'][1] < 255: a['pt6_texa<255'] += r['tris']
        if r['polytype'] == 1 and r['tex_alpha'] and r['tex_alpha'][0] < 255: a['pt1_texa<255'] += r['tris']
        glob['881=' + (r['mat_raw'] or {}).get('881', '?')] += 1
        glob['A81=' + (r['mat_raw'] or {}).get('A81', '?')] += 1
        glob['C81=' + (r['mat_raw'] or {}).get('C81', '?')] += 1
        glob['tfmt=%s' % r['tfmt']] += 1
        if r['polytype'] == 6: glob['pt6 tfmt=%s' % r['tfmt']] += 1
    print(code)
    for k in sorted(agg):
        print('   L%d %-8s %s' % (k[0], k[1], dict(agg[k])))
print(glob)
