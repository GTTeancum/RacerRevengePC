"""Bit-exact append/reset fixtures from unpatched original2963E0/296490.

No RAM, ELF, native implementation, floating point arithmetic or default-filled
reads. Every fixture object byte is explicitly initialized. The original-word
executor is test tooling only; production accepts native records and identities.
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
OUT = ROOT / "evidence/springRecords48N"
SOURCE = "evidence/springOrchestration48K/original-words.json"
SOURCE_SHA = "4468f347b6aa73bd6b93a8d6422096614e84437db7681407be3b0ae35c771354"
SPRING, RECORDS, ARGUMENTS = 0x10000, 0x20000, 0x30000
STATE_OFFSETS = (0x38, 0x3c, 0x40, 0x44, 0x74, 0x78, 0x130, 0x7c, 0x84, 0x88, 0xd4)
SCENES = {0: 0, 0x51000: 7, 0x52000: 9, 0x53000: 14}
BODIES = {0: 0, 0x61000: 3, 0x62000: 13, 0x63000: 21}
SPECIAL = (0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345,
           0x7f812345, 0xff812345, 1, 0x80000001, 0x3f800000,
           0x7f7fffff, 0xff7fffff, 0xdeadbeef, 0xffffffff, 0x12345678, 0x00800000)


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def main():
    source = (ROOT / SOURCE).read_bytes()
    assert sha(source) == SOURCE_SHA
    proof = json.loads(source)
    blocks = []
    words = {}
    for block in proof["blocks"]:
        rows = block["instructions"]
        assert sha(b"".join(struct.pack("<I", row["word"]) for row in rows)) == block["sha256"]
        words.update((row["pc"], row["word"]) for row in rows)
    for name, start, end in (("append2963E0", 0x2963e0, 0x296484), ("reset296490", 0x296490, 0x2964b8)):
        rows = [row for block in proof["blocks"] for row in block["instructions"] if start <= row["pc"] < end]
        assert [row["pc"] for row in rows] == list(range(start, end, 4))
        blocks.append(dict(name=name, start=start, end=end,
                           sha256=sha(b"".join(struct.pack("<I", row["word"]) for row in rows)),
                           instructions=rows))
    # Only these instructions are available, with no patches or call stubs.
    words = {row["pc"]: row["word"] for block in blocks for row in block["instructions"]}
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "original-words.json").write_text(json.dumps(dict(source=SOURCE, source_sha256=SOURCE_SHA,
        elf_sha256=proof["elf_sha256"], blocks=blocks), indent=2) + "\n")
    (OUT / "original-words.txt").write_text("\n".join(
        f'{row["pc"]:08X} {row["word"]:08X} {row["assembly"]}' for b in blocks for row in b["instructions"]) + "\n")
    rng = random.Random(0x48a2963e0)
    cases, coverage, branches, steps, operations, appended, saturated, resets = [], set(), {}, 0, 0, 0, 0, 0

    def state(machine):
        return [machine.read(SPRING + offset) for offset in STATE_OFFSETS]

    def records(machine, slots):
        result = []
        for index in range(slots):
            row = [machine.read(RECORDS + index * 0x60 + j * 4) for j in range(24)]
            row[16], row[17] = SCENES[row[16]], BODIES[row[17]]
            result.append(row)
        return result

    def fixture(name, count, capacity, plan, slots=None):
        nonlocal steps, operations, appended, saturated, resets
        slots = max(capacity, 1) + 2 if slots is None else slots
        machine = Original(words=words)
        for offset in range(0, 0x140, 4):
            machine.write(SPRING + offset, rng.getrandbits(32))
        machine.write(SPRING + 0x84, count)
        machine.write(SPRING + 0x88, capacity)
        machine.write(SPRING + 0x8c, RECORDS)
        for index in range(slots):
            row = [rng.getrandbits(32) for _ in range(24)]
            row[16], row[17] = rng.choice(tuple(SCENES)), rng.choice(tuple(BODIES))
            for j, value in enumerate(row):
                machine.write(RECORDS + index * 0x60 + j * 4, value)
        case = dict(name=name, initial_state=state(machine), initial_records=records(machine, slots), operations=[])
        previous_payload = None
        for index, operation in enumerate(plan):
            initial_object = [machine.read(SPRING + i) for i in range(0, 0x140, 4)]
            initial_records = records(machine, slots)
            op = dict(kind=operation)
            if operation == "reset":
                machine.g[4][0] = SPRING
                machine.run(0x296490)
                changed = {0x84, 0x38, 0x130, 0x3c, 0x74, 0x78, 0x40, 0x44}
                assert records(machine, slots) == initial_records
                resets += 1
            else:
                if operation == "repeat" and previous_payload is not None:
                    payload, other, scene, material = previous_payload
                else:
                    payload = [rng.getrandbits(32) for _ in range(16)]
                    if index % 2 == 0:
                        payload = list(SPECIAL[index % 16:] + SPECIAL[:index % 16])
                    other, scene, material = rng.choice(tuple(BODIES)), rng.choice(tuple(SCENES)), rng.getrandbits(32)
                    previous_payload = payload, other, scene, material
                for j, value in enumerate(payload):
                    machine.write(ARGUMENTS + j * 4, value)
                for register, value in zip(range(4, 12), (SPRING, ARGUMENTS, ARGUMENTS + 16,
                        ARGUMENTS + 32, ARGUMENTS + 48, other, scene, material)):
                    machine.g[register][0] = value
                before_count = machine.read(SPRING + 0x84)
                machine.run(0x2963e0)
                success = machine.read(SPRING + 0x84) != before_count
                op.update(kind="append", input=payload + [SCENES[scene], BODIES[other], material],
                          status="Appended" if success else "Saturated")
                if success:
                    appended += 1
                    changed = {0x84, 0xd4}
                    now = records(machine, slots)
                    for j, (before, after) in enumerate(zip(initial_records, now)):
                        if j != before_count:
                            assert before == after
                        else:
                            assert before[18] == after[18] and before[20:] == after[20:]
                else:
                    saturated += 1
                    changed = set()
                    assert records(machine, slots) == initial_records
            for j, word in enumerate(initial_object):
                if j * 4 not in changed:
                    assert machine.read(SPRING + j * 4) == word
            assert not machine.source_reads
            op.update(state=state(machine), records=records(machine, slots))
            case["operations"].append(op)
            operations += 1
        coverage.update(machine.coverage)
        for pc, values in machine.branches.items():
            branches.setdefault(pc, set()).update(values)
        steps += machine.steps
        cases.append(case)

    fixture("zero capacity preserves state and records", 0, 0, ["append", "repeat", "reset", "append"])
    fixture("one slot preserves stale margin flags and tail", 0, 1, ["append", "repeat", "reset", "repeat"])
    fixture("duplicates remain ordered then saturate", 0, 4, ["append", "repeat", "append", "repeat", "repeat", "reset", "repeat"])
    fixture("exact capacity is original saturation", 4, 4, ["append", "reset", "append"])
    fixture("count above capacity is original saturation", 7, 4, ["append", "reset", "append"])
    fixture("maximum signed count safely saturates", 0x7fffffff, 2, ["append", "reset", "append"])
    fixture("reset twice preserves persistent point count", 2, 3, ["reset", "reset", "append", "reset"])
    fixture("last available slot and extra native backing", 2, 3, ["append", "append"], slots=8)
    for index in range(256):
        capacity = rng.randrange(0, 9)
        count = rng.randrange(capacity + 3)
        plan = [rng.choice(("append", "append", "repeat", "reset")) for _ in range(6)]
        fixture(f"seeded byte-pattern sequence {index:03}", count, capacity, plan)
    assert len(coverage) == len(words), (len(coverage), len(words), sorted(set(words) - coverage))
    assert branches[0x296420] == {False, True}
    raw = (json.dumps(dict(schema=1, state_offsets=[f"{i:X}" for i in STATE_OFFSETS], cases=cases), separators=(",", ":")) + "\n").encode()
    (OUT / "native-fixtures.json").write_bytes(raw)
    report = dict(cases=len(cases), operations=operations, appends=appended, saturations=saturated, resets=resets,
        fixture_sha256=sha(raw), source=SOURCE, source_sha256=SOURCE_SHA,
        original_words_sha256=sha((OUT / "original-words.json").read_bytes()),
        interpreter_sha256=sha((ROOT / "tools/gameplay/query_oracle48h.py").read_bytes()),
        steps=steps, source_reads=0, initialized_object_bytes=0x140,
        coverage=[f"{pc:08X}" for pc in sorted(coverage)],
        branches={f"{pc:08X}": sorted(values) for pc, values in sorted(branches.items())},
        identity_projection=dict(scenes=SCENES, bodies=BODIES),
        scope="Synthetic initialized objects, raw bit copies; no RAM capture, runtime state claim, native oracle, instruction patches, or call interception.",
        native_only_guards="Negative metadata, undersized backing, aliasing, null/misaligned/overflow extents are tested natively; not misrepresented as original policy.")
    (OUT / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({k: v for k, v in report.items() if k not in ("coverage", "branches", "identity_projection")}, indent=2))


if __name__ == "__main__":
    main()
