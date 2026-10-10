"""Bounded process-local Windows snapshots, retaining IPs/counters only.

Uses SDK PSS thread-context capture; no manual SuspendThread/ResumeThread,
register writes, memory clone, handles or heap contents are requested/saved.
The observer perturbs host timing and is not a throughput benchmark.
"""
from pathlib import Path
import argparse,ctypes as c,json,time,datetime,hashlib
from ctypes import wintypes as w
p=argparse.ArgumentParser();p.add_argument('run',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--seconds',type=int,default=20);p.add_argument('--interval-ms',type=int,default=100);a=p.parse_args()
assert 1<=a.seconds<=120 and 50<=a.interval_ms<=1000
a.run=a.run.resolve();meta=json.loads((a.run/'process-launch.json').read_text(encoding='utf-8-sig'));pid=meta['pid']
k=c.WinDLL('kernel32',use_last_error=True)
def bind(n,r,*args):f=getattr(k,n);f.restype=r;f.argtypes=args;return f
class FT(c.Structure):_fields_=[('low',w.DWORD),('high',w.DWORD)]
class ContextControl(c.Structure):
    _fields_=[('home',c.c_uint64*6),('flags',w.DWORD),('mxcsr',w.DWORD),('segments',w.WORD*6),('eflags',w.DWORD),('debug',c.c_uint64*6),('gpr',c.c_uint64*16),('rip',c.c_uint64)]
assert ContextControl.rip.offset==248 and c.sizeof(ContextControl)==256
class Thread(c.Structure):
    _fields_=[('exit_status',w.DWORD),('teb',c.c_void_p),('pid',w.DWORD),('tid',w.DWORD),('affinity',c.c_size_t),('priority',c.c_int),('base_priority',c.c_int),('last_arg',c.c_void_p),('last_syscall',w.WORD),('created',FT),('ended',FT),('kernel',FT),('user',FT),('start',c.c_void_p),('captured',FT),('flags',c.c_uint),('suspend_count',w.WORD),('context_size',w.WORD),('context',c.c_void_p)]
class Module(c.Structure):
    _fields_=[('size',w.DWORD),('module_id',w.DWORD),('pid',w.DWORD),('global_usage',w.DWORD),('process_usage',w.DWORD),('base',c.c_void_p),('bytes',w.DWORD),('handle',w.HMODULE),('name',w.WCHAR*256),('path',w.WCHAR*260)]
op=bind('OpenProcess',w.HANDLE,w.DWORD,w.BOOL,w.DWORD);close=bind('CloseHandle',w.BOOL,w.HANDLE)
query=bind('QueryFullProcessImageNameW',w.BOOL,w.HANDLE,w.DWORD,w.LPWSTR,c.POINTER(w.DWORD))
ex=bind('GetExitCodeProcess',w.BOOL,w.HANDLE,c.POINTER(w.DWORD))
capture=bind('PssCaptureSnapshot',w.DWORD,w.HANDLE,c.c_uint,w.DWORD,c.POINTER(w.HANDLE))
release=bind('PssFreeSnapshot',w.DWORD,w.HANDLE,w.HANDLE)
marker_new=bind('PssWalkMarkerCreate',w.DWORD,c.c_void_p,c.POINTER(w.HANDLE));marker_free=bind('PssWalkMarkerFree',w.DWORD,w.HANDLE)
walk=bind('PssWalkSnapshot',w.DWORD,w.HANDLE,c.c_uint,w.HANDLE,c.c_void_p,w.DWORD)
snap=bind('CreateToolhelp32Snapshot',w.HANDLE,w.DWORD,w.DWORD)
first=bind('Module32FirstW',w.BOOL,w.HANDLE,c.POINTER(Module));nxt=bind('Module32NextW',w.BOOL,w.HANDLE,c.POINTER(Module))
def modules():
    handle=snap(0x18,pid);assert handle and handle!=c.c_void_p(-1).value
    rows=[]
    try:
        entry=Module();entry.size=c.sizeof(entry);ok=first(handle,c.byref(entry))
        while ok:
            rows.append(dict(name=entry.name,path=entry.path,base=entry.base,bytes=entry.bytes));ok=nxt(handle,c.byref(entry))
    finally:close(handle)
    return rows
def number(ft):return (ft.high<<32)|ft.low
def utc():return datetime.datetime.now(datetime.timezone.utc).isoformat()
a.output.mkdir(parents=True,exist_ok=False)
manifest=dict(pid=pid,run=str(a.run),binary_sha256=meta['binary_sha256'],seconds=a.seconds,interval_ms=a.interval_ms,started_utc=utc(),source_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
    documentation=['https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/nf-processsnapshot-psscapturesnapshot','https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/ns-processsnapshot-pss_thread_entry'],
    scope='Process-local PSS thread/control-context observations. Only instruction pointers, thread identity, CPU counters and last-syscall number/first argument metadata are retained; no stack or heap bytes are read. No memory clone, stack/heap contents, injected code, manual suspension or privilege changes. This observer can perturb host timing; IP frequency is wall-sampled, not exclusive CPU weighting.')
(a.output/'observer.json').write_text(json.dumps(manifest,indent=2))
handle=op(0x410,False,pid);assert handle,('OpenProcess',c.get_last_error())
count=0;failures=[];durations=[]
try:
    size=w.DWORD(32768);path=c.create_unicode_buffer(size.value);assert query(handle,0,path,c.byref(size))
    assert Path(path.value).resolve()==(a.run/'racer_native_probe.exe').resolve()
    (a.output/'modules.json').write_text(json.dumps(modules(),indent=2))
    start=time.perf_counter_ns();deadline=time.monotonic()+a.seconds
    with (a.output/'samples.jsonl').open('x') as out:
        while time.monotonic()<deadline:
            code=w.DWORD();assert ex(handle,c.byref(code))
            if code.value!=259:break
            snapshot=w.HANDLE();t0=time.perf_counter_ns()
            error=capture(handle,0x180,0x100001,c.byref(snapshot)) # THREADS|THREAD_CONTEXT, AMD64 CONTROL.
            elapsed=time.perf_counter_ns()-t0;durations.append(elapsed)
            if error:failures.append(dict(operation='capture',error=error));break
            marker=w.HANDLE();rows=[]
            try:
                error=marker_new(None,c.byref(marker))
                if error:failures.append(dict(operation='marker',error=error));break
                try:
                    while True:
                        entry=Thread();error=walk(snapshot,3,marker,c.byref(entry),c.sizeof(entry))
                        if error==259:break # ERROR_NO_MORE_ITEMS.
                        if error:failures.append(dict(operation='walk',error=error));break
                        if entry.pid!=pid:failures.append(dict(operation='identity',error=entry.pid));break
                        if entry.flags&1:continue
                        if not entry.context or entry.context_size<c.sizeof(ContextControl):failures.append(dict(operation='context',error=entry.context_size));break
                        ctx=c.cast(entry.context,c.POINTER(ContextControl)).contents
                        if ctx.flags&0x100001!=0x100001:failures.append(dict(operation='context_flags',error=ctx.flags));break
                        rows.append(dict(tid=entry.tid,created_100ns=number(entry.created),ip=ctx.rip,user_100ns=number(entry.user),kernel_100ns=number(entry.kernel),capture_100ns=number(entry.captured),suspend_count=entry.suspend_count,last_syscall_number=entry.last_syscall,last_syscall_first_argument=entry.last_arg))
                finally:marker_free(marker)
            finally:release(handle,snapshot)
            out.write(json.dumps(dict(utc=utc(),elapsed_ns=time.perf_counter_ns()-start,capture_ns=elapsed,threads=rows),separators=(',',':'))+'\n');out.flush();count+=1
            if failures:break
            if count==1:print('PSS captured',len(rows),'thread contexts for PID',pid,flush=True)
            time.sleep(a.interval_ms/1000)
finally:
    close(handle)
    (a.output/'end.json').write_text(json.dumps(dict(ended_utc=utc(),samples=count,failures=failures,total_capture_ns=sum(durations),maximum_capture_ns=max(durations,default=0)),indent=2))
print('PSS observer ended:',count,'samples; errors',failures,flush=True)
raise SystemExit(bool(failures) or count==0)
