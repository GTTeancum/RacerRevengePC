"""Isolate GS DLL-call timing without changing the renderer or its fences."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1];out=root/'build/gsProfile47O';out.mkdir(exist_ok=True)
original=root/'work/native/sources/src/PS2Recomp/ps2xRuntime/src/lib/gs/gs_pcsx2_backend41n.cpp'
source=original.read_text()
assert hashlib.sha256(original.read_bytes()).hexdigest()=='ca2e6a4869e011b3df82e68a3dddfb11867271b22c566b0c7151c070926a0bd2'
source=source.replace('#include <cstdio>','#include <cstdio>\n#include "gs_profile47o.h"')
source=source.replace(' GSTransferSnapshot transfer{};', ' mutable rr_gs47o::Profile profile;\n GSTransferSnapshot transfer{};')
calls={'Open':'rrgs_open41i_fn(&p)','Packet':'rrgs_packet41i_fn(path,data,bytes)','Vsync':'rrgs_vsync41i_fn(&p,field)',
       'Reset':'rrgs_reset41n_fn()','Flush':'rrgs_sync41n_fn()','Snapshot':'rrgs_snapshot41i_fn(640,448,frame.pixels.data(),uint32_t(frame.pixels.size()),&frame.width,&frame.height)',
       'Fifo':'rrgs_fifo41i_fn(tail.data(),qwc)','Read':'rrgs_read41n_fn(psm,bp,bw,x,y,&value)','Vram':'rrgs_vram41n_fn(out.data(),uint32_t(out.size()))'}
for kind,call in calls.items():
    assert source.count(call)==1,call
    source=source.replace(call,f'profile.invoke(rr_gs47o::Profile::{kind},[&]{{return {call};}})')
before='void ProcessRawGIF(unsigned path,const uint8_t* data,uint32_t bytes) override {std::lock_guard lock(mutex);'
assert source.count(before)==1;source=source.replace(before,before+'profile.packet(bytes);')
before='check(profile.invoke(rr_gs47o::Profile::Vsync,[&]{return rrgs_vsync41i_fn(&p,field);}));'
assert source.count(before)==1;source=source.replace(before,before+'profile.tick();')
(out/'gs_pcsx2_backend41n.cpp').write_text(source)
(out/'provenance.json').write_text(json.dumps(dict(original=str(original.relative_to(root)),original_sha256=hashlib.sha256(original.read_bytes()).hexdigest(),candidate_sha256=hashlib.sha256((out/'gs_pcsx2_backend41n.cpp').read_bytes()).hexdigest(),
    scope='Elapsed timing around the unchanged nine DLL entry calls under the existing backend lock. Original packets, FIFO, readback and synchronous fences remain unchanged. Not exclusive CPU/GPU/queue-wait timing.'),indent=2))
print('Prepared isolated native GS DLL-call profiler; bridge DLL unchanged.')
