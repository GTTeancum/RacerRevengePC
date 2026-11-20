import pathlib
WT = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a242116fa032b3d0b')


def edit(rel, pairs):
    """Exact replacements; each old string must occur exactly once. Keeps the file's line endings."""
    p = WT / rel
    t = open(p, encoding='utf-8', newline='').read()
    nl = '\r\n' if '\r\n' in t else '\n'
    for old, new in pairs:
        old = old.replace('\r\n', '\n').replace('\n', nl)
        new = new.replace('\r\n', '\n').replace('\n', nl)
        n = t.count(old)
        assert n == 1, (rel, n, old[:120])
        t = t.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(t)
    print('edited', rel)
