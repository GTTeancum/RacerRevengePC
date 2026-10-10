from pathlib import Path
import contextlib,io,json,hashlib
r=Path(__file__).resolve().parents[2];p=r/'evidence/movementQuery50L/movement_probe50l.py';base=p.read_text()
base=base[:base.index("print('start'")]+'''initial=dict(pair=m.pair(),prepared=m.prepared(),state=m.state(),records=m.records())
ret=m.run(0x121920);m.executing=False
cs=case['candidates'];e=dict(returned=int(bool(m.read(t.Q+0xf8,1)&0x20)),requires_mode2=0x10f580 in m.coverage,initial=initial,after_prepare=m.after_prepare,pair=m.pair(),prepared=m.prepared(),query_flags=m.read(t.Q+0xf8,1),current_candidate=m.identity(m.read(t.Q+0xf4)),spring_state=m.state(),records=m.records(),candidates=[dict(identity=x['id'],flags=m.read(0x20000+i*0x1000+0xf8,1),copied40=t.get(m,0x20000+i*0x1000+0x40,4),copied50=t.get(m,0x20000+i*0x1000+0x50,4)) for i,x in enumerate(cs)],calls=m.movement_calls,events=m.events,callback_calls=[],query_copied40=t.get(m,t.Q+0x40,4),query_copied50=t.get(m,t.Q+0x50,4),append_events=[],vehicle7a0=m.read(0x600000+0x7a0),original_steps=m.steps)
case['query']['filter']['flags']=0x86;case['expected']=e
assert m.data==b'' and not m.source_reads
'''
base=base.replace('self.extension_coverage=set();self.lo64=0;self.hi64=0','self.extension_coverage=set();self.lo64=0;self.hi64=0;self.movement_calls=[]')
base=base.replace('  w=self.words[pc];op=w>>26;fn=w&63', '''  if pc==0x121a9c:self.after_prepare=dict(pair=self.pair(),prepared=self.prepared())
  if pc in (0x121c3c,0x121f0c,0x1221bc):self.movement_calls.append(dict(identity=self.active_candidate,returned=self.g[2][0]))
  w=self.words[pc];op=w>>26;fn=w&63''')
rows=[];steps=0
for label,order,second,miss in [('first-pass',[0,1,2],False,False),('enabled-bit-only',[0,1,2],False,False),('reversed-first-pass',[2,1,0],False,False),('second-pass',[0,1,2],True,False),('geometry-miss',[0,1,2],False,True)]:
 code=base.replace("case=t.make_case('movement-original')",f"case=t.make_case({label!r},order={order!r})")
 code=code.replace("collection['trees'][0]['items']=[1,2,3]",f"collection['trees'][0]['items']={[i+1 for i in order]!r}")
 if label=='enabled-bit-only':code=code.replace('0x86','0x06')
 if second:
  code=code.replace("collection['zones']=[[0,0]]","collection['zones']=[[0,1]]").replace("other=c.initialize(w,d,collection)","collection['trees'].append(__import__('copy').deepcopy(collection['trees'][0]));collection['trees'][0]['items']=[]\nother=c.initialize(w,d,collection)")
 if miss:code=code.replace("m=namespace['initialize_type3'](w,d,meshes,case)","case['pairs']=[[[t.bits(10.),t.bits(1.),t.bits(10.),t.bits(1.)],[t.bits(10.),t.bits(-1.),t.bits(10.),t.bits(1.)]]]\nm=namespace['initialize_type3'](w,d,meshes,case)")
 namespace={'__file__':str(p)};exec(compile(code,str(p),'exec'),namespace);rows.append(namespace['case']);steps+=namespace['m'].steps
out=r/'evidence/movementQuery50L/reference';out.mkdir(exist_ok=True)
(out/'fixtures.json').write_text(json.dumps({'schema':1,'array_slots':4,'meshes':namespace['meshes'],'cases':rows},separators=(',',':'))+'\n')
print(json.dumps({'cases':len(rows),'original_steps':steps,'hits':[bool(x['expected']['query_flags']&32) for x in rows],'flags7a0':[x['expected']['vehicle7a0'] for x in rows]}))
