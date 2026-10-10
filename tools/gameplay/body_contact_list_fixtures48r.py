"""Freeze/execute known PhysicsRigidBody165430 with strict initialized overlays.

Only authorized hash-pinned ELF bytes, never RAM, patches, helper stubs or native
expected values. --frozen validates and replays the self-contained source freeze.
"""
import sys
sys.dont_write_bytecode=True
from pathlib import Path
import hashlib,itertools,json,random,struct
from query_oracle48h import Original

ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/bodyContactList48R'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
CODE_SHA='efd33a85235838ae49e0427a268e2e9815e8c28014d27461736c88ea70db0e20'
SLOT_SHA='06deed723a8c5ad30ead7300b17d2b4eb2ce1d1cdc1c62c83d93db5b15e477b4'
RESET_SHA='975c7d0eda40f6fd62e3bb9b7c4c277da586de66444ac7d74e5b33f3270b5d84'
RESET_SLOT_SHA='498e914193c73d01706c3ef93fa71a734ea2f8b895cccb391781f54623e027fa'
START,END=0x165430,0x1654b0
BODY=0x10000
WIDTH=44

def sha(raw):return hashlib.sha256(raw).hexdigest()
def save(name,value):(OUT/name).write_text(json.dumps(value,indent=2)+'\n')
def freeze(frozen=False):
    OUT.mkdir(parents=True,exist_ok=True)
    if not frozen:
        elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
        assert sha(elf)==ELF_SHA,'ELF hash mismatch'
        header=struct.unpack_from('<16sHHIIIIIHHHHHH',elf)
        assert header[0][:6]==b'\x7fELF\x01\x01'
        segments=[struct.unpack_from('<8I',elf,header[5]+i*header[9]) for i in range(header[10])]
        def extract(address,length):
            matches=[off+address-va for kind,off,va,pa,size,memsize,flags,align in segments
                     if kind==1 and va<=address and address+length<=va+size]
            assert len(matches)==1,'Ambiguous/non-file-backed ELF extent'
            offset=matches[0];return elf[offset:offset+length],offset
        code,offset=extract(START,END-START);slot,slot_offset=extract(0x3ce098,4)
        assert sha(code)==CODE_SHA and sha(slot)==SLOT_SHA
        assert struct.unpack('<I',slot)==(START,)
        reset,reset_offset=extract(0x165420,8);reset_slot,reset_slot_offset=extract(0x3ce094,4)
        assert sha(reset)==RESET_SHA and sha(reset_slot)==RESET_SLOT_SHA
        assert struct.unpack('<2I',reset)==(0x03e00008,0xac80020c)
        assert struct.unpack('<I',reset_slot)==(0x165420,)
        (OUT/'body-contact-reset.bin').write_bytes(reset)
        (OUT/'body-contact-reset.txt').write_text('00165420 03E00008 jr $ra\n00165424 AC80020C sw $zero,0x20c($a0) [return delay slot]\n')
        (OUT/'body-vtable-slot94.bin').write_bytes(reset_slot)
        rows=[{'pc':pc,'word':struct.unpack_from('<I',code,pc-START)[0]} for pc in range(START,END,4)]
        (OUT/'body-contact-list.bin').write_bytes(code)
        (OUT/'body-contact-list.txt').write_text('\n'.join(f"{r['pc']:08X} {r['word']:08X}" for r in rows)+'\n')
        (OUT/'body-vtable-slot98.bin').write_bytes(slot)
        save('original-words.json',dict(elf_sha256=ELF_SHA,no_guest_ram_read=True,
            code=dict(start=START,end_exclusive=END,file_offset=offset,sha256=CODE_SHA,instructions=rows),
            vtable=dict(address=0x3ce000,slot_offset=0x98,file_offset=slot_offset,word=START,sha256=SLOT_SHA),
            reset=dict(start=0x165420,end_exclusive=0x165428,file_offset=reset_offset,sha256=RESET_SHA,
                instructions=[dict(pc=0x165420,word=0x03e00008),dict(pc=0x165424,word=0xac80020c)]),
            reset_vtable=dict(address=0x3ce000,slot_offset=0x94,file_offset=reset_slot_offset,word=0x165420,sha256=RESET_SLOT_SHA),
            address_mapping='Verified unique file-backed ELF PT_LOAD for each extent'))
    proof=json.loads((OUT/'original-words.json').read_text())
    assert proof['elf_sha256']==ELF_SHA
    c=proof['code'];v=proof['vtable']
    assert (c['start'],c['end_exclusive'],c['sha256'])==(START,END,CODE_SHA)
    assert (v['address'],v['slot_offset'],v['word'],v['sha256'])==(0x3ce000,0x98,START,SLOT_SHA)
    rows=c['instructions'];assert [r['pc'] for r in rows]==list(range(START,END,4))
    code=b''.join(struct.pack('<I',r['word']) for r in rows)
    assert sha(code)==CODE_SHA and code==(OUT/'body-contact-list.bin').read_bytes()
    slot=(OUT/'body-vtable-slot98.bin').read_bytes();assert sha(slot)==SLOT_SHA and slot==struct.pack('<I',START)
    reset=proof['reset'];rv=proof['reset_vtable']
    assert (reset['start'],reset['end_exclusive'],reset['sha256'])==(0x165420,0x165428,RESET_SHA)
    assert (rv['address'],rv['slot_offset'],rv['word'],rv['sha256'])==(0x3ce000,0x94,0x165420,RESET_SLOT_SHA)
    reset_raw=b''.join(struct.pack('<I',r['word']) for r in reset['instructions'])
    assert sha(reset_raw)==RESET_SHA and reset_raw==(OUT/'body-contact-reset.bin').read_bytes()
    assert [r['pc'] for r in reset['instructions']]==[0x165420,0x165424]
    assert sha((OUT/'body-vtable-slot94.bin').read_bytes())==RESET_SLOT_SHA
    return {r['pc']:r['word'] for r in rows+reset['instructions']}

class ContactOriginal(Original):
    def __init__(self,words):
        super().__init__(data=b'',words=words)
        self.executing=False;self.data_reads=[];self.data_writes=[]
    def read(self,address,size=4):
        if self.executing:self.data_reads.append((self.pc,address,size))
        return super().read(address,size)
    def write(self,address,value,size=4):
        if self.executing:self.data_writes.append((self.pc,address,size,value&((1<<(8*size))-1)))
        return super().write(address,value,size)

def run(words,other,initial):
    o=ContactOriginal(words)
    for index,value in enumerate(initial):o.write(BODY+0x200+4*index,value)
    o.g[4][0]=BODY;o.g[5][0]=other;o.executing=True
    o.run(START);o.executing=False
    assert o.data==b'' and not o.source_reads
    return o,[o.read(BODY+0x200+4*i) for i in range(21)]

def cases():
    rng=random.Random(0x48a165430)
    def initial(count,slots):
        before=[rng.getrandbits(32) for _ in range(3)]
        after=[rng.getrandbits(32) for _ in range(7)]
        return before+[count&0xffffffff]+list(slots)+after
    # All ordered prefix states over a three-identity alphabet for counts0..5.
    # A fourth absent identity verifies insertion for every state.
    for n in range(6):
        for prefix in itertools.product((0,BODY,0xffffffff),repeat=n):
            for other in (0,BODY,0xffffffff,0x80000000):
                slots=list(prefix)+[rng.getrandbits(32) for _ in range(10-n)]
                yield other,initial(n,slots),dict(group='exhaustive-small-prefix',count=n)
    # Every active duplicate position for all bounded counts, each corner id.
    edges=(0,1,BODY,0x7fffffff,0x80000000,0xffffffff)
    for n in range(11):
        for other in edges:
            for match in range(-1,n):
                slots=[(other+0x101+i)&0xffffffff for i in range(10)]
                if match>=0:slots[match]=other
                yield other,initial(n,slots),dict(group='every-dedup-position',count=n,match=match)
    # Inactive slots must neither deduplicate nor be cleared; when target==count
    # the payload store is same-valued but count still increments.
    for n in range(10):
        for target in range(n,10):
            for other in edges:
                slots=[(other+0x303+i)&0xffffffff for i in range(10)];slots[target]=other
                yield other,initial(n,slots),dict(group='inactive-tail-match',count=n,target=target)
    for n in (10,11,12,31,0xffff,0x3fffffff,0x40000000,0x7ffffffe,0x7fffffff):
        for other in edges:
            yield other,initial(n,[other]*10),dict(group='signed-full-or-overfull',count=n)
    for index in range(1024):
        n=index%11;other=rng.getrandbits(32);slots=[rng.getrandbits(32) for _ in range(10)]
        if n and index%3==0:slots[index%n]=other
        yield other,initial(n,slots),dict(group='random-identity',count=n)

def negative_probes(words):
    records=[]
    for count in (-1,-2,-11,-0x40000001,-0x40000000,-0x80000000,-0x10000000):
        initial=[0xa5a50000+i for i in range(21)];initial[3]=count&0xffffffff
        o=ContactOriginal(words)
        for index,value in enumerate(initial):o.write(BODY+0x200+4*index,value)
        o.g[4][0]=BODY;o.g[5][0]=0x2468;o.executing=True;error=None
        try:o.run(START)
        except ValueError as exc:error=str(exc)
        o.executing=False
        assert o.data==b'' and not o.source_reads
        records.append(dict(count=count,native_contract='InvalidInput; state unchanged',
            original_error=error,original_writes=[dict(pc=f'{pc:08X}',address=f'{a:08X}',size=s,value=v) for pc,a,s,v in o.data_writes],
            original_count_after=o.read(BODY+0x20c),scope='Unsafe original-state illustration; excluded from native equivalence domain'))
    save('negative-count-probes.json',dict(data_bytes=0,source_reads=0,no_original_words_patched=True,cases=records))

def reset_fixtures(words):
    rng=random.Random(0x48a165420);rows=[];steps=0;coverage=set()
    edges=(0,1,9,10,11,12,0x7fffffff,0x7ffffffe,0x40000000,0x3fffffff,
           0x80000000,0x80000001,0xffffffff,0xfffffffe,0xfffffff6,0xbfffffff)
    for index in range(1024):
        initial=[rng.getrandbits(32) for _ in range(21)]
        if index<len(edges):initial[3]=edges[index]
        o=ContactOriginal(words)
        for i,value in enumerate(initial):o.write(BODY+0x200+4*i,value)
        o.g[4][0]=BODY;o.executing=True;o.run(0x165420);o.executing=False
        assert o.data==b'' and not o.source_reads and not o.data_reads
        assert o.data_writes==[(0x165424,BODY+0x20c,4,0)]
        output=[o.read(BODY+0x200+4*i) for i in range(21)]
        rows.append(initial+output);steps+=o.steps;coverage.update(o.coverage)
    assert coverage=={0x165420,0x165424}
    raw=struct.pack('<II',len(rows),42)+b''.join(struct.pack('<42I',*row) for row in rows)
    (OUT/'body-contact-reset-fixtures.bin').write_bytes(raw)
    save('body-contact-reset-fixtures.json',dict(cases=len(rows),words_per_case=42,
        layout='initial21words body+200..250; expected21words body+200..250',
        data_bytes=0,source_reads=0,original_data_reads=0,steps=steps,
        no_original_words_patched=True,no_native_expected_values=True,no_helper_stubs=True,
        scope='Separate original165420 return delay-slot reset; all signed counts; slots and neighbors unchanged',
        coverage=[f'{pc:08X}' for pc in sorted(coverage)],fixture_sha256=sha(raw),
        original_words_sha256=sha((OUT/'original-words.json').read_bytes())))

def main():
    words=freeze('--frozen' in sys.argv)
    rows=[];labels=[];coverage=set();branches={};steps=0;outcomes={'Appended':0,'Duplicate':0,'Full':0}
    for index,(other,initial,label) in enumerate(cases()):
        o,output=run(words,other,initial)
        # Outcome comes from executed branch/store PCs, not a native formula.
        outcome=0 if 0x165498 in o.coverage else (2 if o.branches[0x165438]=={True} else 1)
        outcomes[('Appended','Duplicate','Full')[outcome]]+=1
        assert all(size==4 and BODY+0x20c<=a<=BODY+0x234 for pc,a,size,value in o.data_writes)
        if outcome==2:assert [a for pc,a,size in o.data_reads]==[BODY+0x20c]
        if outcome==1:assert not o.data_writes
        row=[other]+initial+output+[outcome];assert len(row)==WIDTH
        rows.append(row);labels.append(dict(case=index,other_identity=other,**label))
        coverage.update(o.coverage);steps+=o.steps
        for pc,taken in o.branches.items():branches.setdefault(pc,set()).update(taken)
    for pc in (0x165438,0x165454,0x165474,0x16547c,0x165488):assert branches[pc]=={False,True},hex(pc)
    # Unreachable165464 follows an unconditional taken branch and its delay slot;
    # all other original words must execute in this bounded-domain corpus.
    assert set(range(START,END,4))-coverage=={0x165464}
    raw=struct.pack('<II',len(rows),WIDTH)+b''.join(struct.pack('<44I',*r) for r in rows)
    (OUT/'body-contact-list-fixtures.bin').write_bytes(raw)
    negative_probes(words)
    reset_fixtures(words)
    report=dict(cases=len(rows),words_per_case=WIDTH,input_words=22,output_words=22,
        layout='other; initial21words body+200..250; expected21words body+200..250; diagnostic0=append,1=duplicate,2=full',
        steps=steps,data_bytes=0,source_reads=0,no_guest_ram_read=True,no_original_words_patched=True,
        no_native_expected_values=True,no_helper_stubs=True,elf_sha256=ELF_SHA,
        original_words_sha256=sha((OUT/'original-words.json').read_bytes()),fixture_sha256=sha(raw),
        interpreter_sha256=sha((ROOT/'tools/gameplay/query_oracle48h.py').read_bytes()),
        scope='End-to-end known165430 on initialized synthetic state; signed nonnegative counts; raw32-bit identities including zero/self',
        outcomes=outcomes,coverage=[f'{pc:08X}' for pc in sorted(coverage)],
        unreachable_words=['00165464'],branches={f'{pc:08X}':sorted(v) for pc,v in sorted(branches.items())},cases_detail=labels)
    save('body-contact-list-fixtures.json',report)
    print(json.dumps({k:v for k,v in report.items() if k not in ('coverage','branches','cases_detail')}))
if __name__=='__main__':main()
