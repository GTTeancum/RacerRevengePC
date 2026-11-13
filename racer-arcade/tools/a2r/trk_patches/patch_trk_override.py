"""One-off edit of trk_convert.py: course-code override (--code-as), borrowed target-course files,
facts in the manifest. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


rep("""    def __init__(self, course, out_root):""", """    def __init__(self, course, out_root, code_as=None):""")
rep("""        self.code = a2r_space.CODES[course]""", """        self.natural_code = a2r_space.CODES[course]
        self.code = a2r_space.course_code(course, code_as)     # the one override parameter
        self.override = self.code != self.natural_code""")
rep("""        if gman['code'] != self.code or gman['transform'] != self.XF:""",
    """        if gman['code'] != self.natural_code or gman['transform'] != self.XF:""")
rep("""        # ---- keep for verification""", """        # ---- packing under an original Revenge code: ship that course's own pinned files
        self.borrowed = []
        if self.override and a2r_space.overrides_original(self.code):
            rt = trk_res3.read_res3(open(REVENGE + self.code.upper() + '.RES', 'rb').read())
            for nm in ['tracks' + BS + '%s.tun' % self.code, 'tracks' + BS + '%s.aub' % self.code,
                       'audio' + BS + 'introAud.ini', 'audio' + BS + 'scrptLns.ini',
                       'particle' + BS + 'demoprt.prt', 'particle' + BS + 'demoprt.pst', 'particle' + BS + 'demoprt.psm']:
                real = [n_ for n_ in rt.names() if n_.lower() == nm.lower()][0]
                for old in [k for k in self.files if k.lower() == nm.lower()]:
                    del self.files[old]; self.prov.pop(old, None)
                self.put(real, rt.get(real), 'BORROWED: byte copy of the original %s.RES member (the port pins it; silo-override.md 5)' % self.code.upper())
                self.borrowed.append(real)
            ld = 'UI' + BS + 'TEXTURES' + BS + 'LD_%s.PST' % self.code.upper()
            self.extra.pop(ld, None)
            self.borrowed.append(ld + ' (original kept in place, not written)')
            self.flags.append('packed as original course code %s (override of %s): borrowed %s [D]' % (
                self.code, self.natural_code, ', '.join(self.borrowed)))

        # ---- keep for verification""")
rep("""    ap.add_argument('--out', default=os.path.join(SCR, 'export', 'a2r'))""",
    """    ap.add_argument('--out', default=None)
    ap.add_argument('--code-as', default=None, help='pack under this course code (a2r_space.course_code override)')""")
rep("""    c = Conv(a.course, a.out)""", """    out = a.out or (os.path.join(SCR, 'export', 'a2r', 'override') if a.code_as else os.path.join(SCR, 'export', 'a2r'))
    c = Conv(a.course, out, a.code_as)""")
open(p, 'w').write(s)
print('ok')
