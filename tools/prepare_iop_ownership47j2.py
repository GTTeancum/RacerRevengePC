"""Preserve the rejected 47J candidate and repair its allocation-free path."""
from pathlib import Path
import hashlib
import json
import shutil
import argparse
root=Path(__file__).resolve().parents[1]
out=root/'build/iop47J2'
parser=argparse.ArgumentParser()
parser.add_argument('--resume-partial',action='store_true')
args=parser.parse_args()
if out.exists():
    assert args.resume_partial and not (out/'free-path-correction.json').exists()
else:
    shutil.copytree(root/'build/iop47J',out)
p=out/'src/emulator/core/iop_memory.cpp'
source=root/'build/iop47J/src/emulator/core/iop_memory.cpp'
text=source.read_text(); before=hashlib.sha256(source.read_bytes()).hexdigest()
a='''        std::fill(m_owned.begin() + block->address,
                  m_owned.begin() + block->address + block->size,
                  uint8_t{0});'''
assert text.count(a)==1
text=text.replace(a,'        m_ownership.release(block->address, block->size);')
assert text.count('m_owned')==3, 'Re-audit all remaining backing-vector accesses'
if args.resume_partial:
    assert p.read_text()==text, 'Unexpected partial candidate contents'
p.write_text(text,encoding='utf-8',newline='\n')
header=out/'src/emulator/core/iop_memory.h'
body=header.read_text()
assert body.count('        IopMemory();')==1
body=body.replace('        IopMemory();','''        IopMemory();
        // Devices and the bitmap view retain references into this owner.
        IopMemory(const IopMemory&) = delete;
        IopMemory& operator=(const IopMemory&) = delete;
        IopMemory(IopMemory&&) = delete;
        IopMemory& operator=(IopMemory&&) = delete;''')
header.write_text(body,encoding='utf-8',newline='\n')
(out/'free-path-correction.json').write_text(json.dumps(dict(
    before_sha256=before,after_sha256=hashlib.sha256(p.read_bytes()).hexdigest(),
    original_candidate='build/iop47J',
    failure='Free still used guest-byte indexes into a word vector; ownership mismatch and scheduler process access violation.',
    correction='Release exactly the freed byte range through the bitmap.',
    status='Prepared; must rerun original-memory differential, all checks and native validation'),indent=2))
cmake=(root/'tools/IopCandidate47J.cmake').read_text().replace('47j','47j2').replace('47J','47J2')
cmake=cmake.replace('tests/iop_ownership47j2.cpp','tests/iop_ownership47j.cpp')
cmake=cmake.replace('memory_reference47j2.cpp','memory_reference47j.cpp')
cmake+='\ntarget_compile_definitions(racer_iop47j2_memory PRIVATE RR_OWNERSHIP_LIFETIME_GUARD)\n'
(root/'tools/IopCandidate47J2.cmake').write_text(cmake)
shutil.copy2(out/'differential47j.cpp',out/'differential47j2.cpp')
(root/'tools/LegacyHook47J2.cmake').write_text('include("${RR_SHARED_ROOT}/tools/LegacyHook47F.cmake")\ncmake_language(DEFER CALL include "${RR_SHARED_ROOT}/tools/IopCandidate47J2.cmake")\n')
evidence=root/'evidence/47J2';evidence.mkdir(exist_ok=False)
cmd=(root/'evidence/47J/build.cmd').read_text().replace('47j','47j2').replace('47J','47J2')
(evidence/'build.cmd').write_text(cmd)
runner=(root/'tools/Run-Native47J.ps1').read_text().replace('47J','47J2').replace('47j','47j2')
(root/'tools/Run-Native47J2.ps1').write_text(runner)
print(out)
