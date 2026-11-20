import re
from edlib import WT
rel = 'tools/capture/run_windows_takeover.py'
p = WT / rel
t = open(p, encoding='utf-8', newline='').read()
rx = re.compile(r'<<<<<<< ours\r?\n(.*?)=======\r?\n(.*?)>>>>>>> theirs\r?\n', re.S)
parts = rx.findall(t)
assert len(parts) == 3


def choose(i, o, th):
    if i == 0:
        return '\n'.join(l for l in o.split('\n') if 'hold_rider and args.addon_rider' not in l and "'--hold-rider needs --addon-rider'" not in l) + th
    if i == 1:
        return th.replace('args.logs_only and not args.hold_rider and args.eyecheck_dlc_hall is None',
                          'args.logs_only and not args.hold_rider and not args.hold_course and args.eyecheck_dlc_hall is None') \
                 .replace('# A --hold-rider / --eyecheck-dlc-hall run', '# A --hold-rider / --hold-course / --eyecheck-dlc-hall run')
    return o + th


i = [0]


def f(m):
    r = choose(i[0], m.group(1), m.group(2)); i[0] += 1; return r


t = rx.sub(f, t)
assert '<<<<<<<' not in t
open(p, 'w', encoding='utf-8', newline='').write(t)
print('ok')
