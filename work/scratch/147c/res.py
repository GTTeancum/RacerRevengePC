import re
WT = r'D:/Programming/GitHub/RacerRevengePC/.claude/worktrees/agent-a931b4f26f2a94abf/'
def res(path, fn):
    p = WT + path
    s = open(p, encoding='utf-8', newline='').read()
    pat = re.compile(r'<<<<<<< HEAD\r?\n(.*?)=======\r?\n(.*?)>>>>>>> [^\n]*\n', re.S)
    out = []; i = 0; n = 0
    for m in pat.finditer(s):
        out.append(s[i:m.start()]); out.append(fn(n, m.group(1), m.group(2))); i = m.end(); n += 1
    out.append(s[i:])
    r = ''.join(out)
    assert '<<<<<<<' not in r and '>>>>>>>' not in r
    open(p, 'w', encoding='utf-8', newline='').write(r)
    print(path, n, 'hunks')
