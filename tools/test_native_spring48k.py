#!/usr/bin/env python3
"""Build and run the portable 48J–48L spring checks without CMake or downloads.

Uses a GCC/Clang-compatible C++ driver (CXX or g++), one process at a time.
Outputs are retained outside the source tree in a fresh temporary directory, or
in an empty --output-dir. Exit codes: 0 passed, 1 failed, 2 blocked. A selected
subset is explicitly reported as partial coverage. Timings are host wall time,
not game FPS or original-Xbox performance measurements.
"""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import os
import platform
import re
import shlex
import shutil
import struct
import subprocess
import sys
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]
SUITE_NAME = "spring"
EXTRA_INPUT_PATHS = ()
SOURCES = (
    "src/game/spring_contact48j.cpp",
    "src/game/spring_impulse48k.cpp",
    "src/game/spring_model48k.cpp",
    "src/game/spring_force48l.cpp",
)
TESTS = {
    "spring_contact48j": {
        "source": "tests/native/spring_contact48j.cpp",
        "fixture": "evidence/spring48J/friction-fixtures.bin",
        "shape": (1024, 80),
    },
    "spring_impulse48k": {
        "source": "tests/native/spring_impulse48k.cpp",
        "fixture": "evidence/spring48K/impact-fixtures.bin",
        "shape": (1024, 197),
    },
    "spring_model48k": {
        "source": "tests/native/spring_model48k.cpp",
        "fixture": "evidence/springOrchestration48K/coordinator-native-fixtures.json",
    },
}
TESTS.update({
    "spring_force48l": {
        "source": "tests/native/spring_force48l.cpp",
        "fixture": "evidence/springResponse48L/spring-force-fixtures.bin",
        "shape": (2048, 91),
        "extra_fixtures": [{"fixture": "evidence/springResponse48L/secondary-fixtures.bin", "shape": (512, 12)}],
    },
    "spring_response48l": {
        "source": "tests/native/spring_response48l.cpp",
        "fixture": "evidence/springResponse48L/coordinator-fixtures.json",
    },
})
INCLUDES = ("src/game", "third_party/native-json")
STRICT_FLAGS = ["-std=c++17", "-Wall", "-Wextra", "-Wpedantic",
                "-fno-fast-math", "-frounding-math", "-ffp-contract=off"]


def fingerprint(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return {"sha256": digest.hexdigest(), "bytes": path.stat().st_size}


def stamp():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def run_command(command, root, out, name, timeout, env):
    stdout_path, stderr_path = out / (name + ".stdout.log"), out / (name + ".stderr.log")
    result = {"command": command, "cwd": str(root), "started_utc": stamp()}
    start = time.monotonic()
    with stdout_path.open("wb") as stdout, stderr_path.open("wb") as stderr:
        try:
            process = subprocess.run(command, cwd=root, stdout=stdout, stderr=stderr,
                                     timeout=timeout, env=env, check=False)
            result.update(returncode=process.returncode,
                          status="passed" if process.returncode == 0 else "failed")
        except subprocess.TimeoutExpired:
            result.update(returncode=None, status="failed", error=f"Timed out after {timeout} seconds")
        except OSError as error:
            result.update(returncode=None, status="blocked", error=str(error))
    result["elapsed_seconds"] = round(time.monotonic() - start, 6)
    for key, path in (("stdout", stdout_path), ("stderr", stderr_path)):
        result[key] = {"path": str(path), **fingerprint(path)}
    return result


def fixture_error(path, spec):
    if path is None:
        return None
    if not path.is_file():
        return f"BLOCKED: required fixture is missing: {path}; restore verified evidence. No fixture test was silently skipped."
    try:
        if "shape" in spec:
            with path.open("rb") as stream:
                header = stream.read(8)
            shape = struct.unpack("<II", header) if len(header) == 8 else None
            expected = spec["shape"]
            if shape != expected or path.stat().st_size != 8 + expected[0] * expected[1] * 4:
                return f"BLOCKED: malformed fixture {path}; expected {expected[0]} rows of {expected[1]} binary32 words."
        elif spec.get("binary"):
            with path.open("rb") as stream:
                if not stream.read(1):
                    return f"BLOCKED: empty binary fixture: {path}"
        else:
            json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError) as error:
        return f"BLOCKED: unreadable fixture {path}: {error}"
    return None


def source_inputs(root, relative_paths):
    """Track quoted local includes recursively, without globbing production units."""
    pending, found = [root / p for p in relative_paths], set()
    while pending:
        path = pending.pop().resolve()
        if path in found:
            continue
        found.add(path)
        if not path.is_file():
            continue
        for header in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"',
                                 path.read_text(encoding="utf-8"), re.MULTILINE):
            candidates = [path.parent / header] + [root / base / header for base in INCLUDES]
            resolved = next((candidate for candidate in candidates if candidate.is_file()), candidates[0])
            pending.append(resolved)
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", type=Path, default=ROOT)
    parser.add_argument("--output-dir", type=Path, help="Empty output directory outside the source tree")
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"), help="GCC/Clang-compatible driver command; default CXX or g++")
    parser.add_argument("--sanitizers", action="store_true", help="Require AddressSanitizer and UndefinedBehaviorSanitizer; no fallback")
    parser.add_argument("--no-leak-check", action="store_true", help="Explicitly omit LeakSanitizer in sanitizer mode (e.g. ptrace-restricted hosts)")
    parser.add_argument("--stack-usage", action="store_true", help="Require compiler .su frame-size reports (per-function estimates, not total live stack)")
    parser.add_argument("--tests", nargs="+", choices=tuple(TESTS), default=list(TESTS), help="Optional focused subset; default all registered tests")
    parser.add_argument("--build-timeout", type=float, default=180, help="Seconds per compile or link command")
    parser.add_argument("--test-timeout", type=float, default=120, help="Seconds per test")
    args = parser.parse_args()
    root = args.repo_root.resolve()
    if not root.is_dir() or args.build_timeout <= 0 or args.test_timeout <= 0:
        parser.error("Repository must exist, and timeouts must be positive")
    if args.no_leak_check and not args.sanitizers:
        parser.error("--no-leak-check requires --sanitizers")
    selected = list(dict.fromkeys(args.tests))
    out = args.output_dir.resolve() if args.output_dir else Path(tempfile.mkdtemp(prefix="rr-native-"+SUITE_NAME+"-"))
    if out == root or root in out.parents:
        parser.error("--output-dir must be outside the source tree")
    try:
        out.mkdir(parents=True, exist_ok=True)
        if any(out.iterdir()):
            parser.error("--output-dir must be empty; use a fresh directory to preserve previous evidence")
    except OSError as error:
        parser.error(f"Cannot prepare output directory: {error}")

    compile_flags = STRICT_FLAGS + (["-O1", "-g", "-fsanitize=address,undefined",
                                    "-fno-omit-frame-pointer", "-fno-sanitize-recover=all"]
                                   if args.sanitizers else ["-O2"])
    link_flags = ["-fsanitize=address,undefined"] if args.sanitizers else []
    if args.stack_usage:
        compile_flags.append("-fstack-usage")
    include_flags = ["-I" + str(root / path) for path in INCLUDES]
    env = os.environ.copy()
    env["LC_ALL"] = "C"
    if args.sanitizers:
        env["ASAN_OPTIONS"] = "halt_on_error=1:detect_leaks=" + ("0" if args.no_leak_check else "1")
        env["UBSAN_OPTIONS"] = "halt_on_error=1:print_stacktrace=1"
    report = {
        "schema_version": 1, "started_utc": stamp(), "repository": str(root), "output_directory": str(out),
        "host": {"platform": platform.platform(), "machine": platform.machine(), "python": sys.version},
        "scope": "all_"+SUITE_NAME+"_checks" if set(selected) == set(TESTS) else "selected_"+SUITE_NAME+"_checks_only",
        "selected_tests": selected, "serial": True, "sanitizers": args.sanitizers,
        "leak_detection": args.sanitizers and not args.no_leak_check,
        "stack_usage_requested": args.stack_usage,
        "compile_flags": compile_flags, "include_flags": include_flags, "link_flags": link_flags,
        "environment": {key: env[key] for key in ("LC_ALL", "ASAN_OPTIONS", "UBSAN_OPTIONS") if key in env},
        "timing_note": "Host build/test wall time only; not gameplay FPS or original-Xbox performance.",
        "build": [], "tests": [], "errors": [], "input_hashes": {},
    }
    report_path = out / "report.json"

    def save():
        report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    # Include all code dependencies and all selected fixtures in the evidence.
    paths = source_inputs(root, list(SOURCES) + [TESTS[name]["source"] for name in selected])
    paths.update(root / TESTS[name]["fixture"] for name in selected if TESTS[name]["fixture"])
    paths.update(root / extra["fixture"] for name in selected for extra in TESTS[name].get("extra_fixtures", []))
    paths.update(root / path for path in ("tools/test_native_spring48k.py", "CMakeLists.txt", "cmake/NativeSpring48K.cmake"))
    paths.update(root / path for path in EXTRA_INPUT_PATHS)
    paths.add(root / "third_party/native-json/json.hpp")
    for path in sorted(paths):
        key = str(path.relative_to(root)) if path.is_relative_to(root) else str(path)
        try:
            report["input_hashes"][key] = fingerprint(path)
        except OSError as error:
            report["input_hashes"][key] = {"missing_or_unreadable": str(error)}

    try:
        compiler = shlex.split(args.cxx)
    except ValueError as error:
        compiler = []
        report["errors"].append(f"BLOCKED: invalid CXX command: {error}")
    executable = shutil.which(compiler[0]) if compiler else None
    if not executable:
        report["errors"].append(f"BLOCKED: C++ compiler not found: {args.cxx}")
    else:
        compiler[0] = executable
        report["compiler"] = {"command": compiler, "executable": fingerprint(Path(executable).resolve())}
        version = run_command(compiler + ["--version"], root, out, "compiler-version", args.build_timeout, env)
        report["compiler"]["version"] = version
        if version["status"] != "passed":
            report["errors"].append("BLOCKED: compiler version check failed; see compiler-version logs")

    for name in selected:
        spec = TESTS[name]
        error = fixture_error(root / spec["fixture"] if spec["fixture"] else None, spec)
        for extra in spec.get("extra_fixtures", []):
            error = error or fixture_error(root / extra["fixture"], extra)
        result = {"name": name, "source": spec["source"], "fixture": spec["fixture"], "status": "pending"}
        if not (root / spec["source"]).is_file():
            error = f"BLOCKED: test source is missing: {spec['source']}"
        if error:
            result.update(status="blocked", error=error)
            print(f"{name}: {error}", flush=True)
        report["tests"].append(result)
    save()

    objects = []
    library_ok = not report["errors"]
    if library_ok:
        for source in SOURCES:
            obj = out / (Path(source).stem + ".o")
            command = compiler + compile_flags + include_flags + ["-c", str(root / source), "-o", str(obj)]
            result = run_command(command, root, out, "build-" + obj.stem, args.build_timeout, env)
            result["source"] = source
            if result["status"] == "passed":
                result["object"] = {"path": str(obj), **fingerprint(obj)}
                objects.append(str(obj))
            else:
                library_ok = False
            report["build"].append(result)
            print(f"build {source}: {result['status']}", flush=True)
            save()

    for result in report["tests"]:
        if result["status"] == "blocked":
            continue
        if not library_ok:
            result.update(status="blocked", error="BLOCKED: spring library compilation or compiler setup did not complete")
            continue
        name = result["name"]
        binary = out / (name + (".exe" if os.name == "nt" else ""))
        command = compiler + compile_flags + include_flags + [str(root / result["source"])] + objects + link_flags + ["-o", str(binary)]
        result["build"] = run_command(command, root, out, "build-test-" + name, args.build_timeout, env)
        result["status"] = result["build"]["status"]
        if result["status"] == "passed":
            result["executable"] = {"path": str(binary), **fingerprint(binary)}
            arguments = [str(root / argument) for argument in TESTS[name]["arguments"]] if "arguments" in TESTS[name] else [str(root)]
            result["run"] = run_command([str(binary)] + arguments, root, out, "test-" + name, args.test_timeout, env)
            result["status"] = result["run"]["status"]
            if result["run"]["returncode"] == 2:
                result.update(status="blocked", error="BLOCKED: test rejected its fixture or invocation; inspect test logs")
            for key in ("stdout", "stderr"):
                # Logs remain complete on disk; limit terminal output on a failing test.
                with Path(result["run"][key]["path"]).open("rb") as stream:
                    excerpt = stream.read(16384).decode("utf-8", errors="replace")
                if excerpt:
                    print(excerpt.rstrip(), flush=True)
                if (key == "stderr" and result["run"]["returncode"] != 0
                        and "LeakSanitizer does not work under ptrace" in excerpt):
                    result.update(status="blocked", error="BLOCKED: LeakSanitizer is unavailable under ptrace. An explicit --no-leak-check run can check ASan/UBSan separately; it will not validate leaks.")
        print(f"{name}: {result['status']}", flush=True)
        save()

    report["changed_inputs"] = []
    if args.stack_usage:
        report["stack_usage"] = [{"path": str(path), **fingerprint(path), "text": path.read_text(encoding="utf-8")}
                                 for path in sorted(out.glob("*.su"))]
        report["stack_usage_note"] = "Compiler per-function frame estimates; caller buffers and nested call frames are additional. This is not a measured peak stack bound."
        if not report["stack_usage"]:
            report["errors"].append("BLOCKED: --stack-usage requested but compiler emitted no .su files")
    for path, before in report["input_hashes"].items():
        if "sha256" not in before:
            continue
        try:
            after = fingerprint(root / path)
        except OSError:
            after = None
        if before != after:
            report["changed_inputs"].append(path)
    states = [result["status"] for result in report["build"] + report["tests"]]
    report["status"] = ("failed" if "failed" in states else
                        "blocked" if report["errors"] or report["changed_inputs"] or "blocked" in states else "passed")
    if report["changed_inputs"]:
        report["errors"].append("BLOCKED: input files changed during the run; rerun against stable inputs")
    report["finished_utc"] = stamp()
    save()
    for error in report["errors"]:
        print(error, file=sys.stderr)
    print(json.dumps({"status": report["status"], "scope": report["scope"], "report": str(report_path)}))
    return {"passed": 0, "failed": 1, "blocked": 2}[report["status"]]


if __name__ == "__main__":
    sys.exit(main())
