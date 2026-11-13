import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()
i = s.index("        self.pending = [\n")
j = s.index("        ]\n", i)
B = chr(92)
block = s[i:j].replace(B + B, B).replace(B, B + B)   # normalise to doubled backslashes
s = s[:i] + block + s[j:]
open(p, 'w').write(s)
print('ok')
