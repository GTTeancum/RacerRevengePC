import sys;sys.path.insert(0,'.')
import res
e,d=res.load(sys.argv[1])
for n,lo,sz in e:
    if any(n.lower().endswith(x) for x in sys.argv[2].split(',')):
        b=d[lo:lo+sz]
        print('==',n,sz, repr(b[:int(sys.argv[3]) if len(sys.argv)>3 else 120]))
