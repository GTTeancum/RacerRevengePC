"""Prepare an opt-in legacy adapter candidate without modifying reference source."""
import hashlib
import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
original = root / 'work/native/sources/src/PS2Recomp/ps2xIOP/src/emulator/iop_emulator.cpp'
out = root / 'build/legacy-iop47B2'
out.mkdir(parents=True, exist_ok=True)
target = out / 'iop_emulator.cpp'
assert not target.exists(), 'Use a new candidate directory; do not overwrite evidence'
data = original.read_bytes()
text = data.decode().replace('\r\n', '\n')

def replace(before, after):
    global text
    assert text.count(before) == 1, (text.count(before), before[:100])
    text = text.replace(before, after)

replace('#include "rr_iop_capture45n.h"', '#include "rr_iop_capture45n.h"\n#include "idle_window.h"\n#include <cstdlib>')
replace('            memory.reset();', '''            idleWindow.invalidate();
            idleAdvances = idleCycles = servicePasses = 0;
            const char* idleFlag = std::getenv("RR_IOP_IDLE_BATCH");
            idleBatchEnabled = idleFlag && idleFlag[0] == '1';
            memory.reset();''')
replace('''                while (totalCycles < target)
                {
                    memory.advanceSoundTo(totalCycles);''', '''                while (totalCycles < target)
                {
                    if (idleBatchEnabled) {
                        const uint64_t beforeIdle = totalCycles;
                        if (idleWindow.advance(target, totalCycles)) {
                            ++idleAdvances;
                            idleCycles += totalCycles - beforeIdle;
                            if (totalCycles == target) break;
                        }
                        idleWindow.invalidate();
                        ++servicePasses;
                    }
                    memory.advanceSoundTo(totalCycles);''')
replace('uint64_t nextWake = kernel.nextWakeCycle(target);',
        'uint64_t nextWake = kernel.nextWakeCycle(idleBatchEnabled ? UINT64_MAX : target);')
replace('''                        nextWake = alarm.nextEventCycle(nextWake);
                        totalCycles''', '''                        nextWake = alarm.nextEventCycle(nextWake);
                        if (idleBatchEnabled) idleWindow.remember(totalCycles, nextWake);
                        totalCycles''')
replace('''        IopHost &host;
        IopMemory memory;''', '''        rr::IdleWindow idleWindow;
        bool idleBatchEnabled = false;
        uint64_t idleAdvances = 0, idleCycles = 0, servicePasses = 0;
        IopHost &host;
        IopMemory memory;''')
# These are all external mutators. Read-only diagnostics and RAM reads are not
# service operations in the reference emulator.
for signature in [
    'ModuleLoadResult IopEmulator::loadModule(std::string_view path, const void *arguments, uint32_t argumentSize)',
    'ModuleLoadResult IopEmulator::loadModuleBuffer(uint32_t guestAddress, const void *arguments, uint32_t argumentSize)',
    'bool IopEmulator::stopModule(int32_t moduleId, int32_t *result)',
    'std::vector<int16_t> IopEmulator::takeAutoDmaPcm(unsigned core)',
    'RpcResult IopEmulator::handleRpc(const RpcRequest &request)',
    'void IopEmulator::onSifTransfer(const SifTransfer &transfer)',
    'uint32_t IopEmulator::allocateMemory(uint32_t size, uint32_t alignment)',
    'bool IopEmulator::freeMemory(uint32_t address)',
    'bool IopEmulator::writeMemory(uint32_t address, const void *source, size_t size)',
    'bool IopEmulator::zeroMemory(uint32_t address, size_t size)',
]:
    before = signature + '\n    {'
    replace(before, before + '\n        m_impl->idleWindow.invalidate();')
replace('''        std::vector<std::string> result;
        if(m_impl->memory.sio2()''', '''        std::vector<std::string> result;
        const char* idleStats = std::getenv("RR_IOP_IDLE_STATS");
        if (m_impl->idleBatchEnabled && idleStats && idleStats[0] == '1') {
            std::ostringstream out;
            out << "idle-batch advances=" << m_impl->idleAdvances
                << " cycles=" << m_impl->idleCycles << " services=" << m_impl->servicePasses;
            result.push_back(out.str());
        }
        if(m_impl->memory.sio2()''')
target.write_text(text, encoding='utf-8', newline='\n')
(out / 'provenance.json').write_text(json.dumps({
    'original': str(original), 'original_sha256': hashlib.sha256(data).hexdigest(),
    'candidate_sha256': hashlib.sha256(target.read_bytes()).hexdigest(),
    'reference_modified': False, 'enabled_by': 'RR_IOP_IDLE_BATCH=1',
    'acceptance': 'Unverified candidate; compare to unchanged source and validate native run before adoption.'
}, indent=2), encoding='utf-8')
print(target)
