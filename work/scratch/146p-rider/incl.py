import os,re,sys,collections
root=sys.argv[1];targets=sys.argv[2:]
inc=re.compile(r'^\s*#\s*include\s*"([^"]+)"',re.M)
files=[]
for top in ('src','platform','tests','tools'):
    for dp,dn,fn in os.walk(os.path.join(root,top)):
        for f in fn:
            if f.endswith(('.cpp','.h','.inl','.inc','.hpp')):files.append(os.path.normpath(os.path.join(dp,f)))
rev=collections.defaultdict(set)
for f in files:
    try:t=open(f,encoding='utf-8',errors='replace').read()
    except:continue
    for m in inc.finditer(t):
        p=os.path.normpath(os.path.join(os.path.dirname(f),m.group(1)))
        rev[p.lower()].add(f)
for tg in targets:
    seen=set();st=[os.path.normpath(os.path.join(root,tg)).lower()]
    while st:
        x=st.pop()
        for y in rev.get(x,()):
            if y.lower() not in seen:seen.add(y.lower());st.append(y.lower())
    cpps=sorted(s for s in seen if s.endswith('.cpp'))
    print(tg,len(cpps))
    if '-v' in os.environ.get('V',''):print('\n'.join(cpps))
