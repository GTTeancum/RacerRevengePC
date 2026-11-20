import pathlib
S = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b\work\scratch\146r')
one = (S / 'one.py').read_text(encoding='utf-8')
vs = (S / 'vs.cmd').read_text(encoding='utf-8')
for L in 'UWSPRM':
    d = S / L
    (d / 'temp').mkdir(parents=True, exist_ok=True)
    (d / 'one.py').write_text(one.replace("OUT=WT/'work'/'scratch'/'146r'/'obj'", "OUT=WT/'work'/'scratch'/'146r'/'%s'/'obj'" % L)
                              .replace('max_workers=4', 'max_workers=2'), encoding='utf-8')
    (d / 'vs.cmd').write_text(vs.replace(r'work\scratch\146r\temp', 'work\\scratch\\146r\\%s\\temp' % L), encoding='utf-8')
    assert ("'%s'/'obj'" % L) in (d / 'one.py').read_text()
print('ok')
