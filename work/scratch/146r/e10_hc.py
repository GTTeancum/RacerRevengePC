import pathlib
p = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b\work\scratch\146r\host_check.py')
t = p.read_text(encoding='utf-8')
old = '''        tob = ini(get(f'tracks\\\\{code}.tob'))
        env = tob.get('environment', {})
        cube = env.get('cubefile1') or env.get('cubefile') or ''
        ok(bool(cube) and has(cube.replace('/', '\\\\')), f'sky from TOB [Environment] cube={cube!r} present')
        trig = tob.get('triggers', {})
        print(f'  info triggers authored={"yes" if trig else "no"} (route trigger owner {"bound" if trig else "none"})')'''
new = '''        tobb = get(f'tracks\\\\{code}.tob')   # binary TOB: names are length-prefixed strings
        cubes = sorted({m.group(0).decode().lower() for m in re.finditer(rb'skies[\\\\/][a-z0-9_]+\\.cub', tobb, re.I)})
        ok(len(cubes) >= 1 and all(has(c.replace('/', '\\\\')) for c in cubes), f'sky named by the TOB environment {cubes} present in the archive')
        trig = re.search(rb'triggers', tobb, re.I) is not None
        print(f'  info TOB names a triggers block={"yes" if trig else "no"}')'''
assert t.count(old) == 1
p.write_text(t.replace(old, new), encoding='utf-8')
print('ok')
