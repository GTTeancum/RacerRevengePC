"""Independent original219810 merge interval: genuine ELF words, no native oracle.

Only [219B1C,219BD0) executes. Stopping before219BD0 is an interval boundary,
not a function return. No122350/world/detector call is stubbed or executed.
"""
import hashlib
import json
import random
import struct
import sys
from pathlib import Path

sys.dont_write_bytecode = True
from query_oracle48h import Original

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "evidence/candidateMerge48S"
ELF = "work/native/project/disc/SLUS_202.68"
ELF_SHA = "c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30"
MAP = "evidence/vehicleContact48R/map/original-words.json"
MAP_SHA = "717452d4d1f70808eda3cf6cba3391da96005c1ef8afe3c1b5d2151e6282d99c"
INTERPRETER_SHA = "ff6a9069093e7f157aea7b143a3a72581791af937fe1b6dde7e3d52271f72c79"
START, END, VEHICLE = 0x219b1c, 0x219bd0, 0x10000
COUNT, OUTPUT, SECOND_COUNT, SECOND, FIRST_COUNT, FIRST = (VEHICLE + x for x in
    (0x384, 0x388, 0x488, 0x48c, 0x58c, 0x590))


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


class AuditedOriginal(Original):
    """Log real executor memory operations; never replace instruction effects."""
    def __init__(self, words):
        super().__init__(data=b"", words=words)
        self.auditing = False
        self.read_log, self.write_log = [], []

    def read(self, address, size=4):
        value = super().read(address, size)
        if self.auditing:
            self.read_log.append((self.pc, address, size, value))
        return value

    def write(self, address, value, size=4):
        if self.auditing:
            self.write_log.append((self.pc, address, size, value & 0xffffffff))
        super().write(address, value, size)

    def interval(self):
        self.g[19][0] = VEHICLE
        self.auditing = True
        pc = START
        while pc != END:
            assert START <= pc < END, f"unexpected interval escape {pc:08x}"
            target = self.execute(pc)
            if target is None:
                pc += 4
            else:
                if not self.annul:
                    assert self.execute(pc + 4) is None, "branch in delay slot"
                pc = target
        self.auditing = False
        self.stop_pc = pc


def freeze():
    raw = (ROOT / ELF).read_bytes()
    assert sha(raw) == ELF_SHA
    assert raw[:7] == b"\x7fELF\x01\x01\x01"
    phoff = struct.unpack_from("<I", raw, 28)[0]
    phsize, phcount = struct.unpack_from("<HH", raw, 42)
    assert phsize == 32
    loads = [struct.unpack_from("<8I", raw, phoff + phsize * i) for i in range(phcount)]
    loads = [p for p in loads if p[0] == 1 and p[2] <= START and END <= p[2] + p[4]]
    assert len(loads) == 1
    segment = loads[0]
    assert segment == (1, 0x80, 0x100000, 0x100000, 2972160, 3470848, 7, 128)
    source = (ROOT / MAP).read_bytes()
    assert sha(source) == MAP_SHA
    annotations = {row["pc"]: row for b in json.loads(source)["blocks"] for row in b["instructions"]}
    rows = []
    for pc in range(START, END, 4):
        word = struct.unpack_from("<I", raw, segment[1] + pc - segment[2])[0]
        assert word == annotations[pc]["word"]
        rows.append(dict(pc=pc, word=word, assembly=annotations[pc]["assembly"]))
    # No helper call, return patch, call interception or injected instruction.
    assert all(row["word"] >> 26 != 3 for row in rows)
    frozen = dict(schema=1, elf=ELF, elf_sha256=ELF_SHA, pt_load=list(segment),
        annotation_source=MAP, annotation_sha256=MAP_SHA, start=START, end_exclusive=END,
        boundary="Stop before detector dereference219BD0; not a natural function return",
        interval_sha256=sha(raw[segment[1]+START-segment[2]:segment[1]+END-segment[2]]),
        instructions=rows)
    (OUT / "original-words.json").write_text(json.dumps(frozen, indent=2) + "\n")
    (OUT / "original-words.txt").write_text("\n".join(
        f'{r["pc"]:08X} {r["word"]:08X} {r["assembly"]}' for r in rows) + "\n")
    return {row["pc"]: row["word"] for row in rows}


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    assert sha((ROOT / "tools/gameplay/query_oracle48h.py").read_bytes()) == INTERPRETER_SHA
    words = freeze()
    rng = random.Random(0x48a219b1c)
    cases, audits, coverage, branches = [], [], set(), {}
    total_steps = 0

    def fixture(name, first_count, second_count, first_backing=None, second_backing=None,
                repeated=False):
        nonlocal total_steps
        a = min(max(first_count, 0), 64)
        b = min(max(second_count, 0), 64-a)
        first_backing = a if first_backing is None else first_backing
        second_backing = b if second_backing is None else second_backing
        assert a <= first_backing <= 64 and b <= second_backing <= 64
        first = [rng.getrandbits(32) for _ in range(first_backing)]
        second = [rng.getrandbits(32) for _ in range(second_backing)]
        if repeated:
            first = [(0, 17, 17, 0xffffffff, 0x80000000)[i % 5] for i in range(first_backing)]
            second = [(17, 0, 0xffffffff, 17, 0x80000000)[i % 5] for i in range(second_backing)]
        before = [rng.getrandbits(32) for _ in range(64)]
        machine = AuditedOriginal(words)
        machine.write(COUNT, rng.getrandbits(32))
        machine.write(FIRST_COUNT, first_count)
        machine.write(SECOND_COUNT, second_count)
        for base, values in ((OUTPUT, before), (FIRST, first), (SECOND, second)):
            for i, value in enumerate(values):
                machine.write(base+i*4, value)
        initial = machine.overlay.copy()
        machine.interval()
        assert machine.data == b"" and not machine.source_reads
        actual_count = machine.read(COUNT)
        actual = [machine.read(OUTPUT+i*4) for i in range(64)]
        # Expected outputs are ONLY executed memory. Formula assertions below
        # independently check reached addresses, never generate expected words.
        list_reads = [(pc, addr) for pc, addr, size, value in machine.read_log
                      if pc in (0x219b40, 0x219b98)]
        assert list_reads == ([(0x219b40, FIRST+i*4) for i in range(a)] +
                              [(0x219b98, SECOND+i*4) for i in range(b)])
        assert sum(pc == 0x219b68 for pc, *_ in machine.read_log) == max(first_count, 0)+1
        assert sum(pc == 0x219bc0 for pc, *_ in machine.read_log) == max(second_count, 0)+1
        assert sum(pc == 0x219b28 for pc, *_ in machine.read_log) == max(first_count, 0)
        assert sum(pc == 0x219b80 for pc, *_ in machine.read_log) == max(second_count, 0)
        assert actual_count == a+b and machine.stop_pc == END
        writable = set(range(COUNT, COUNT+4)) | set(range(OUTPUT, OUTPUT+actual_count*4))
        assert all(initial.get(addr) == value for addr, value in machine.overlay.items() if addr not in writable)
        assert all(size == 4 and set(range(addr, addr+4)) <= writable
                   for _, addr, size, _ in machine.write_log)
        assert actual[actual_count:] == before[actual_count:]
        cases.append(dict(name=name, first_count=first_count, second_count=second_count,
            first=first, second=second, before=before, expected_count=actual_count, expected=actual))
        audits.append(dict(name=name, first_count=first_count, second_count=second_count,
            first_backing=first_backing, second_backing=second_backing,
            first_list_loads=a, second_list_loads=b,
            first_count_loads=max(first_count, 0)+1, second_count_loads=max(second_count, 0)+1,
            output_count_loads=max(first_count, 0)+max(second_count, 0)+a+b,
            steps=machine.steps, stop_pc=f"{machine.stop_pc:08X}", source_reads=0,
            reads_sha256=sha(json.dumps(machine.read_log, separators=(",", ":")).encode()),
            writes_sha256=sha(json.dumps(machine.write_log, separators=(",", ":")).encode()),
            branches={f"{pc:08X}": sorted(v) for pc, v in sorted(machine.branches.items())}))
        coverage.update(machine.coverage)
        for pc, outcomes in machine.branches.items():
            branches.setdefault(pc, set()).update(outcomes)
        total_steps += machine.steps

    fixture("zero counts preserve all output slots", 0, 0)
    fixture("negative counts skip all list reads", -1, -7)
    fixture("minimum signed counts", -0x80000000, -0x80000000)
    fixture("negative first and positive second", -2, 9)
    fixture("positive first and negative second", 13, -3)
    fixture("exact first64 with absent second backing", 64, 37)
    fixture("first65 reads only64 and still tests count66 times", 65, 0)
    fixture("second65 reads only64", 0, 65)
    fixture("combined63 initialized unused tails", 31, 32, 64, 64)
    fixture("combined64 first31 then33", 31, 33)
    fixture("combined65 second suffix unread", 31, 34)
    fixture("first63 then large second only one read", 63, 4096)
    fixture("first saturated large logical counts absent second", 4096, 4096)
    fixture("zero raw IDs and repeats remain ordered", 12, 13, 64, 64, True)
    fixture("empty logical lists with initialized tails", 0, 0, 64, 64)
    fixture("negative logical lists with initialized tails", -100, -300, 64, 64)
    fixture("first32 second32 repeats exactly64", 32, 32, repeated=True)
    fixture("first1 second64 boundary", 1, 64)
    for index in range(256):
        c1, c2 = (rng.choice((-0x80000000, -7, -1, 0, 1, 31, 32, 63, 64, 65, 79,
                              rng.randrange(0, 130))) for _ in range(2))
        a = min(max(c1, 0), 64)
        b = min(max(c2, 0), 64-a)
        fixture(f"seeded ordered merge {index:03}", c1, c2,
                rng.randrange(a, 65), rng.randrange(b, 65), index % 3 == 0)

    # Strict initialization negative controls: each required source read fails,
    # whereas every case above succeeds with absent skipped backing when named.
    negative_controls = []
    for first_count, second_count, missing in ((1, 0, FIRST), (0, 1, SECOND), (0, 0, FIRST_COUNT)):
        machine = AuditedOriginal(words)
        machine.write(FIRST_COUNT, first_count)
        machine.write(SECOND_COUNT, second_count)
        for address in range(missing, missing+4):
            machine.overlay.pop(address, None)
        try:
            machine.interval()
            raise AssertionError("missing required word unexpectedly succeeded")
        except ValueError as error:
            assert "uninitialized fixture read" in str(error)
            negative_controls.append(dict(missing=f"{missing:08X}", error=str(error)))
    assert coverage == set(words), sorted(set(words)-coverage)
    assert len(words) == 45
    for pc in (0x219b30, 0x219b70, 0x219b88, 0x219bc8):
        assert branches[pc] == {False, True}
    raw = (json.dumps(dict(schema=1, cases=cases), separators=(",", ":")) + "\n").encode()
    (OUT / "native-fixtures.json").write_bytes(raw)
    (OUT / "read-audit.json").write_text(json.dumps(dict(cases=audits,
        required_read_negative_controls=negative_controls), indent=2) + "\n")
    report = dict(schema=1, cases=len(cases), steps=total_steps, start=f"{START:08X}",
        stop_before=f"{END:08X}", original_instruction_count=len(words), source_reads=0,
        data_bytes=0, strict_initialized_reads=True, instruction_patches=0, helper_stubs=0,
        native_expected_generation=False, elf_sha256=ELF_SHA, annotation_sha256=MAP_SHA,
        interpreter_sha256=INTERPRETER_SHA, fixture_sha256=sha(raw),
        original_words_sha256=sha((OUT / "original-words.json").read_bytes()),
        generator_sha256=sha(Path(__file__).read_bytes()),
        coverage=[f"{pc:08X}" for pc in sorted(coverage)],
        branches={f"{pc:08X}": sorted(v) for pc, v in sorted(branches.items())},
        scope="Isolated first-list-then-second-list merge after122350; no full219810 return or enumeration claim",
        native_only="Full declared-span alias/capacity guards and immutable-count saturated-loop collapsing; INT_MAX positive tested natively, not billions of original iterations")
    (OUT / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({k: v for k, v in report.items() if k not in ("coverage", "branches")}, indent=2))


if __name__ == "__main__":
    main()
