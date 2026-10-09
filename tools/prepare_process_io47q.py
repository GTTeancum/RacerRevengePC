"""Derive read-only observers with process IO counters and syscall metadata."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
t=(root/'tools/watch_thread_cpu47p.py').read_text()
t=t.replace("def number(ft):", "class IO(c.Structure):_fields_=[(n,c.c_ulonglong) for n in ['read_operations','write_operations','other_operations','read_bytes','write_bytes','other_bytes']]\nio=bind('GetProcessIoCounters',w.BOOL,w.HANDLE,c.POINTER(IO))\n\ndef number(ft):")
t=t.replace("record=dict(utc=utc()", "ioc=IO();io_ok=bool(io(handle,c.byref(ioc)))\n            record=dict(io={n:getattr(ioc,n) for n,_ in IO._fields_} if io_ok else None,utc=utc()")
t=t.replace('Read-only per-thread/process OS CPU counters and thread descriptions.', 'Read-only per-thread/process OS CPU counters, process IO counters and thread descriptions.')
(root/'tools/watch_process_io47q.py').write_text(t)
t=(root/'tools/sample_process47p.py').read_text()
t=t.replace("capture_100ns=number(entry.captured),suspend_count=entry.suspend_count", "capture_100ns=number(entry.captured),suspend_count=entry.suspend_count,last_syscall_number=entry.last_syscall,last_syscall_first_argument=entry.last_arg")
t=t.replace('Only instruction pointers, thread identity and CPU counters are retained.', 'Only instruction pointers, thread identity, CPU counters and last-syscall number/first argument metadata are retained; no stack or heap bytes are read.')
(root/'tools/sample_process47q.py').write_text(t)
print('Prepared IO counter and last-syscall metadata observers; no OS policy changes.')
