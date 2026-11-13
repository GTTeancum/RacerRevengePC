#!/usr/bin/env python3
r"""podre_lin: linear disassembly of the main code range to analysis\podre_lin.txt (for grepping).
    python -I podre_lin.py ..\img\eprom.bin
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sh4dis import Image, BASE_DEFAULT
im = Image(sys.argv[1], BASE_DEFAULT)
out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "analysis", "podre_lin.txt")
lo, hi = 0x0C100000, 0x0C160000
with open(out, "w") as f:
    for line in im.disasm(lo, (hi - lo) // 2): f.write(line + "\n")
print(out)
