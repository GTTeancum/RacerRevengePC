#!/usr/bin/env python3
"""Build one native B9D component check and run it once, only when authorized.

Source-only candidate: this script has not been executed or syntax-checked.
Requires POSIX and the project's GCC/Clang-compatible strict-FP workflow.
Each child owns a new process group; timeout cleanup kills the whole group.
No compiler/version probe, CMake, imported project runner, retries, sanitizer second pass, object
reuse, LTO, section garbage collection, or alternative test is invoked.
The caller owns the centralized hourly test slot; this script does not reserve it.
"""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import math
import os
import re
import shutil
import signal
import subprocess
import sys
import time


HERE = Path(__file__).resolve().parent
INCLUDE_DIRS = ("tests/native", "src/assets", "src/game")
FLAGS = ["-std=c++20", "-O2", "-Wall", "-Wextra", "-Wpedantic",
         "-fno-fast-math", "-frounding-math", "-ffp-contract=off"]


def stamp():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def fingerprint(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return {"sha256": digest.hexdigest(), "bytes": path.stat().st_size}


def manifest(path):
    rows = [row.strip() for row in path.read_text(encoding="utf-8").splitlines()
            if row.strip() and not row.lstrip().startswith("#")]
    if len(rows) != len(set(rows)):
        raise ValueError("duplicate manifest entry: " + str(path))
    for row in rows:
        if Path(row).is_absolute() or ".." in Path(row).parts:
            raise ValueError("manifest must contain bounded relative paths: " + row)
    return rows


def quoted_inputs(sources, include_dirs):
    """Record existing quoted includes as data; never preprocess or execute them."""
    pending = list(sources)
    found = set()
    while pending:
        path = pending.pop().resolve()
        if path in found:
            continue
        if not path.is_file():
            raise ValueError("missing source/header input: " + str(path))
        found.add(path)
        for name in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"',
                               path.read_text(encoding="utf-8"), re.MULTILINE):
            candidates = [path.parent / name] + [base / name for base in include_dirs]
            resolved = next((candidate for candidate in candidates if candidate.is_file()), None)
            if resolved is None:
                raise ValueError("missing quoted include " + name + " from " + str(path))
            pending.append(resolved)
    return found


def invoke(command, cwd, output, label, timeout, environment):
    result = {"command": command, "cwd": str(cwd), "started_at_utc": stamp()}
    before = time.monotonic()
    stdout = output / (label + ".stdout.log")
    stderr = output / (label + ".stderr.log")
    process = None
    logs_stable = True

    def stop_group():
        """One kill attempt and one bounded reap; never retry the command."""
        nonlocal logs_stable
        cleanup = {"signal": "SIGKILL", "group_stopped": False, "reaped": False}
        try:
            os.killpg(process.pid, signal.SIGKILL)
            cleanup["group_stopped"] = True
        except ProcessLookupError:
            cleanup["group_stopped"] = True
        except OSError as error:
            cleanup["kill_error"] = str(error)
        try:
            cleanup["exit_code"] = process.wait(timeout=10)
            cleanup["reaped"] = True
        except subprocess.TimeoutExpired:
            cleanup["reap_error"] = "process did not exit within 10 seconds after group cleanup"
        except OSError as error:
            cleanup["reap_error"] = str(error)
        logs_stable = cleanup["group_stopped"] and cleanup["reaped"]
        result["cleanup"] = cleanup
        if not logs_stable:
            result["status"] = "blocked"
            result["cleanup_required"] = True

    try:
        out = stdout.open("wb")
        try:
            err = stderr.open("wb")
        except OSError:
            out.close()
            raise
    except OSError as error:
        result.update(exit_code=None, status="blocked", error=str(error))
        result.update(finished_at_utc=stamp(), elapsed_seconds=time.monotonic() - before)
        return result

    with out, err:
        try:
            process = subprocess.Popen(command, cwd=cwd, env=environment,
                                       stdout=out, stderr=err, start_new_session=True)
            result["pid"] = process.pid
            result["exit_code"] = process.wait(timeout=timeout)
            result["status"] = "passed" if result["exit_code"] == 0 else "failed"
        except subprocess.TimeoutExpired:
            result.update(exit_code=None, status="failed", error="timeout")
            stop_group()
        except OSError as error:
            result.update(exit_code=None, status="blocked", error=str(error))
            if process is not None:
                stop_group()
        except KeyboardInterrupt:
            result.update(exit_code=None, status="failed", error="interrupted")
            if process is not None:
                stop_group()
    result.update(finished_at_utc=stamp(), elapsed_seconds=time.monotonic() - before)
    for key, path in (("stdout", stdout), ("stderr", stderr)):
        result[key] = {"path": str(path)}
        if logs_stable:
            try:
                result[key].update(fingerprint(path))
            except OSError as error:
                result[key]["hash_error"] = str(error)
                result["status"] = "blocked"
        else:
            result[key]["hash_omitted"] = "process-group cleanup/reap was not confirmed"
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", required=True, type=Path)
    parser.add_argument("--assets-root", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path,
                        help="Fresh nonexistent directory outside the repository")
    parser.add_argument("--test-source", type=Path,
                        help="Default: REPO_ROOT/tests/native/vehicle_b9d_native_check.cpp")
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"),
                        help="One GCC/Clang-compatible compiler executable, no shell arguments")
    parser.add_argument("--source-label", default="",
                        help="Caller-verified commit/identity; file hashes remain authoritative")
    parser.add_argument("--build-timeout", type=float, default=900)
    parser.add_argument("--test-timeout", type=float, default=120)
    args = parser.parse_args()
    if os.name != "posix":
        parser.error("this GCC/Clang convenience runner requires POSIX process groups")
    root = args.repo_root.resolve()
    assets = args.assets_root.resolve()
    output = args.output_dir.resolve()
    test = (args.test_source if args.test_source is not None else
            root / "tests/native/vehicle_b9d_native_check.cpp").resolve()
    if not root.is_dir() or not assets.is_dir():
        parser.error("repository and asset roots must exist")
    if (not math.isfinite(args.build_timeout) or not math.isfinite(args.test_timeout)
            or args.build_timeout <= 0 or args.test_timeout <= 0):
        parser.error("timeouts must be finite and positive")
    if output == root or root in output.parents or output.exists():
        parser.error("output directory must be fresh and outside the repository")
    output.mkdir(parents=True, exist_ok=False)
    report = {"scope": "native_b9d_reader_and_tail_only", "started_at_utc": stamp(),
              "source_label": args.source_label, "repository": str(root),
              "assets_root": str(assets), "test_source": str(test),
              "build": None, "test": None, "phase": "preparing", "status": "preparing",
              "input_hashes": {}, "changed_inputs": []}
    report_path = output / "report.json"

    def save():
        report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    try:
        source_names = manifest(HERE / "native_sources.txt")
        if not source_names:
            raise ValueError("empty production source manifest")
        for name in source_names:
            if not name.endswith(".cpp") or not name.startswith(("src/game/", "src/assets/")):
                raise ValueError("unexpected production source scope: " + name)
        sources = [root / name for name in source_names]
        for source in sources:
            if re.search(r'\b(?:int|auto)\s+main\s*\(', source.read_text(encoding="utf-8")):
                raise ValueError("alternate main in production manifest: " + str(source))
        includes = [root / name for name in INCLUDE_DIRS]
        paths = quoted_inputs([test] + sources, includes)
        paths.update(assets / name for name in manifest(HERE / "native_assets.txt"))
        paths.update({Path(__file__).resolve(), HERE / "native_sources.txt", HERE / "native_assets.txt"})
        for path in sorted(paths):
            report["input_hashes"][str(path)] = fingerprint(path)
        compiler = shutil.which(args.cxx)
        if compiler is None:
            raise ValueError("compiler executable not found: " + args.cxx)
        report["compiler"] = {"path": compiler,
                              **fingerprint(Path(compiler).resolve())}
        binary = output / ("vehicle_b9d_native_check.exe" if os.name == "nt" else "vehicle_b9d_native_check")
        command = [compiler] + FLAGS + ["-I" + str(path) for path in includes]
        # Link every listed production translation unit directly. No archive
        # selection or dead-strip flags hide unresolved functions/owners.
        command += [str(test)] + [str(path) for path in sources] + ["-o", str(binary)]
        report["production_sources"] = source_names
        report["link_libraries"] = []
        report["flags"] = FLAGS
        (output / "build-command.json").write_text(json.dumps(command, indent=2) + "\n")
        environment = os.environ.copy()
        environment["LC_ALL"] = "C"
        report.update(phase="build", status="running")
        save()
        report["build"] = invoke(command, root, output, "build", args.build_timeout, environment)
        report.update(phase="build_finished", status="awaiting_test" if
                      report["build"]["status"] == "passed" else report["build"]["status"])
        save()
        if report["build"]["status"] == "passed":
            report["executable"] = {"path": str(binary), **fingerprint(binary)}
            report.update(phase="test", status="running")
            save()
            report["test"] = invoke([str(binary), str(assets)], root, output,
                                    "test", args.test_timeout, environment)
            report["status"] = report["test"]["status"]
        else:
            report["status"] = report["build"]["status"]
        for name, before in report["input_hashes"].items():
            try:
                after = fingerprint(Path(name))
            except OSError:
                after = None
            if before != after:
                report["changed_inputs"].append(name)
        if report["changed_inputs"]:
            report["status"] = "blocked"
            report["error"] = "inputs changed during validation; no passing identity established"
    except (OSError, ValueError) as error:
        report.update(status="blocked", error=str(error))
    report["phase"] = "finished"
    report["finished_at_utc"] = stamp()
    save()
    print(json.dumps({"status": report["status"], "report": str(report_path)}))
    return {"passed": 0, "failed": 1, "blocked": 2}[report["status"]]


if __name__ == "__main__":
    sys.exit(main())
