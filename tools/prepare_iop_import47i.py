"""Prepare a bounded positive import cache with observed-RAM range versions."""
from pathlib import Path
import hashlib
import json
import shutil
root=Path(__file__).resolve().parents[1]
source=root/'build/iop47H'
out=root/'build/iop47I'
assert not out.exists()
shutil.copytree(source,out)
changes=[]
def edit(name,pairs):
    p=out/name
    text=p.read_text()
    before=hashlib.sha256(p.read_bytes()).hexdigest()
    for a,b in pairs:
        assert text.count(a)==1,(name,text.count(a),a[:80])
        text=text.replace(a,b)
    p.write_text(text,encoding='utf-8',newline='\n')
    changes.append(dict(path=name,before_sha256=before,after_sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
edit('src/emulator/core/iop_memory.h',[
    ('#include "iop_sio2.h"','#include "iop_sio2.h"\n#include "iop_ram_versions.h"'),
    ('        void reset();','        void reset();\n        const rr::IopRamVersions<>& ramVersions() const noexcept {return m_ramVersions;}'),
    ('        std::vector<uint8_t> m_ram;','        rr::IopRamVersions<> m_ramVersions;\n        std::vector<uint8_t> m_ram;')])
edit('src/emulator/core/iop_memory.cpp',[
    ('        std::fill(m_ram.begin(), m_ram.end(), uint8_t{0});','        m_ramVersions.changed(0, RamSize);\n        std::fill(m_ram.begin(), m_ram.end(), uint8_t{0});'),
    ('        std::fill(m_owned.begin() + address, m_owned.begin() + address + size, uint8_t{1});',
     '        m_ramVersions.changed(address, static_cast<uint32_t>(size));\n        std::fill(m_owned.begin() + address, m_owned.begin() + address + size, uint8_t{1});')])
edit('src/emulator/imports/iop_imports.h',[
    ('#include <cstddef>','#include <cstddef>\n#include <array>\n#include "iop_ram_versions.h"'),
    ('        void reset();','''        void reset();
        struct DecodeStats {uint64_t probes=0,hits=0,searchWords=0,stubPairs=0;};
        DecodeStats decodeStats() const noexcept {return m_decodeStats;}
        bool decodeCacheEnabled() const noexcept {return m_decodeEnabled;}
        size_t decodeCacheBytes() const noexcept {return sizeof(m_decodeCache);}'''),
    ('        IopMemory &m_memory;','''        struct DecodeEntry {
            uint32_t physicalPc=0;
            IopImportCall call;
            rr::IopImportRangeVersion range;
            bool valid=false;
        };
        bool m_decodeEnabled=false;
        mutable std::array<DecodeEntry,256> m_decodeCache;
        mutable DecodeStats m_decodeStats;
        IopMemory &m_memory;''')])
edit('src/emulator/imports/iop_imports.cpp',[
    ('#include <algorithm>','#include <algorithm>\n#include <cstdlib>'),
    ('        m_libraries.clear();','''        m_libraries.clear();
        const char* flag=std::getenv("RR_IOP_IMPORT_CACHE47I");
        m_decodeEnabled=flag && flag[0]=='1';
        for(auto& entry:m_decodeCache) entry.valid=false;
        m_decodeStats={};'''),
    ('        const uint32_t searchBegin = physicalPc > 0x10000u ? physicalPc - 0x10000u : 0u;', '''        ++m_decodeStats.probes;
        auto* cached=m_decodeEnabled && !(physicalPc&3u) && physicalPc<=IopMemory::RamSize-8u
            ? &m_decodeCache[(physicalPc>>3u)&255u] : nullptr;
        if(cached && cached->valid && cached->physicalPc==physicalPc && cached->range.matches(m_memory.ramVersions())) {
            ++m_decodeStats.hits;
            return cached->call;
        }
        if(cached) cached->valid=false;
        const uint32_t searchBegin = physicalPc > 0x10000u ? physicalPc - 0x10000u : 0u;'''),
    ('            const uint32_t table = candidate - 20u;','            ++m_decodeStats.searchWords;\n            const uint32_t table = candidate - 20u;'),
    ('                const uint32_t first = m_memory.read32(stub);','                ++m_decodeStats.stubPairs;\n                const uint32_t first = m_memory.read32(stub);'),
    ('''                return IopImportCall{
                    trimLibraryName(name),
                    static_cast<uint16_t>(delay & 0xFFFFu),
                    m_memory.read16(table + 8u),
                };''','''                IopImportCall call{
                    trimLibraryName(name),
                    static_cast<uint16_t>(delay & 0xFFFFu),
                    m_memory.read16(table + 8u),
                };
                // Only bytes actually capable of changing this successful search
                // matter: the selected header through this stub's second word.
                // A nearer injected header or a changed earlier terminator is
                // inside this interval. Cache misses retain the original search.
                if(cached && cached->range.capture(m_memory.ramVersions(),table,physicalPc+8u)) {
                    cached->physicalPc=physicalPc; cached->call=call; cached->valid=true;
                }
                return call;''')])
edit('src/emulator/iop_emulator.cpp',[
    ('        const char* readyStats = std::getenv("RR_IOP_READY_STATS47G");','''        const char* importStats = std::getenv("RR_IOP_IMPORT_STATS47I");
        if(importStats && importStats[0]=='1') {
            const auto stats=m_impl->imports.decodeStats();
            std::ostringstream out;
            out<<"import-cache enabled="<<m_impl->imports.decodeCacheEnabled()
               <<" probes="<<stats.probes<<" hits="<<stats.hits<<" search_words="<<stats.searchWords
               <<" stub_pairs="<<stats.stubPairs<<" cache_bytes="<<m_impl->imports.decodeCacheBytes();
            result.push_back(out.str());
        }
        const char* readyStats = std::getenv("RR_IOP_READY_STATS47G");''')])
(out/'import-provenance47i.json').write_text(json.dumps(dict(changes=changes,
    base='47H corrected idle/ready candidate', reference_modified=False,
    status='Prepared, requires differential/state/performance validation',
    ram_mutations='All writes use markOwned; reset explicitly versions full RAM. ram() exposes const bytes only.',
    saturation='Page-version serial overflow permanently falls back to original decoder'),indent=2))
cmake=(root/'tools/IopCandidate47H.cmake').read_text().replace('47h','47i').replace('47H','47I')
cmake+='\ntarget_include_directories(rr_iop47i PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../src/core")\n'
cmake+='add_executable(racer_iop47i_import_cache "${CMAKE_CURRENT_LIST_DIR}/../tests/iop_import_cache47i.cpp")\n'
cmake+='target_include_directories(racer_iop47i_import_cache PRIVATE "${rr_iop47i}/src")\n'
cmake+='target_link_libraries(racer_iop47i_import_cache PRIVATE rr_iop47i)\n'
(root/'tools/IopCandidate47I.cmake').write_text(cmake)
test=(out/'differential47h.cpp').read_text()
test=test.replace('s.rfind("ready-cache ",0)==0;', 's.rfind("ready-cache ",0)==0 || s.rfind("import-cache ",0)==0;')
test=test.replace('_putenv_s("RR_IOP_READY_CACHE47G","0");','_putenv_s("RR_IOP_READY_CACHE47G","1");\n        _putenv_s("RR_IOP_IMPORT_CACHE47I","0");')
test=test.replace('        Emulator candidate(b);','        _putenv_s("RR_IOP_IMPORT_CACHE47I","1");\n        Emulator candidate(b);')
test=test.replace('    _putenv_s("RR_IOP_READY_STATS47G","1");','    _putenv_s("RR_IOP_READY_STATS47G","1");\n    _putenv_s("RR_IOP_IMPORT_STATS47I","1");')
test=test.replace('            if(line.rfind("ready-cache ",0)==0)', '            if(line.rfind("import-cache ",0)==0) std::cout<<line<<"\\n";\n            if(line.rfind("ready-cache ",0)==0)')
(out/'differential47i.cpp').write_text(test)
(root/'tools/LegacyHook47I.cmake').write_text('include("${RR_SHARED_ROOT}/tools/LegacyHook47F.cmake")\ncmake_language(DEFER CALL include "${RR_SHARED_ROOT}/tools/IopCandidate47I.cmake")\n')
evidence=root/'evidence/47I'
evidence.mkdir(exist_ok=False)
cmd=(root/'evidence/47H/build.cmd').read_text().replace('47h','47i').replace('47H','47I').replace(' --parallel 2',' racer_iop47i_import_cache --parallel 2')
(evidence/'build.cmd').write_text(cmd)
print(out)
