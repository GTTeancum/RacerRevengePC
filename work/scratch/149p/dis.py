import sys
# dis.py START END : print disassembly lines in [START,END) from the shared dump.
s=int(sys.argv[1],16);e=int(sys.argv[2],16)
with open(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dis_all.txt',errors='replace') as f:
    on=False
    for l in f:
        try:a=int(l[:8],16)
        except ValueError:continue
        if a>=e:break
        if a>=s:print(l.rstrip())
