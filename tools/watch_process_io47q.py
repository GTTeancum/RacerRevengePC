"""Read CPU counters for one verified native-run process; no thread suspension."""
from pathlib import Path
import argparse,ctypes as c,json,time,datetime,re,hashlib
from ctypes import wintypes as w
p=argparse.ArgumentParser();p.add_argument('run',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--interval',type=float,default=5.0);a=p.parse_args()
assert 1<=a.interval<=60
a.run=a.run.resolve();meta=json.loads((a.run/'process-launch.json').read_text(encoding='utf-8-sig'));pid=meta['pid']
k=c.WinDLL('kernel32',use_last_error=True)
class FT(c.Structure):_fields_=[('low',w.DWORD),('high',w.DWORD)]
class TE(c.Structure):_fields_=[('size',w.DWORD),('usage',w.DWORD),('tid',w.DWORD),('pid',w.DWORD),('base',w.LONG),('delta',w.LONG),('flags',w.DWORD)]
def bind(name,restype,*args):
    f=getattr(k,name);f.restype=restype;f.argtypes=args;return f
op=bind('OpenProcess',w.HANDLE,w.DWORD,w.BOOL,w.DWORD)
ot=bind('OpenThread',w.HANDLE,w.DWORD,w.BOOL,w.DWORD)
close=bind('CloseHandle',w.BOOL,w.HANDLE)
query=bind('QueryFullProcessImageNameW',w.BOOL,w.HANDLE,w.DWORD,w.LPWSTR,c.POINTER(w.DWORD))
ex=bind('GetExitCodeProcess',w.BOOL,w.HANDLE,c.POINTER(w.DWORD))
snap=bind('CreateToolhelp32Snapshot',w.HANDLE,w.DWORD,w.DWORD)
first=bind('Thread32First',w.BOOL,w.HANDLE,c.POINTER(TE));nxt=bind('Thread32Next',w.BOOL,w.HANDLE,c.POINTER(TE))
times=bind('GetThreadTimes',w.BOOL,w.HANDLE,c.POINTER(FT),c.POINTER(FT),c.POINTER(FT),c.POINTER(FT))
cycles=bind('QueryThreadCycleTime',w.BOOL,w.HANDLE,c.POINTER(c.c_ulonglong))
description=bind('GetThreadDescription',w.LONG,w.HANDLE,c.POINTER(c.c_void_p))
free=bind('LocalFree',w.HANDLE,w.HANDLE)
ptimes=bind('GetProcessTimes',w.BOOL,w.HANDLE,c.POINTER(FT),c.POINTER(FT),c.POINTER(FT),c.POINTER(FT))
class IO(c.Structure):_fields_=[(n,c.c_ulonglong) for n in ['read_operations','write_operations','other_operations','read_bytes','write_bytes','other_bytes']]
io=bind('GetProcessIoCounters',w.BOOL,w.HANDLE,c.POINTER(IO))

def number(ft):return (ft.high<<32)|ft.low
def utc():return datetime.datetime.now(datetime.timezone.utc).isoformat()
handle=op(0x1000,False,pid);assert handle,'Cannot open target process'
try:
    size=w.DWORD(32768);path=c.create_unicode_buffer(size.value)
    assert query(handle,0,path,c.byref(size))
    assert Path(path.value).resolve()==(a.run/'racer_native_probe.exe').resolve(),path.value
    a.output.mkdir(parents=True,exist_ok=False)
    (a.output/'observer.json').write_text(json.dumps(dict(run=str(a.run),pid=pid,executable=path.value,binary_sha256=meta['binary_sha256'],interval_seconds=a.interval,started_utc=utc(),scope='Read-only per-thread/process OS CPU counters, process IO counters and thread descriptions. No suspension, injection, affinity changes or guest-state edits. Host thread cycle counts cannot be converted directly into Xbox FPS.'),indent=2))
    start=time.perf_counter_ns();count=0;cache={}
    with (a.output/'samples.jsonl').open('x',encoding='utf-8') as output:
        while True:
            code=w.DWORD();assert ex(handle,c.byref(code))
            if code.value!=259:break
            threads=[];errors=[];snapshot=snap(4,0)
            assert snapshot and snapshot!=c.c_void_p(-1).value
            try:
                entry=TE();entry.size=c.sizeof(entry);ok=first(snapshot,c.byref(entry))
                while ok:
                    if entry.pid==pid:
                        thread=ot(0x800,False,entry.tid)
                        if thread:
                            try:
                                created,ended,kernel,user=FT(),FT(),FT(),FT()
                                if times(thread,c.byref(created),c.byref(ended),c.byref(kernel),c.byref(user)):
                                    key=(entry.tid,number(created));name=cache.get(key)
                                    if name is None:
                                        ptr=c.c_void_p();name=''
                                        if description(thread,c.byref(ptr))>=0 and ptr.value:
                                            try:name=c.wstring_at(ptr)
                                            finally:free(ptr)
                                        cache[key]=name
                                    ticks=c.c_ulonglong();has_cycles=bool(cycles(thread,c.byref(ticks)))
                                    threads.append(dict(tid=entry.tid,created_100ns=number(created),name=name,user_100ns=number(user),kernel_100ns=number(kernel),cycles=ticks.value if has_cycles else None))
                                else:errors.append(dict(tid=entry.tid,operation='times',error=c.get_last_error()))
                            finally:close(thread)
                        else:errors.append(dict(tid=entry.tid,operation='open',error=c.get_last_error()))
                    ok=nxt(snapshot,c.byref(entry))
            finally:close(snapshot)
            created,ended,kernel,user=FT(),FT(),FT(),FT()
            process_ok=bool(ptimes(handle,c.byref(created),c.byref(ended),c.byref(kernel),c.byref(user)))
            frames=list(a.run.glob('frame-*.json'));latest=max(frames,key=lambda f:f.stat().st_mtime_ns) if frames else None
            frame=None
            if latest:
                raw=latest.read_text();m=re.search(r'"vsync"\s*:\s*(\d+)',raw)
                if m:frame=dict(path=latest.name,vsync=int(m[1]))
            ioc=IO();io_ok=bool(io(handle,c.byref(ioc)))
            record=dict(io={n:getattr(ioc,n) for n,_ in IO._fields_} if io_ok else None,utc=utc(),elapsed_ns=time.perf_counter_ns()-start,threads=threads,thread_errors=errors,frame=frame,process_user_100ns=number(user) if process_ok else None,process_kernel_100ns=number(kernel) if process_ok else None)
            output.write(json.dumps(record,separators=(',',':'))+'\n');output.flush();count+=1
            if count==1:print('Observing',pid,'threads',len(threads),'at',a.output,flush=True)
            time.sleep(a.interval)
    (a.output/'end.json').write_text(json.dumps(dict(ended_utc=utc(),samples=count,process_exit=code.value),indent=2))
    print('Process is terminal; CPU observations closed:',count,'samples; exit',code.value,flush=True)
finally:close(handle)
