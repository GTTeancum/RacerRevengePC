"""Replace only the IOP ownership representation, preserving its byte semantics."""
from pathlib import Path
import hashlib
import json
import shutil
root=Path(__file__).resolve().parents[1]
out=root/'build/iop47J'
assert not out.exists()
shutil.copytree(root/'build/iop47I',out)
changes=[]
def edit(name,pairs):
    p=out/name; text=p.read_text(); before=hashlib.sha256(p.read_bytes()).hexdigest()
    for a,b in pairs:
        assert text.count(a)==1,(name,text.count(a),a[:80])
        text=text.replace(a,b)
    p.write_text(text,encoding='utf-8',newline='\n')
    changes.append(dict(path=name,before_sha256=before,after_sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
edit('src/emulator/core/iop_memory.h',[
    ('#include "iop_ram_versions.h"','#include "iop_ram_versions.h"\n#include "byte_ownership.h"'),
    ('        void reset();','        void reset();\n        size_t ownershipStorageBytes() const noexcept {return m_owned.size()*sizeof(uint32_t);}'),
    ('        std::vector<uint8_t> m_owned;','        std::vector<uint32_t> m_owned;\n        rr::ByteOwnership m_ownership;')])
edit('src/emulator/core/iop_memory.cpp',[
    ('m_owned(RamSize)', 'm_owned(RamSize / 32u)'),
    ('        m_sio2.hostPadReader=readXinput31j;', '''        if(!m_ownership.configure(m_owned.data(),static_cast<uint32_t>(m_owned.size()),RamSize))
            throw std::runtime_error("Invalid IOP ownership storage");
        m_sio2.hostPadReader=readXinput31j;'''),
    ('        std::fill(m_owned.begin(), m_owned.end(), uint8_t{0});','        m_ownership.clear();'),
    ('''        return std::all_of(m_owned.begin() + phys, m_owned.begin() + phys + size,
                           [](uint8_t value)
                           { return value != 0u; });''',
     '        return m_ownership.owns(phys, static_cast<uint32_t>(size));'),
    ('        std::fill(m_owned.begin() + address, m_owned.begin() + address + size, uint8_t{1});',
     '        m_ownership.mark(address, static_cast<uint32_t>(size));')])
# Compile the preserved original memory implementation under a different class
# name in the same differential executable. Share identical device type headers;
# do not accidentally compare the new bitmap against a second copy of itself.
original=root/'work/native/sources/src/PS2Recomp/ps2xIOP/src/emulator/core'
header=(original/'iop_memory.h').read_text().replace('IopMemory','OriginalIopMemory47J')
for name in ('iop_sio2.h','iop_adma31e.h','spu/audio-candidate32q/spu_register_bus32q.h'):
    header=header.replace('"'+name+'"','"emulator/core/'+name+'"')
header=header.replace('        void reset();','        void reset();\n        size_t ownershipStorageBytes() const noexcept {return m_owned.size();}')
(out/'memory_reference47j.h').write_text(header)
cpp=(original/'iop_memory.cpp').read_text().replace('IopMemory','OriginalIopMemory47J').replace('"iop_memory.h"','"memory_reference47j.h"')
(out/'memory_reference47j.cpp').write_text(cpp)
(out/'ownership-provenance47j.json').write_text(json.dumps(dict(changes=changes,
    original_memory_cpp_sha256=hashlib.sha256((original/'iop_memory.cpp').read_bytes()).hexdigest(),
    original_memory_h_sha256=hashlib.sha256((original/'iop_memory.h').read_bytes()).hexdigest(),
    source_unchanged=True, original_bytes=2097152, candidate_bytes=262144,
    status='Prepared; original-memory differential and native validation required'),indent=2))
cmake=(root/'tools/IopCandidate47I.cmake').read_text().replace('47i','47j').replace('47I','47J')
cmake=cmake.replace('tests/iop_import_cache47j.cpp','tests/iop_import_cache47i.cpp')
cmake+='\nadd_executable(racer_iop47j_memory "${CMAKE_CURRENT_LIST_DIR}/../tests/iop_ownership47j.cpp" "${rr_iop47j}/memory_reference47j.cpp")\n'
cmake+='target_include_directories(racer_iop47j_memory PRIVATE "${rr_iop47j}" "${rr_iop47j}/src")\n'
cmake+='target_link_libraries(racer_iop47j_memory PRIVATE rr_iop47j)\n'
(root/'tools/IopCandidate47J.cmake').write_text(cmake)
shutil.copy2(out/'differential47i.cpp',out/'differential47j.cpp')
(root/'tools/LegacyHook47J.cmake').write_text('include("${RR_SHARED_ROOT}/tools/LegacyHook47F.cmake")\ncmake_language(DEFER CALL include "${RR_SHARED_ROOT}/tools/IopCandidate47J.cmake")\n')
evidence=root/'evidence/47J'; evidence.mkdir(exist_ok=False)
cmd=(root/'evidence/47I/build.cmd').read_text().replace('47i','47j').replace('47I','47J').replace(' --parallel 2',' racer_iop47j_memory --parallel 2')
(evidence/'build.cmd').write_text(cmd)
runner=(root/'tools/Run-Native47I.ps1').read_text().replace('47I','47J').replace('47i','47j')
runner=runner.replace('RR_IOP_IMPORT_CACHE47J','RR_IOP_IMPORT_CACHE47I').replace('RR_IOP_IMPORT_STATS47J','RR_IOP_IMPORT_STATS47I')
(root/'tools/Run-Native47J.ps1').write_text(runner)
print(out)
