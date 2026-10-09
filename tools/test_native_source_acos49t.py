#!/usr/bin/env python3
"""49T source acos proof. Default execution only reads the checkout."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / "evidence/chariotAnimation49T/acos"
SOURCE = ROOT / "src/game/source_acos49t.cpp"
HEADER = ROOT / "src/game/source_acos49t.h"
TEST = ROOT / "tests/native/source_acos49t.cpp"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def invoke(command, env=None):
    result = subprocess.run([str(x) for x in command], cwd=ROOT, env=env,
                            text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode:
        raise RuntimeError(f"command failed ({result.returncode}): {command}\n{result.stdout}{result.stderr}")
    return result


def authenticate():
    manifest = json.loads((EVIDENCE / "source-authentication.json").read_text())
    raw = (ROOT / "work/native/project/disc/SLUS_202.68").read_bytes()
    assert hashlib.sha256(raw).hexdigest() == manifest["elf_sha256"]
    phoff = struct.unpack_from("<I", raw, 28)[0]
    phsize, count = struct.unpack_from("<HH", raw, 42)
    segments = [struct.unpack_from("<8I", raw, phoff + i * phsize) for i in range(count)]

    def data(address, count):
        for kind, offset, base, _, size, *_ in segments:
            if kind == 1 and base <= address and address + count <= base + size:
                return raw[offset + address - base:offset + address - base + count]
        raise AssertionError(f"non-file-backed source {address:x}")

    words = (EVIDENCE / "original_words.h").read_text()
    total = 0
    for block in manifest["blocks"]:
        expected = data(block["start"], block["end_exclusive"] - block["start"])
        assert hashlib.sha256(expected).hexdigest() == block["sha256"]
        body = re.search(r"\b" + block["name"] + r"\[\]=\{(.*?)\};", words, re.S).group(1)
        values = [int(x, 16) for x in re.findall(r"0x([0-9a-f]{8})u", body)]
        assert struct.pack("<%dI" % len(values), *values) == expected
        assert len(values) == block["words"]
        total += len(values)
    declared = re.search(r"total_words=(\d+)", words)
    assert int(declared.group(1)) == total
    constants = {}
    for address, bits in re.findall(r"\{0x([0-9a-f]{8})u,0x([0-9a-f]{8})u\}",
                                    (EVIDENCE / "original_data.h").read_text()):
        constants[int(address, 16)] = int(bits, 16)
    for block in manifest["source_data"]:
        expected = data(block["start"], block["end_exclusive"] - block["start"])
        assert expected.hex() == block["bytes"]
        assert hashlib.sha256(expected).hexdigest() == block["sha256"]
        actual = b"".join(struct.pack("<I", constants[a]) for a in range(block["start"], block["end_exclusive"], 4))
        assert actual == expected
    assert digest(EVIDENCE / "actual-arguments.tsv") == manifest["actual_arguments"]["tsv_sha256"]
    exact = json.loads((EVIDENCE / "exact-vectors.json").read_text())
    assert digest(EVIDENCE / "exact_vectors.h") == exact["sha256"]
    return {"elf_sha256": manifest["elf_sha256"], "source_words": total,
            "data_words": len(constants), "actual_arguments": 1250}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--skip-sanitizers", action="store_true")
    parser.add_argument("--skip-controls", action="store_true")
    parser.add_argument("--write-evidence", action="store_true",
                        help="explicitly write verification.json and original-coverage.tsv")
    args = parser.parse_args()
    report = {"authentication": authenticate(), "normal": None, "sanitizers": None, "controls": []}
    flags = [args.compiler, "-std=c++17", "-O2", "-Wall", "-Wextra", "-Werror",
             "-DRR_SOURCE_ACOS49T_TEST_API", "-I", ROOT / "src/game"]
    report["compiler"] = invoke([args.compiler, "--version"]).stdout.splitlines()[0]
    with tempfile.TemporaryDirectory(prefix="rr_source_acos49t_") as temporary:
        temp = Path(temporary)

        def build(source, name, sanitize=False):
            binary = temp / name
            extra = ["-O1", "-g", "-fno-omit-frame-pointer", "-fsanitize=address,undefined", "-no-pie"] if sanitize else []
            invoke(flags + extra + [TEST, source, "-o", binary])
            return binary

        binary = build(SOURCE, "normal")
        command = [binary]
        if args.write_evidence:
            command += ["--coverage", EVIDENCE / "original-coverage.tsv"]
        report["normal"] = json.loads(invoke(command).stdout)
        print(json.dumps(report["normal"], sort_keys=True), flush=True)
        if not args.skip_sanitizers:
            binary = build(SOURCE, "sanitizers", True)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=0:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            result = json.loads(invoke([binary], env).stdout)
            assert result == report["normal"], "sanitizer results diverged"
            report["sanitizers"] = {"address": True, "undefined": True, "leak": False, "result": result}
            print("ASan/UBSan passed; LSan disabled", flush=True)
        if not args.skip_controls:
            source = SOURCE.read_text()
            mutations = [
                ("domain_propagates_inner_nan", "const D wrapped = domain ? 0 : inner;", "const D wrapped = inner;"),
                ("domain_errno_wrong", "if (domain) state.errnoValue = 33;", "if (domain) state.errnoValue = 34;"),
                ("success_resets_errno", "if (domain) state.errnoValue = 33;", "state.errnoValue = domain ? 33 : 0;"),
                ("admits_wrong_mode", "state.mode != 1 || ", ""),
                ("ignores_handler_contract", "!state.defaultMatherr || ", ""),
                ("float_nan_host_marker", "f|=0x100000;", "f|=0x400000;"),
                ("double_pack_round_down", "else f+=127;", "else f+=0;"),
                ("float_pack_round_down", "else f+=63;", "else f+=0;"),
                ("polynomial_coefficient_truncated", "0x3fc5555555555555ull", "0x3fc5555555550000ull"),
                ("positive_df_keeps_low_word", "df=s&0xffffffff00000000ull", "df=s"),
                ("sqrt_omits_final_rounding", "else q1+=q1&1;", "else q1+=0;"),
                ("negative_branch_uses_subtraction", "mul(add(x,one),half)", "mul(sub(x,one),half)"),
                ("narrow_loses_sticky_bit", "if(n.fraction&0x3fffffffull)f|=1;", ""),
            ]
            for name, before, after in mutations:
                assert source.count(before) == 1, f"mutation anchor changed: {name}"
                path = temp / f"{name}.cpp"
                path.write_text(source.replace(before, after))
                binary = build(path, name)
                result = subprocess.run([str(binary)], cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                if result.returncode != 1 or not result.stderr.startswith("FAIL "):
                    raise RuntimeError(f"undetected control {name}: {result.returncode}\n{result.stdout}{result.stderr}")
                report["controls"].append({"name": name, "status": "detected", "assertion": result.stderr.strip()})
            print(f"{len(report['controls'])} semantic controls detected", flush=True)
        # A production build does not compile the test interface or depend on it.
        invoke([args.compiler, "-std=c++17", "-O2", "-Wall", "-Wextra", "-Werror", "-c", SOURCE, "-o", temp / "production.o"])
    files = [HEADER, SOURCE, TEST, Path(__file__), *sorted(EVIDENCE.glob("*.h")),
             EVIDENCE / "actual-arguments.tsv", EVIDENCE / "source-authentication.json",
             EVIDENCE / "exact-vectors.json", EVIDENCE / "generate_exact_vectors.py"]
    report["inputs"] = {str(p.relative_to(ROOT)): digest(p) for p in files}
    if args.write_evidence:
        (EVIDENCE / "verification.json").write_text(json.dumps(report, indent=2) + "\n")
    print("Source acos49T verification passed; " + ("evidence written" if args.write_evidence else "checkout read-only"), flush=True)


if __name__ == "__main__":
    main()
