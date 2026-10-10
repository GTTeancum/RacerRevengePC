#!/usr/bin/env python3
"""49R response semantic controls, original-word helper witness, and sanitizers."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/game/visual_response49r.cpp"
TEST = ROOT / "tests/native/visual_response49r.cpp"
DEPENDENCIES = [ROOT / "src/game" / name for name in (
    "body_scene48g.cpp", "scene_graph49p.cpp", "gradient_noise49m.cpp")]
EVIDENCE = ROOT / "evidence/vehicleVisual49R/response"


def invoke(command, *, env=None, accepted=(0,)):
    completed = subprocess.run([str(x) for x in command], cwd=ROOT, env=env,
                               text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if completed.returncode not in accepted:
        raise RuntimeError(f"command failed ({completed.returncode}): {' '.join(map(str, command))}\n"
                           f"{completed.stdout}{completed.stderr}")
    return completed


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--skip-sanitizers", action="store_true")
    parser.add_argument("--skip-controls", action="store_true")
    args = parser.parse_args()
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    common = [args.compiler, "-std=c++20", "-O2", "-frounding-math", "-ffp-contract=off",
              "-fno-fast-math", "-I", ROOT / "src/game"]
    report = {"scope": "focused suffix semantic checks; complete21C120 word proof is separate",
              "compiler": invoke([args.compiler, "--version"]).stdout.splitlines()[0],
              "normal": None, "sanitizers": None, "semantic_controls": []}
    with tempfile.TemporaryDirectory(prefix="rr_visual_response49r_") as directory:
        temporary = Path(directory)

        def build(source, name, sanitize=False):
            binary = temporary / name
            flags = ["-g", "-O1", "-fno-omit-frame-pointer", "-fsanitize=address,undefined", "-no-pie"] if sanitize else []
            invoke(common + flags + [TEST, source, *DEPENDENCIES, "-o", binary])
            return binary

        binary = build(SOURCE, "normal")
        report["normal"] = json.loads(invoke([binary]).stdout)
        print(json.dumps(report["normal"], sort_keys=True), flush=True)
        if not args.skip_sanitizers:
            binary = build(SOURCE, "asan_ubsan", sanitize=True)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1", UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            report["sanitizers"] = {"address": True, "undefined": True, "leak": False,
                                    "result": json.loads(invoke([binary], env=env).stdout)}
            print("ASan/UBSan passed; leak sanitizer disabled", flush=True)

        witness = EVIDENCE / "tools/double_normalization.cpp"
        witness_binary = temporary / "double_normalization"
        invoke(common + [witness, "-o", witness_binary])
        report["double_normalization_witness"] = json.loads(invoke([witness_binary]).stdout)
        (EVIDENCE / "double-normalization-witness.json").write_text(
            json.dumps(report["double_normalization_witness"], indent=2) + "\n")

        if not args.skip_controls:
            source = SOURCE.read_text()
            old_euler = """    for (unsigned i = 0; i < 3; ++i) s.linearVelocity.v[i] = f.add(s.linearVelocity.v[i], f.mul(dt, force.v[i]));
    for (unsigned i = 0; i < 3; ++i) s.position.v[i] = f.add(s.position.v[i], f.mul(dt, s.linearVelocity.v[i]));"""
            new_euler = "\n".join(reversed(old_euler.splitlines()))
            mutations = [
                ("explicit_euler_uses_old_velocity", old_euler, new_euler),
                ("differential_tie_selects_engine", "f.abs(engineDifference) <= f.abs(aeroDifference)", "f.abs(engineDifference) < f.abs(aeroDifference)"),
                ("application_bit0_instead_of_bit1", "(in.applicationByte58 & 2u)", "(in.applicationByte58 & 1u)"),
                ("omit_absent_left_clock", "s.phase950 = f.add(s.phase950, f.mul(leftTime, in.frameFactor));", "s.phase950 = s.phase950;"),
                ("skip_zero_amplitude_noise", "if (in.vehicle844 == 0) {", "if (in.vehicle844 == 0 && common != 0.0f) {"),
                ("omit_second_normalization", "normalize(visual, f); // Also required with844!=0.", "// Mutant omits second normalization."),
                ("feed_visual_quaternion_into_history", "normalize(visual, f); // Also required with844!=0.", "normalize(visual, f); s.orientation = visual;"),
                ("omit_container_world_composition", "if (!setLocalMatrix49P(graph, node, composed))", "if (!setLocalMatrix49P(graph, node, local))"),
                ("preserve_clamped_y_velocity", "s.linearVelocity.v[1] = 0.0f;", "s.linearVelocity.v[1] = s.linearVelocity.v[1];"),
                ("omit_empty_tail_guard", "lazy(shared.globals[8], 0x41200000u);", "// Mutant skips empty-list tail guard."),
            ]
            for name, old, new in mutations:
                if old not in source:
                    raise RuntimeError(f"missing mutation anchor: {name}")
                mutation = temporary / f"{name}.cpp"
                mutation.write_text(source.replace(old, new))
                binary = build(mutation, name)
                result = subprocess.run([str(binary)], cwd=ROOT, text=True,
                                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                if result.returncode != 1 or not result.stderr.startswith("FAIL "):
                    raise RuntimeError(f"semantic control did not fail an assertion: {name}\n{result.stdout}{result.stderr}")
                report["semantic_controls"].append({"name": name, "status": "detected", "assertion": result.stderr.strip()})
            print(f"{len(report['semantic_controls'])} behavioral mutation controls detected", flush=True)
    inputs = [SOURCE, TEST, *DEPENDENCIES, Path(__file__), EVIDENCE / "tools/double_normalization.cpp",
              ROOT / "src/game/vehicle_visual49r.h", ROOT / "src/game/vehicle_visual49r_internal.h",
              ROOT / "tools/gameplay/body_scene_oracle48g.h", ROOT / "tools/gameplay/body_scene_words48g.h",
              ROOT / "tools/gameplay/physics_words48c.h",
              ROOT / "work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h"]
    report["inputs"] = {str(path.relative_to(ROOT)): hashlib.sha256(path.read_bytes()).hexdigest() for path in inputs}
    report["status"] = "pass"
    (EVIDENCE / "validation.json").write_text(json.dumps(report, indent=2) + "\n")
    print("evidence/vehicleVisual49R/response/validation.json", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
