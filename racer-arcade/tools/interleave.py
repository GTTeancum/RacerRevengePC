# Build linear images from swracer chip dumps: pairs of 16-bit chips form a 32-bit bus.
import sys, os, numpy as np
chips, out = sys.argv[1], sys.argv[2]
ep = ["epr-23174.ic29","epr-23175.ic30","epr-23176.ic31","epr-23177.ic32"]
mk = sorted(f for f in os.listdir(chips) if f.startswith("mpr-"))
def build(names, path):
    parts=[]
    for a,b in zip(names[0::2], names[1::2]):
        x=np.fromfile(os.path.join(chips,a),dtype='<u2'); y=np.fromfile(os.path.join(chips,b),dtype='<u2')
        z=np.empty(x.size*2,dtype='<u2'); z[0::2]=x; z[1::2]=y; parts.append(z)
    np.concatenate(parts).tofile(path)
build(ep, os.path.join(out,"eprom.bin")); build(mk, os.path.join(out,"mask.bin"))
