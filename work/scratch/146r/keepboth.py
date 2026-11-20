# keepboth.py <file>: resolve every conflict hunk by keeping ours then theirs (line endings preserved).
import sys, re
p = sys.argv[1]
t = open(p, encoding='utf-8', newline='').read()
rx = re.compile(r'<<<<<<< ours\r?\n(.*?)=======\r?\n(.*?)>>>>>>> theirs\r?\n', re.S)
t, n = rx.subn(lambda m: m.group(1) + m.group(2), t)
open(p, 'w', encoding='utf-8', newline='').write(t)
print(p, 'hunks', n)
