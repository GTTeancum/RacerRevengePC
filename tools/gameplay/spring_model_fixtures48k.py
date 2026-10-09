"""Reproducible generated-contact fixtures from unmodified original instructions.

Consumes checked-in instruction evidence and the 764-byte corrected saved replay
only. No full RAM image, ELF, patched instructions, or native implementation is
used to calculate expected results. Run with Python 3 from any directory.
"""
from pathlib import Path
import hashlib
import json
import math
import random
import struct
import sys

sys.dont_write_bytecode = True

from spring_fixture48j import SpringOriginal
from spring_impulse_fixtures48k import body as initialize_body, OFFSETS, OUTPUT_OFFSETS
from query_oracle48h import bits, fp, f32

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "evidence/springOrchestration48K"
DEPENDENCIES = {
    "evidence/springOrchestration48K/original-words.json": "4468f347b6aa73bd6b93a8d6422096614e84437db7681407be3b0ae35c771354",
    "evidence/spring48J/original-words.json": "b447e613b37c5ef0bbb503f4c084fbfc30a9715ce4fe5cb0db2d6c1ea9fc258e",
    "evidence/query48H/original-words.json": "eeb2bf1d207f466ecbc20704cf892b5f60444630a06cfc148b5deacbd9f99838",
}
SAVED_REPORT_SHA256 = "abee47a872d78a4e4bd5bb2257c360bd10025d7cd3d5752855df3e627a5ae941"
SAVED_DATA_SHA256 = "bb701510d1195d8c8174e9435c6297c62b2aaa632f6d7afe69b673b1f316e8af"
STATE_OFFSETS = (0x38, 0x3c, 0x40, 0x44, 0x74, 0x78, 0x130)
PARAMETER_SCALARS = (0x98, 0xb0, 0xb4, 0xb8, 0xbc, 0xc0, 0xcc, 0xd0)
PARAMETER_FLAGS = (0x5c, 0x64, 0x6c, 0x70)
TOGGLE_OFFSETS = (0x48, 0x4c, 0x50, 0x54, 0x58, 0x68)
FIELD_NAMES = ("worldVelocity", "localAngularVelocity", "force", "torque",
               "positionCorrection", "rotationCorrection", "velocityChange", "angularVelocityChange")
HELPERS = (0x296f10, 0x297610, 0x297390, 0x297190, 0x2983c0)
GP = 0x3dd970


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def load_words():
    hashes = {}
    words = {}
    for path in DEPENDENCIES:
        raw = (ROOT / path).read_bytes()
        hashes[path] = sha(raw)
        assert hashes[path] == DEPENDENCIES[path], (path, hashes[path])
        proof = json.loads(raw)
        for block in proof.get("blocks", proof.get("ranges", [])):
            rows = block.get("instructions", block.get("words", []))
            for row in rows:
                pc, word = row["pc"], row["word"]
                assert pc not in words or words[pc] == word, hex(pc)
                words[pc] = word
            if "blocks" in proof:
                raw_words = b"".join(struct.pack("<I", row["word"]) for row in rows)
                assert sha(raw_words) == block["sha256"], block["name"]
    return words, hashes


class Trace(SpringOriginal):
    def __init__(self, words, spring, data=b"", allowed=None):
        super().__init__(data, words)
        self.spring = spring
        self.allowed_source_addresses = allowed
        self.calls = []
        self.records = []
        self.body_calls = []

    def body_outputs(self, address):
        return [self.read(address + offset + i * 4) for offset in OUTPUT_OFFSETS for i in range(3)]

    def execute(self, pc):
        if pc == 0x296978:
            record, sp = self.g[17][0], self.g[29][0]
            self.records.append({
                "address": record, "penetration": self.f[20], "penetration_word": bits(self.f[20]),
                "worldPoint": self.vec(sp + 0x50, 4), "localPoint": self.vec(record + 0x10, 4),
                "normal": self.vec(record + 0x20, 4), "surfacePoint": self.vec(record + 0x30, 4),
                "scene": self.read(record + 0x40), "otherBody": self.read(record + 0x44),
                "material": self.read(record + 0x4c),
            })
        if pc in HELPERS:
            primary = self.read(self.g[4][0] + 0x30)
            self.calls.append({
                "pc": f"{pc:08X}", "spring": self.g[4][0], "primary": primary,
                "args": [self.g[i][0] for i in range(5, 8)], "f12": self.f[12],
                "worldCenter110": self.vec(self.spring + 0x110, 4),
                "contactVelocity120": self.vec(self.spring + 0x120, 4),
                "vectors": self.vec(self.g[5][0], 4) if self.g[5][0] else None,
                "point": self.vec(self.g[6][0], 4) if pc != 0x296f10 and self.g[6][0] else None,
            })
            # These fields were also captured before/after each saved replay.
            other = self.g[6 if pc == 0x296f10 else 7][0]
            self.body_calls.append({"pc": f"{pc:08X}", "primary": self.body_outputs(primary),
                                    "other": self.body_outputs(other) if other else None})
        return super().execute(pc)


def project(machine, spring, body_addresses, record_address, count, name, origin):
    """Project available bytes; never fill gaps in the original machine itself."""
    defaults = []

    def word(address, field):
        value = 0
        for i in range(4):
            if address + i in machine.overlay:
                byte = machine.overlay[address + i]
            elif (machine.allowed_source_addresses is not None and
                  address + i in machine.allowed_source_addresses):
                byte = machine.data[address + i]
            else:
                defaults.append({"field": field, "address": f"{address:08X}", "value": 0})
                return 0
            value |= byte << (8 * i)
        return value

    def vector(address, field, width=3):
        return [word(address + i * 4, f"{field}[{i}]") for i in range(width)]

    parameters = []
    for offset in (0x110, 0x120, 0x100):
        parameters.extend(vector(spring + offset, f"spring+{offset:X}"))
    parameters.extend(word(spring + off, f"spring+{off:X}") for off in PARAMETER_SCALARS)
    parameters.extend(bits(float(bool(word(spring + off, f"spring+{off:X}")))) for off in PARAMETER_FLAGS)
    bodies = []
    for address in body_addresses:
        scene = word(address + 0x288, f"body[{address:08X}].scenePointer")
        values = vector(scene + 0x100, f"body[{address:08X}].scene", 16)
        values += vector(address + 0x1e0, f"body[{address:08X}].localCenter")
        values += [word(address + off, f"body[{address:08X}]+{off:X}") for off in (0x26c, 0x270)]
        for i in range(3):
            values += vector(address + 0x140 + i * 16, f"body[{address:08X}].inertia[{i}]")
        for off in OFFSETS:
            values += vector(address + off, f"body[{address:08X}]+{off:X}")
        assert len(values) == 60
        bodies.append({"identity": address, "input": values})
    records = []
    for i in range(count):
        address = record_address + i * 0x60
        scene = word(address + 0x40, f"record[{i}].scenePointer")
        records.append({
            "vectors": sum((vector(address + off, f"record[{i}]+{off:X}") for off in (0x10, 0x20, 0x30)), []),
            "scene": vector(scene + 0x100, f"record[{i}].scene", 16) if scene else [0] * 16,
            "hasScene": bool(scene), "other": word(address + 0x44, f"record[{i}].other"),
            "material": word(address + 0x4c, f"record[{i}].material"),
            "flags": machine.read(address + 0x50, 1),
        })
    case = {"name": name, "origin": origin, "parameters": {
        "impact": parameters,
        "frictionTorque": word(spring + 0xc4, "spring+C4"),
        "frictionByMaterial": [word(spring + 0xd8 + i * 4, f"spring.material[{i}]") for i in range(10)],
        "toggles": [bool(word(spring + off, f"spring+{off:X}")) for off in TOGGLE_OFFSETS],
    }, "state": [word(spring + off, f"spring+{off:X}") for off in STATE_OFFSETS],
        "bodies": bodies, "records": records}
    assert len(parameters) == 21
    return case, defaults


def finish_case(case, machine, spring, record_address):
    for body in case["bodies"]:
        body["expected"] = machine.body_outputs(body["identity"])
    case["expected"] = {
        "state": [machine.read(spring + off) for off in STATE_OFFSETS],
        "center": [machine.read(spring + 0x110 + i * 4) for i in range(3)],
        "velocity": [machine.read(spring + 0x120 + i * 4) for i in range(3)],
        "flags": [machine.read(record_address + i * 0x60 + 0x50, 1) for i in range(len(case["records"]))],
    }
    case["calls"] = [dict(call, bodyStateBefore=state) for call, state in zip(machine.calls, machine.body_calls)]


def saved_cases(words):
    report_raw = (OUT / "coordinator-saved-replay.json").read_bytes()
    assert sha(report_raw) == SAVED_REPORT_SHA256
    report = json.loads(report_raw)
    raw = (OUT / "coordinator-saved-data.bin").read_bytes()
    assert sha(raw) == SAVED_DATA_SHA256
    assert len(raw) == report["source_bytes"] == 764
    assert sha(raw) == report["source_sha256"]
    sparse = bytearray(0x2000000)
    allowed = set()
    for span in report["source_slices"]:
        chunk = raw[span["offset"]:span["offset"] + span["bytes"]]
        assert len(chunk) == span["bytes"] and sha(chunk) == span["sha256"]
        address = span["address"]
        sparse[address:address + len(chunk)] = chunk
        allowed.update(range(address, address + len(chunk)))
    chain = report["chain"]
    assert chain == {"vehicle": 0xc5b0b0, "spring_slot": 0x354, "spring": 0xd3c570,
                     "primary": 0xd110a0, "detection_slot": 0x358, "detection": 0x95d0a0,
                     "detection_spring": 0xd3c570}
    spring = chain["spring"]
    cases, summaries, defaults = [], [], {}
    for recorded in report["runs"]:
        name = recorded["variant"]
        machine = Trace(words, spring, sparse, allowed)
        machine.g[28][0] = GP
        machine.g[4][0] = spring
        assert machine.read(spring + 0x30) == chain["primary"]
        record = machine.read(spring + 0x8c)
        if name != "saved":
            preparation = Trace(words, spring, sparse, allowed)
            preparation.g[28][0] = GP
            preparation.g[4][0] = spring
            preparation.g[29][0] = 0x1ff0000
            preparation.g[31][0] = 0xfffffff0
            pc = 0x2964c0
            while pc != 0x296978:
                target = preparation.execute(pc)
                if target is None:
                    pc += 4
                else:
                    if not preparation.annul:
                        assert preparation.execute(pc + 4) is None
                    pc = target
            point = preparation.vec(preparation.g[29][0] + 0x50)
            normal = preparation.vec(record + 0x20)
            machine.vector(record + 0x30, [f32(p + f32(n * .5)) for p, n in zip(point, normal)] + [1])
            if name == "positive-two-records":
                for off in range(0, 0x60, 4):
                    machine.write(record + 0x60 + off, machine.read(record + off))
                machine.write(spring + 0x84, 2)
            if name == "positive-other-body":
                machine.write(record + 0x44, 0xd117b0)
        addresses = [chain["primary"]] + ([0xd117b0] if name == "positive-other-body" else [])
        count = machine.read(spring + 0x84)
        case, missing = project(machine, spring, addresses, record, count, "saved-" + name, "corrected-saved-replay")
        before = {f"{b:08X}": {field: machine.body_outputs(b)[i * 3:i * 3 + 3]
                               for i, field in enumerate(FIELD_NAMES)} for b in addresses}
        returned = machine.run(0x2964c0)
        after = {f"{b:08X}": {field: machine.body_outputs(b)[i * 3:i * 3 + 3]
                              for i, field in enumerate(FIELD_NAMES)} for b in addresses}
        reproduced = {"variant": name, "return": returned, "steps": machine.steps, "calls": machine.calls,
                      "records": machine.records, "before": before, "after": after,
                      "spring_flags": {f"{off:X}": machine.read(spring + off) for off in (*STATE_OFFSETS[:-1], 0x84, 0x130)},
                      "worldCenter110": machine.vec(spring + 0x110, 4),
                      "contactVelocity120": machine.vec(spring + 0x120, 4),
                      "record_flags50": machine.read(record + 0x50, 1),
                      "coverage": [f"{pc:08X}" for pc in sorted(machine.coverage)]}
        assert reproduced == recorded, f"saved replay mismatch: {name}"
        finish_case(case, machine, spring, record)
        cases.append(case)
        defaults[case["name"]] = missing
        summaries.append({"variant": name, "steps": machine.steps, "source_bytes_read": len(machine.source_reads),
                          "recorded_replay_equal": True, "guarded": True})
    return cases, {"report_sha256": sha(report_raw), "data_sha256": sha(raw), "source_bytes": len(raw),
                   "chain": chain, "verification": summaries,
                   "projection_zero_defaults": defaults,
                   "projection_policy": "Zero defaults are typed-projection scaffolding for uncaptured, unused native input words, not claims about original saved values. No default is inserted into original oracle memory. Every original source read is guarded by preserved byte membership. The saved report's chain is asserted; this sparse artifact does not re-read uncaptured vehicle/detection links."}


def transformed(matrix, point):
    return [f32(f32(f32(f32(matrix[i] * point[0]) + f32(matrix[4 + i] * point[1])) +
                       f32(matrix[8 + i] * point[2])) + matrix[12 + i]) for i in range(3)]


def synthetic_cases(words, count=192):
    rng = random.Random(0x48c2964c0)
    cases, coverage, branches = [], set(), {}
    totals = {"velocity_changing_cases": 0, "repeated_other_body_cases": 0,
              "persisted_velocity_cases": 0, "impact_changed_friction_body_state_cases": 0}

    def vec(scale):
        return [f32(rng.uniform(-scale, scale)) for _ in range(3)]

    def matrix(index):
        angle = rng.uniform(-3, 3)
        c, s = f32(math.cos(angle)), f32(math.sin(angle))
        result = [c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0] + vec(20) + [1]
        if index % 3 == 0:
            for i in (0, 1, 2, 4, 5, 6, 8, 9, 10):
                result[i] = f32(result[i] + rng.uniform(-.08, .08))
        return result

    for index in range(count):
        spring, record = 0x10000, 0x18000
        addresses, scenes = [0x11000, 0x12000, 0x13000], [0x14000, 0x15000, 0x16000]
        machine = Trace(words, spring)
        machine.g[28][0] = GP
        machine.g[4][0] = spring
        for address in (GP - 0x7358, GP - 0x7354):
            machine.write(address, 1, 1)
        machine.vector(0x4435a0, [0, 0, 0, 1])
        body_values = []
        for body_index, (address, scene) in enumerate(zip(addresses, scenes)):
            mass = f32(rng.uniform(100, 1900))
            if index % 16 in (0, 1, 2):
                mass = fp(0x447a0000 + index % 16 - 1)
            inertia = [f32(rng.uniform(.001, .015)) if row == col else 0
                       for col in range(3) for row in range(3)]
            values = (matrix(index + body_index) + vec(2) + [mass, f32(1 / mass)] + inertia +
                      vec(12) + vec(1) + vec(3) + vec(.3) + vec(100) + vec(10) +
                      ([0] * 6 if index % 3 else vec(.1) + vec(.01)) + vec(.1) + vec(.01))
            if index == 3:
                values[:19] = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0]
                values[19:30] = [100, .01, .01, 0, 0, 0, .01, 0, 0, 0, .01]
                values[30:36] = [7, -8, 2, .1, .2, .3] if body_index == 0 else [0] * 6
                values[36:] = [0] * 24
            values = [f32(v) for v in values]
            assert len(values) == 60
            initialize_body(machine, address, scene, values)
            body_values.append(values)
        machine.write(spring + 0x30, addresses[0])
        p = vec(10) + vec(10) + vec(2) + [f32(rng.uniform(.1, 1.5)) for _ in range(8)]
        # Keep response toggles independent of the impact helper's own modes.
        p += [float((index >> shift) & 1) for shift in (4, 5, 6, 7)]
        if index == 3:
            p[9:17] = [.97, 1, 1, .25, .25, .25, .2, .1]
            p[17:] = [0, 1, 0, 0]
        for i, off in enumerate((0x110, 0x120, 0x100)):
            machine.vector(spring + off, p[i * 3:i * 3 + 3] + [1])
        for off, value in zip(PARAMETER_SCALARS, p[9:17]):
            machine.storef(spring + off, value)
        for off, value in zip(PARAMETER_FLAGS, p[17:]):
            machine.write(spring + off, int(value))
        toggles = [0, 0, (index >> 2) & 1, (index >> 3) & 1, 0, 0]
        if index == 3:
            toggles[2:4] = [1, 1]
        for off, value in zip(TOGGLE_OFFSETS, toggles):
            machine.write(spring + off, value)
        for off in STATE_OFFSETS:
            machine.write(spring + off, rng.randrange(4))
        machine.storef(spring + 0xc4, f32(rng.uniform(-1, 2)))
        for i in range(10):
            machine.storef(spring + 0xd8 + i * 4, f32((i + 1) / 20))
        record_count = index % 4
        machine.write(spring + 0x84, record_count)
        machine.write(spring + 0x8c, record)
        for i in range(record_count):
            address = record + i * 0x60
            # All record bytes initialized; unused words are explicit test data.
            for off in range(0, 0x60, 4):
                machine.write(address + off, 0)
            local = vec(5)
            normal = vec(1)
            length = math.sqrt(sum(x * x for x in normal))
            normal = [f32(x / length) for x in normal]
            if index % 8 in (1, 2, 3):
                normal = [.6, fp(0x3f4ccccd + index % 8 - 2), 0]
            has_scene = (index + i) % 2 == 0
            record_scene = 0x19000 + i * 0x200
            chosen_matrix = matrix(index + i) if has_scene else body_values[0][:16]
            other = addresses[1 + (index % 2)] if (index + i) % 3 else 0
            if record_count == 3 and index % 2:
                other = addresses[1]  # Exercise updates to the same other body.
            depth = (0, -.5, .25, .75)[(index // 4 + i) % 4]
            if index == 3:
                local = ([1, 1, 0], [-1, 2, 1], [2, 1, -1])[i]
                normal, depth, has_scene, other = [0, 1, 0], .5, False, addresses[1]
                chosen_matrix = body_values[0][:16]
            world = transformed(chosen_matrix, local)
            surface = [f32(x + f32(depth * n)) for x, n in zip(world, normal)]
            for off, value in ((0x10, local), (0x20, normal), (0x30, surface)):
                machine.vector(address + off, value + [1])
            if has_scene:
                machine.vector(record_scene + 0x100, chosen_matrix)
            machine.write(address + 0x40, record_scene if has_scene else 0)
            machine.write(address + 0x44, other)
            machine.write(address + 0x4c, (index + i) % 10)
            machine.write(address + 0x50, rng.randrange(256), 1)
        case, defaults = project(machine, spring, addresses, record, record_count,
                                 f"synthetic-{index:03d}", "explicitly-initialized-original-words")
        assert not defaults, defaults
        assert machine.run(0x2964c0) == 1
        finish_case(case, machine, spring, record)
        assert not machine.source_reads, f"synthetic input escaped initialization: {index}"
        for b in case["bodies"]:
            assert all(math.isfinite(fp(v)) for v in b["expected"])
        assert all(math.isfinite(fp(v)) for key in ("center", "velocity") for v in case["expected"][key])
        if any(b["input"][30:36] != b["expected"][:6] for b in case["bodies"]):
            totals["velocity_changing_cases"] += 1
        others = [r["other"] for r in case["records"] if r["other"]]
        if len(others) != len(set(others)):
            totals["repeated_other_body_cases"] += 1
        prep = [call for call in machine.body_calls if call["pc"] == "00296F10"]
        if prep and any(call["primary"][:6] != prep[0]["primary"][:6] for call in prep[1:]):
            totals["persisted_velocity_cases"] += 1
        impact_seen = None
        for call in machine.body_calls:
            if call["pc"] == "00297610":
                impact_seen = call
            elif call["pc"] == "00297390" and impact_seen is not None:
                if impact_seen["primary"][:6] != call["primary"][:6]:
                    totals["impact_changed_friction_body_state_cases"] += 1
                    break
        coverage.update(machine.coverage)
        for pc, taken in machine.branches.items():
            branches.setdefault(pc, set()).update(taken)
        cases.append(case)
    assert all(value > 0 for value in totals.values()), totals
    consumed_materials = {r["material"] for c in cases if c["parameters"]["toggles"][2]
                          for r, flag in zip(c["records"], c["expected"]["flags"]) if flag & 2}
    assert consumed_materials == set(range(10))
    assert {len(c["records"]) for c in cases} == {0, 1, 2, 3}
    active_impact = [c for c in cases if any(call["pc"] == "00297610" for call in c["calls"])]
    for mode in range(17, 21):
        assert {c["parameters"]["impact"][mode] for c in active_impact} == {bits(0.), bits(1.)}, mode
    assert {tuple(c["parameters"]["toggles"][2:4]) for c in cases if c["records"]} == {
        (False, False), (False, True), (True, False), (True, True)}
    return cases, {"seed": "48C2964C0", "cases": count, "source_reads": 0,
                   "coverage_assertions": totals, "coverage": [f"{pc:08X}" for pc in sorted(coverage)],
                   "branches": {f"{pc:08X}": sorted(taken) for pc, taken in sorted(branches.items())}}


def main():
    words, hashes = load_words()
    saved, saved_report = saved_cases(words)
    synthetic, synthetic_report = synthetic_cases(words)
    report = {"schema": "spring-generated-contact48k-v1", "dependencies": hashes,
              "arithmetic": "Original instructions, ordinary individually rounded IEEE binary32; current scenes; no physical EE exceptional-mode claim.",
              "original_instructions_patched": False, "saved_replay": saved_report,
              "synthetic": synthetic_report, "cases": saved + synthetic}
    raw = (json.dumps(report, indent=2, allow_nan=False) + "\n").encode()
    destination = OUT / "coordinator-native-fixtures.json"
    destination.write_bytes(raw)
    print(json.dumps({"path": str(destination.relative_to(ROOT)), "sha256": sha(raw),
                      "saved_cases": len(saved), "synthetic_cases": len(synthetic),
                      "synthetic_assertions": synthetic_report["coverage_assertions"]}))


if __name__ == "__main__":
    main()
