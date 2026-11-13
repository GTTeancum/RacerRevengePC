"""One-off edit of trk_convert.py: shared transform (a2r_space), merge of the geometry output,
install layout. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()
BS = chr(92)


def rep(a, b, count=1):
    global s
    assert s.count(a) >= 1, a[:80]
    s = s.replace(a, b) if count == 0 else s.replace(a, b, count)


rep("import trk_vub  # noqa: E402\n", "import trk_vub  # noqa: E402\nimport a2r_space  # noqa: E402  (the one transform / course-code definition)\n")
rep("""    def __init__(self, course, scale, mirror, out_root):
        self.course = course
        self.cfg = COURSES[course]
        self.code = self.cfg['code']
        self.cls = physics_class(self.code)
        self.S = scale
        self.mirror = mirror
        self.out = os.path.join(out_root, self.code)""", """    def __init__(self, course, out_root):
        self.course = course
        self.cfg = COURSES[course]
        self.code = a2r_space.CODES[course]
        self.cls = physics_class(self.code)
        self.XF = a2r_space.course_transform(course)
        self.out = os.path.join(out_root, self.code)""")
i = s.index("    # -------------------------------------------------- transform")
j = s.index("    def put(self, name, data, prov):")
s = s[:i] + """    # -------------------------------------------------- transform (a2r_space, nothing local)
    def X(self, p):
        return a2r_space.points(p, self.XF)

    def X2(self, q):
        return a2r_space.xz(q, self.XF)

""" + s[j:]
rep("""        self.setup_transform(tris_a)
        tris = self.X(tris_a)
        fn = fn_a * np.array([1, 1, -1 if self.mirror else 1])""", """        tris = self.X(tris_a)
        fn = a2r_space.normals(fn_a, self.XF)""")
rep("tv_cameras=ntv, scale=self.S, mirror=self.mirror,\n                                translate=[round(x, 3) for x in self.T]))",
    "tv_cameras=ntv, transform=self.XF))")
rep("""    ap.add_argument('--scale', type=float, default=3.28)
    ap.add_argument('--mirror', action='store_true')
""", "")
rep("c = Conv(a.course, a.scale, a.mirror, a.out)", "c = Conv(a.course, a.out)")
open(p, 'w').write(s)
print('ok')
