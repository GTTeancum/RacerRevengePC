"""Full original2964C0 response fixtures; no production-derived expectations.

The prior 196 cases are retained verbatim. New cases execute the unmodified
point and generated coordinator loops plus their original response helpers.
Every synthetic data byte read is explicitly initialized; there is no RAM image,
zero-filled fallback, instruction substitution, or native-code expected value.
"""
from pathlib import Path
import hashlib
import json
import math
import random
import struct
import sys

sys.dont_write_bytecode = True

from spring_model_fixtures48k import (
    ROOT, Trace, load_words, project, finish_case, transformed, initialize_body,
    STATE_OFFSETS, PARAMETER_SCALARS, PARAMETER_FLAGS, TOGGLE_OFFSETS, HELPERS, GP,
)
from query_oracle48h import bits, fp, f32

OUT = ROOT / "evidence/springResponse48L"
BASE = "evidence/springOrchestration48K/coordinator-native-fixtures.json"
BASE_SHA256 = "bc3004473c401fc79a44cf8c77120ba9d01e017b7f6b535b94bf86a5dd2dce5d"
SECONDARY = "evidence/springResponse48L/secondary-original-words.json"
SECONDARY_SHA256 = "ac3ccaa51207b47d2ba41e7afe6d0542c66c11eb6b1b90ebee50e9a224b62b4c"
RESPONSE_OFFSETS = (0x90, 0x94, 0x9c, 0xa0, 0xa4, 0xa8)
SEED = 0x48d2964c0
PREP, IMPACT, FRICTION, FORCE, SECONDARY_PC = (
    "00296F10", "00297610", "00297390", "00297190", "002983C0")


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def original_words():
    words, hashes = load_words()
    raw = (ROOT / SECONDARY).read_bytes()
    assert sha(raw) == SECONDARY_SHA256
    proof = json.loads(raw)
    for block in proof["blocks"]:
        rows = block["instructions"]
        assert sha(b"".join(struct.pack("<I", row["word"]) for row in rows)) == block["sha256"]
        for row in rows:
            assert row["pc"] not in words or words[row["pc"]] == row["word"]
            words[row["pc"]] = row["word"]
    hashes[SECONDARY] = sha(raw)
    return words, hashes


class FullTrace(Trace):
    """Observe contact boundaries without changing any fetched instruction."""
    def __init__(self, words, spring):
        super().__init__(words, spring)
        self.contacts = []

    def execute(self, pc):
        if pc in (0x296688, 0x296978):
            self.contacts.append({
                "kind": "point" if pc == 0x296688 else "generated",
                "address": self.g[17][0], "depth": self.f[20], "calls": [],
            })
        if pc in HELPERS:
            assert self.contacts
            self.contacts[-1]["calls"].append(len(self.calls))
        return super().execute(pc)


def project_records(machine, address, count):
    records = []
    for i in range(count):
        rec = address + i * 0x60
        scene = machine.read(rec + 0x40)
        records.append({
            "vectors": [machine.read(rec + off + j * 4)
                        for off in (0x10, 0x20, 0x30) for j in range(3)],
            "scene": [machine.read(scene + 0x100 + j * 4) for j in range(16)] if scene else [0] * 16,
            "hasScene": bool(scene), "other": machine.read(rec + 0x44),
            "material": machine.read(rec + 0x4c), "flags": machine.read(rec + 0x50, 1),
            "margin": machine.read(rec + 0x48),
        })
    return records


def synthetic_cases(words, count=256):
    assert count == 256
    rng = random.Random(SEED)
    cases, coverage, branches = [], set(), {}
    totals = {key: 0 for key in (
        "point_enabled_cases", "point_disabled_with_records_cases", "mixed_list_cases",
        "point_positive_contacts", "point_miss_contacts", "generated_positive_contacts",
        "generated_miss_contacts", "point_other_ignored_contacts", "point_friction_bit_set_contacts",
        "point_friction_bit_clear_contacts", "generated_friction_bit_clear_contacts",
        "alternate_force_contacts", "alternate_force_without_4c_contacts", "alternate_skipped_impact_contacts",
        "point_double_secondary_contacts", "generated_double_secondary_contacts", "cached_velocity_secondary_pairs",
        "secondary_inward_contacts", "secondary_outward_contacts", "secondary_zero_contacts",
        "point_margin_changes_contact_sign", "generated_margin_ignored_contacts",
        "velocity_changing_cases", "velocity_changes_visible_to_later_contacts", "all_helpers_contacts",
        "point_limit_below_contacts", "point_limit_equal_contacts", "point_limit_above_contacts",
        "generated_limit_below_contacts", "generated_limit_equal_contacts", "generated_limit_above_contacts",
    )}
    point_materials, generated_materials, active_modes = set(), set(), [set() for _ in range(4)]
    flags_seen = {"point": set(), "generated": set()}
    active_toggles = [set() for _ in range(6)]

    def vec(scale):
        return [f32(rng.uniform(-scale, scale)) for _ in range(3)]

    def matrix(simple):
        if simple:
            # Integer scene translations preserve exact one-ULP depth tests.
            return [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
                    rng.randrange(-8, 9), 0, rng.randrange(-8, 9), 1]
        angle = rng.uniform(-3, 3)
        c, s = f32(math.cos(angle)), f32(math.sin(angle))
        result = [c, 0, -s, 0, 0, 1, 0, 0, s, 0, c, 0] + vec(20) + [1]
        if rng.randrange(3) == 0:
            for j in (0, 1, 2, 4, 5, 6, 8, 9, 10):
                result[j] = f32(result[j] + rng.uniform(-.08, .08))
        return result

    for index in range(count):
        phase = index // 64
        spring, points, generated = 0x10000, 0x18000, 0x19000
        addresses, scenes = [0x11000, 0x12000, 0x13000], [0x14000, 0x15000, 0x16000]
        machine = FullTrace(words, spring)
        machine.g[28][0], machine.g[4][0] = GP, spring
        # Alternate initialized singleton states exercise their real initialization branches.
        for off in (-0x7358, -0x7354):
            machine.write(GP + off, index & 1, 1)
        machine.vector(0x4435a0, [0, 0, 0, 1])
        body_values = []
        for body_index, (address, scene) in enumerate(zip(addresses, scenes)):
            mass = f32(rng.uniform(100, 1900))
            if index % 16 in (0, 1, 2):
                mass = fp(0x447a0000 + index % 16 - 1)
            inertia = [f32(rng.uniform(.001, .015)) if row == col else 0
                       for col in range(3) for row in range(3)]
            velocity = vec(12)
            angular = vec(1)
            if phase != 2:
                # All three normal-speed signs, including exact zero, reach secondary.
                velocity = [3, (-2, 0, 2)[index % 3] if body_index == 0 else 0, -1]
                angular = [0, 0, 0]
            values = (matrix(phase != 2) + vec(2) + [mass, f32(1 / mass)] + inertia +
                      velocity + angular + vec(3) + vec(.3) + vec(100) + vec(10) +
                      ([0] * 6 if index % 3 else vec(.1) + vec(.01)) + vec(.1) + vec(.01))
            values = [f32(value) for value in values]
            initialize_body(machine, address, scene, values)
            body_values.append(values)
        machine.write(spring + 0x30, addresses[0])
        p = vec(10) + vec(10) + vec(2) + [f32(rng.uniform(.1, 1.5)) for _ in range(8)]
        p += [float((index >> shift) & 1) for shift in (4, 5, 6, 7)]
        for i, off in enumerate((0x110, 0x120, 0x100)):
            machine.vector(spring + off, p[i * 3:i * 3 + 3] + [1])
        for off, value in zip(PARAMETER_SCALARS, p[9:17]):
            machine.storef(spring + off, value)
        for off, value in zip(PARAMETER_FLAGS, p[17:]):
            machine.write(spring + off, int(value))
        toggles = [bool((index >> shift) & 1) for shift in range(6)]
        secondary_at_limit = bool(index & 64)
        for off, value in zip(TOGGLE_OFFSETS, toggles):
            machine.write(spring + off, int(value))
        machine.write(spring + 0x60, int(secondary_at_limit))
        response = [.5, 4, 8, 4, 16, 2]
        if phase == 2:
            response = [f32(rng.uniform(.1, 1.5)), f32(rng.uniform(.1, 8))] + [f32(rng.uniform(-2, 20)) for _ in range(4)]
        for off, value in zip(RESPONSE_OFFSETS, response):
            machine.storef(spring + off, value)
        for off in STATE_OFFSETS:
            machine.write(spring + off, rng.randrange(4))
        machine.storef(spring + 0xc4, f32(rng.uniform(-1, 2)))
        for i in range(10):
            machine.storef(spring + 0xd8 + i * 4, f32(i / 15))
        point_count = generated_count = 3
        if 128 <= index < 144:
            point_count, generated_count = index % 4, (index // 4) % 4
        for off, value in ((0x7c, point_count), (0x80, points), (0x84, generated_count), (0x8c, generated)):
            machine.write(spring + off, value)
        geometry = {}
        for kind, base, amount in (("point", points, point_count), ("generated", generated, generated_count)):
            for i in range(amount):
                address = base + i * 0x60
                # Full record padding is explicit synthetic input, never oracle fallback.
                for off in range(0, 0x60, 4):
                    machine.write(address + off, 0)
                local = [float(i - 1), 0, float(1 - i)] if phase != 2 else vec(5)
                normal = [0, 1, 0]
                if phase == 2:
                    normal = [.6, fp(0x3f4ccccd + i - 1), 0]
                has_scene = kind == "point" or (index + i) % 2 == 0
                record_scene = (0x1a000 if kind == "point" else 0x1b000) + i * 0x200
                chosen_matrix = matrix(phase != 2) if has_scene else body_values[0][:16]
                other = addresses[1 + (index % 2)] if (index + i) % 3 else 0
                depth = (fp(0x3effffff), .5, fp(0x3f000001))[i]
                margin = 0.
                if phase == 1:
                    depth = (-.25, .25, 0.)[i] if kind == "point" else (.5, -.25, 0.)[i]
                    margin = (.5, -.5, 0.)[i]
                elif phase == 2:
                    depth = (.25, -.5, .75)[i]
                    margin = (.125, -.125, .25)[i]
                elif kind == "generated":
                    # Generated penetration must ignore +48 even when nonzero.
                    margin = (-2., 0., 2.)[i]
                world = transformed(chosen_matrix, local)
                surface = [f32(x + f32(depth * n)) for x, n in zip(world, normal)]
                for off, value in ((0x10, local), (0x20, normal), (0x30, surface)):
                    machine.vector(address + off, value + [1])
                if has_scene:
                    machine.vector(record_scene + 0x100, chosen_matrix)
                machine.write(address + 0x40, record_scene if has_scene else 0)
                machine.write(address + 0x44, other)
                machine.storef(address + 0x48, margin)
                machine.write(address + 0x4c, (index // 2 + i * 3) % 10)
                flags = (0, 1, 2, 3, 0x80, 0x81, 0xfe, 0xff)[(index + i) % 8]
                machine.write(address + 0x50, flags, 1)
                geometry[address] = {"geometricDepth": depth, "margin": margin}
        case, defaults = project(machine, spring, addresses, generated, generated_count,
                                 f"response-{index:03d}", "explicitly-initialized-original-words-48l")
        assert not defaults, defaults
        case["pointRecords"] = project_records(machine, points, point_count)
        case["records"] = project_records(machine, generated, generated_count)
        case["parameters"]["response"] = [machine.read(spring + off) for off in RESPONSE_OFFSETS]
        case["parameters"]["secondaryAtLimit60"] = secondary_at_limit
        assert machine.run(0x2964c0) == 1
        finish_case(case, machine, spring, generated)
        case["expected"]["flags"] = [machine.read(base + i * 0x60 + 0x50, 1)
                                          for base, amount in ((points, point_count), (generated, generated_count))
                                          for i in range(amount)]
        assert not machine.source_reads, f"synthetic input escaped initialization: {index}"
        for body in case["bodies"]:
            assert all(math.isfinite(fp(value)) for value in body["expected"])
        assert all(math.isfinite(fp(value)) for field in ("center", "velocity") for value in case["expected"][field])
        totals["point_enabled_cases"] += bool(toggles[0] and point_count)
        totals["point_disabled_with_records_cases"] += bool(not toggles[0] and point_count)
        totals["mixed_list_cases"] += bool(toggles[0] and point_count and generated_count)
        if not toggles[0]:
            assert case["expected"]["flags"][:point_count] == [r["flags"] for r in case["pointRecords"]]
        kinds = [contact["kind"] for contact in machine.contacts]
        assert kinds == ["point"] * (point_count if toggles[0] else 0) + ["generated"] * generated_count
        for contact in machine.contacts:
            kind, address, depth = contact["kind"], contact["address"], contact["depth"]
            records, base = (case["pointRecords"], points) if kind == "point" else (case["records"], generated)
            rec = records[(address - base) // 0x60]
            calls = [case["calls"][i] for i in contact["calls"]]
            pcs = [call["pc"] for call in calls]
            normal = [fp(value) for value in rec["vectors"][3:6]]
            local = [fp(value) for value in rec["vectors"][:3]]
            surface = [fp(value) for value in rec["vectors"][6:9]]
            scene_values = [fp(value) for value in rec["scene"]] if rec["hasScene"] else body_values[0][:16]
            world = transformed(scene_values, local)
            products = [f32(f32(s - w) * n) for s, w, n in zip(surface, world, normal)]
            geometric_depth = f32(f32(products[0] + products[1]) + products[2])
            measured_depth = f32(fp(rec["margin"]) + geometric_depth) if kind == "point" else geometric_depth
            assert bits(depth) == bits(measured_depth), (case["name"], kind, depth, measured_depth)
            positive = depth > 0
            totals[kind + ("_positive_contacts" if positive else "_miss_contacts")] += 1
            output_flag = machine.read(address + 0x50, 1)
            assert output_flag == ((rec["flags"] | 2) if positive else (rec["flags"] & ~2))
            assert (output_flag & ~2) == (rec["flags"] & ~2)
            flags_seen[kind].add(rec["flags"])
            if kind == "point" and phase == 1:
                totals["point_margin_changes_contact_sign"] += ((geometry[address]["geometricDepth"] > 0) != positive)
            if kind == "generated" and geometry[address]["margin"]:
                totals["generated_margin_ignored_contacts"] += 1
            if not positive:
                assert not pcs
                continue
            expected = [PREP]
            alternate = kind == "generated" and rec["other"] and toggles[4]
            if alternate:
                expected.append(FORCE)
                totals["alternate_force_contacts"] += 1
                totals["alternate_force_without_4c_contacts"] += not toggles[1]
                totals["alternate_skipped_impact_contacts"] += toggles[3]
            else:
                if toggles[3]:
                    expected.append(IMPACT)
                if toggles[5]:
                    expected.append(SECONDARY_PC)
                if toggles[1]:
                    if secondary_at_limit and depth >= response[0] and not toggles[3]:
                        expected.append(SECONDARY_PC)
                    expected.append(FORCE)
            friction = toggles[2] and (kind == "generated" or rec["flags"] & 1)
            if friction:
                expected.append(FRICTION)
                (point_materials if kind == "point" else generated_materials).add(rec["material"])
            assert pcs == expected, (case["name"], kind, depth, pcs, expected)
            # Preparation caches contact velocity once. Neither impact nor the
            # first secondary causes any later helper to refresh that cache.
            if len(calls) > 2:
                assert all(call["contactVelocity120"] == calls[1]["contactVelocity120"] for call in calls[2:])
            for mode, value in enumerate(toggles):
                active_toggles[mode].add(value)
            if IMPACT in pcs:
                for mode in range(4):
                    active_modes[mode].add(case["parameters"]["impact"][17 + mode])
            if kind == "point":
                assert rec["hasScene"]
                for call in calls:
                    assert call["args"][1 if call["pc"] == PREP else 2] == 0
                totals["point_other_ignored_contacts"] += bool(rec["other"])
                if toggles[2]:
                    totals["point_friction_bit_set_contacts" if rec["flags"] & 1 else "point_friction_bit_clear_contacts"] += 1
            elif friction and not rec["flags"] & 1:
                totals["generated_friction_bit_clear_contacts"] += 1
            secondaries = [call for call in calls if call["pc"] == SECONDARY_PC]
            if len(secondaries) == 2:
                totals[kind + "_double_secondary_contacts"] += 1
                assert secondaries[0]["contactVelocity120"] == secondaries[1]["contactVelocity120"]
                totals["cached_velocity_secondary_pairs"] += 1
            for call in secondaries:
                velocity, normal = call["contactVelocity120"], call["vectors"]
                speed = f32(f32(f32(velocity[0] * normal[0]) + f32(velocity[1] * normal[1])) + f32(velocity[2] * normal[2]))
                totals["secondary_inward_contacts" if speed < 0 else "secondary_outward_contacts" if speed > 0 else "secondary_zero_contacts"] += 1
            if not alternate and toggles[1] and secondary_at_limit:
                suffix = "below" if depth < response[0] else "above" if depth > response[0] else "equal"
                totals[kind + "_limit_" + suffix + "_contacts"] += 1
            totals["all_helpers_contacts"] += all(pc in pcs for pc in (PREP, IMPACT, SECONDARY_PC, FORCE, FRICTION))
        if any(body["input"][30:36] != body["expected"][:6] for body in case["bodies"]):
            totals["velocity_changing_cases"] += 1
        preparations = [call for call in case["calls"] if call["pc"] == PREP]
        if preparations and any(call["bodyStateBefore"]["primary"][:6] != preparations[0]["bodyStateBefore"]["primary"][:6]
                                for call in preparations[1:]):
            totals["velocity_changes_visible_to_later_contacts"] += 1
        coverage.update(machine.coverage)
        for pc, taken in machine.branches.items():
            branches.setdefault(pc, set()).update(taken)
        cases.append(case)
    assert all(value > 0 for value in totals.values()), totals
    assert point_materials == generated_materials == set(range(10)), (point_materials, generated_materials)
    assert all(values == {False, True} for values in active_toggles), active_toggles
    assert all(values == {bits(0.), bits(1.)} for values in active_modes), active_modes
    assert all(len(flags) == 8 for flags in flags_seen.values()), flags_seen
    assert {len(case["pointRecords"]) for case in cases} == {0, 1, 2, 3}
    assert {len(case["records"]) for case in cases} == {0, 1, 2, 3}
    required_branches = (
        0x2964f8, 0x296504, 0x296510, 0x2965a0, 0x2965c0, 0x296690,
        0x296730, 0x296754, 0x296774, 0x296780, 0x296794, 0x2967a0,
        0x2967dc, 0x2967ec, 0x296850, 0x29685c, 0x296888,
        0x296988, 0x2969a0, 0x2969cc, 0x296a64, 0x296a70, 0x296a90,
        0x296ac8, 0x296aec, 0x296b0c, 0x296b18, 0x296b2c, 0x296b38,
        0x296b74, 0x296bd8,
    )
    for pc in required_branches:
        assert branches.get(pc) == {False, True}, (hex(pc), branches.get(pc))
    assert set(range(0x2983c0, 0x298474, 4)).issubset(coverage)
    return cases, {"seed": f"{SEED:X}", "cases": count, "source_reads": 0,
                   "coverage_assertions": totals,
                   "friction_materials": {"point": sorted(point_materials), "generated": sorted(generated_materials)},
                   "record_flags": {kind: sorted(flags) for kind, flags in flags_seen.items()},
                   "required_both_way_branches": [f"{pc:08X}" for pc in required_branches],
                   "coverage": [f"{pc:08X}" for pc in sorted(coverage)],
                   "branches": {f"{pc:08X}": sorted(taken) for pc, taken in sorted(branches.items())}}


def main():
    words, hashes = original_words()
    base_raw = (ROOT / BASE).read_bytes()
    assert sha(base_raw) == BASE_SHA256
    base = json.loads(base_raw)
    assert len(base["cases"]) == 196
    synthetic, synthetic_report = synthetic_cases(words)
    report = {
        "schema": "spring-full-response48l-v1", "dependencies": hashes,
        "arithmetic": base["arithmetic"], "original_instructions_patched": False,
        "prior_fixture": {"path": BASE, "sha256": BASE_SHA256, "cases": 196, "case_objects_unchanged": True,
                          "saved_projection_policy": base["saved_replay"]["projection_policy"]},
        "saved_replay": base["saved_replay"],
        "synthetic": synthetic_report,
        "schema_extension": {
            "pointRecords": "Optional, same record shape as records plus margin uint32 binary32; processed before records only when toggles[0] is true. Explicit scenes are required for point records.",
            "records": "Generated contacts, unchanged 48K record schema; optional margin binary32 is ignored by generated original instructions.",
            "parameters.response": ["maxPenetration90", "maxSpeed94", "stiffness9C", "dampingA0", "impactStiffnessA4", "impactDampingA8"],
            "parameters.secondaryAtLimit60": "Boolean; the six preexisting toggles retain their offset order 48,4C,50,54,58,68.",
            "expected.flags": "All point-record flags followed by all generated-record flags, including unchanged disabled point records.",
            "legacy_optional_fields": "Absent new fields in preserved 48K cases are test projection scaffolding only. Their original modes do not read them; no missing byte is inserted into original interpreter memory.",
        },
        "cases": base["cases"] + synthetic,
    }
    assert report["cases"][:196] == base["cases"]
    OUT.mkdir(parents=True, exist_ok=True)
    raw = (json.dumps(report, indent=2, allow_nan=False) + "\n").encode()
    destination = OUT / "coordinator-fixtures.json"
    destination.write_bytes(raw)
    print(json.dumps({"path": str(destination.relative_to(ROOT)), "sha256": sha(raw),
                      "retained_cases": 196, "new_cases": len(synthetic),
                      "synthetic_assertions": synthetic_report["coverage_assertions"]}))


if __name__ == "__main__":
    main()
