# Find "asset-id getter" functions: test id & 0x40000000 (EPROM-resident set) else mask set;
# print the two table bases, the record size and the bound + the error string used.
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image
import sh4an
im = Image(sys.argv[1]); X = sh4an.load(im)
sites = X["lit_refs"].get(0xBFFFFFFF, [])
funcs = sorted(set(sh4an.func_of(X, s) for s in sites))
for f in funcs:
    lines = list(im.disasm(f, 1, labels={}))  # warm
    ins = sh4an.flow(im, f)
    if len(ins) > 120: ins = [a for a in ins if any(abs(a - s) < 60 for s in sites)]
    consts = []; strs = []; bound = None
    for a in ins:
        for ln in im.disasm(a, 1):
            m = re.search(r"=0x(0C[0-9A-F]{6})", ln)
            if m and 0x0C160000 <= int(m.group(1), 16) < 0x0C400000:
                s = im.cstr(int(m.group(1), 16))
                (strs if s else consts).append((m.group(1), s))
            m = re.search(r"mov #(\d+),r1\b", ln)
            if m: bound = int(m.group(1))
    print("f_%08X  tables=%s  bound=%s  %s" % (f, ",".join(c for c, _ in consts), bound, " ".join(repr(s) for _, s in strs)))
