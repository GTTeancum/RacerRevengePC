# Host check (146r): the common-route data decisions for every add-on course in the test DLC root.
# Reads only; prints one line per decision per course and a final pass/fail count.
import sys, json, re, pathlib, struct, hashlib
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import dlc_out
D = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC')
STOCK = pathlib.Path(r'C:\Games\Star Wars Racer Revenge')
fails = 0


def ok(cond, what):
    global fails
    if not cond:
        fails += 1
    print(('  ok   ' if cond else '  FAIL ') + what)


def res(p):
    return dlc_out._archive_members(p.read_bytes())


def ini(b):
    out, sec = {}, None
    for line in b.decode('latin-1').splitlines():
        line = line.split(';')[0].strip()
        if line.startswith('[') and line.endswith(']'):
            sec = line[1:-1].strip().lower(); out.setdefault(sec, {})
        elif sec and line:
            k, _, v = line.partition(' ') if '=' not in line else line.partition('=')
            out[sec][k.strip().lower()] = v.strip()
    return out


stock_cache = {}


def stock_res(code):
    if code not in stock_cache:
        stock_cache[code] = res(STOCK / 'TRACKS' / (code.upper() + '.RES'))
    return stock_cache[code]


ui = res(STOCK / 'UI' / 'UICOURSE' / 'UICOURSE.RES')
for pkg in sorted(D.iterdir()):
    j = json.loads((pkg / 'dlc.json').read_text(encoding='utf-8'))
    for it in j.get('items', []):
        if it['kind'] != 'course':
            continue
        code = it['code']; C = code.upper()
        print('==', j['id'] + '/' + it['id'], 'code=' + code)
        f = next(x for x in j['files'] if x['path'].upper() == it['archive'].upper())
        raw = (pkg / it['archive']).read_bytes()
        ok(len(raw) == f['bytes'] and hashlib.sha256(raw).hexdigest() == f['sha256'].lower(),
           f'archive listed bytes/sha match (session row archiveSha256 = listed sha) bytes={len(raw)}')
        r = res(pkg / it['archive'])
        names = {n.lower(): n for n in r.names()}
        has = lambda m: m.lower() in names  # noqa: E731
        get = lambda m: r.get(names[m.lower()])  # noqa: E731
        for m in (f'tracks\\{code}.scb', f'tracks\\{code}.tob', f'tracks\\{code}.aub', f'tracks\\m{code}.spl', f'tracks\\a{code}.spl'):
            ok(has(m), f'member by code: {m}')
        ok(not has('tracks\\sa.scb') and not any(n.startswith('tracks\\sa') for n in names), 'no SA-named member (route never reads SA names)')
        cuts = sum(1 for n in range(1, 100) if has(f'tracks\\{code}{n:02d}.vub'))
        ok(cuts >= 1, f'cut cameras tracks\\{code}NN.vub = {cuts} (registry cut slot)')
        ok(has(f'tracks\\{code}TVCam.pos'), f'tv camera tracks\\{code}TVCam.pos')
        tobb = get(f'tracks\\{code}.tob')   # binary TOB: names are length-prefixed strings
        scbb = get(f'tracks\\{code}.scb')   # the SCB [Environment] names the cube file (bare name, read from skies\)
        cubes = sorted({m.group(1).decode().lower() for m in re.finditer(rb'\x00([a-z0-9_]+\.cub)\x00', scbb, re.I)})
        ok(len(cubes) >= 1 and all(has('skies\\' + c) for c in cubes), f'sky named by the SCB environment {cubes} present as skies\\<name>')
        trig = re.search(rb'triggers', tobb, re.I) is not None
        print(f'  info TOB names a triggers block={"yes" if trig else "no"}')
        atm = [n for n in names if n.endswith('.atm')]
        ok(not atm, f'no ATM textures authored (ATM owner none) count={len(atm)}')
        own_tun = has(f'tracks\\{code}.tun'); inh = it.get('inherit', {})
        if own_tun:
            ok(True, 'own tuning member')
        else:
            t = inh.get('tuning', '')
            ok(bool(t) and f'tracks\\{t}.tun' in {n.lower() for n in stock_res(t).names()},
               f'inherit.tuning {t}: stock TRACKS/{t.upper()}.RES#tracks\\{t}.tun present')
        own_part = all(has(f'particle\\demoprt.{e}') for e in ('prt', 'pst', 'psm'))
        if not own_part:
            p = inh.get('particles', '')
            sn = {n.lower() for n in stock_res(p).names()} if p else set()
            ok(bool(p) and all(f'particle\\demoprt.{e}' in sn for e in ('prt', 'pst', 'psm')),
               f'inherit.particles {p}: stock TRACKS/{p.upper()}.RES demoprt.prt/.pst/.psm present')
        ia = ini(get('audio\\introAud.ini')) if has('audio\\introAud.ini') else {}
        sl = ini(get('audio\\scrptLns.ini')) if has('audio\\scrptLns.ini') else {}
        ev = int(ia.get('responseemitter', {}).get('numevents', '0') or 0)
        ln = int(sl.get('general', {}).get('numberoflines', '0') or 0)
        ok(ev == 0 and ln == 0, f'intro accepted with 0 events/0 lines (events={ev} lines={ln})')
        ls = it['loading_screen']
        if 'file' in ls:
            b = (pkg / ls['file']).read_bytes()
            wd = lambda at: struct.unpack_from('<I', b, at)[0]  # noqa: E731
            ok(len(b) == 287772 and (wd(0), wd(4), wd(8), wd(1036), wd(1040)) == (1, 1, 1, 448, 640),
               f'loading screen {ls["file"]} is the 640x448 one-level presenter layout')
        cs = it.get('course_screen', {'stock_template': 'ta'})
        tmpl = cs['stock_template']; holo = f'ui\\textures\\holo_{tmpl}.pst'
        un = {n.lower(): n for n in ui.names()}
        if cs.get('preview'):
            pv = (pkg / cs['preview']).read_bytes(); st = ui.get(un[holo])
            ok(len(pv) == len(st) and pv[:12] == st[:12], f'preview {cs["preview"]} same size/format as stock {holo} ({len(pv)} bytes)')
        bg = it.get('course_background', f'stock:{tmpl}')
        print(f'  info course_background={bg} (absent -> stock:{tmpl}, borrowed)')
        ok(it['ai_class'] in 'ABC' and it['phy_class'] in 'ABC', f'ai_class={it["ai_class"]} -> race class {" ABC".index(it["ai_class"])}, phy_class={it["phy_class"]}')
        rv = it['rival']
        ok(rv == 'none' or re.fullmatch(r'stock:(0[1-9]|1[0-9]|2[0-6])', rv) is not None, f'rival={rv} -> record pod {0 if rv == "none" else int(rv[6:])}')
        modes = it.get('modes', ['single', 'time_trial', 'vs'])
        ok('circuit' not in modes, f'modes={modes} (Single Event subtype 1/2, Time Trial subtype 3)')
print('host check', 'PASS' if not fails else f'FAIL ({fails})')
