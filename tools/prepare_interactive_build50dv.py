#!/usr/bin/env python3
"""Write the Linux interactive client compiler argv without building or running it."""
import argparse
import json
import os
from pathlib import Path
import re
import shlex

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "platform/windows/native/interactive50du.sources.json"
SHARED_SOURCE = re.compile(r"(?:src/(?:game|assets)|platform/linux/native|tests/native)/[A-Za-z0-9_]+\.cpp")


def build_command(output_executable):
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    shared = manifest["sources"]
    linux = manifest["linuxAdditionalSources"]
    if manifest["schema"] != 1 or "tests/native/terrain_interactive_drive50du.cpp" not in shared:
        raise ValueError("Invalid interactive source manifest or missing game entry point")
    if any(not SHARED_SOURCE.fullmatch(unit) or "offscreen" in unit or "joystick_transport" in unit or
           "pcsx2" in unit for unit in shared):
        raise ValueError("Unexpected shared interactive source path")
    if linux != ["platform/linux/native/offscreen_gl_context50dc.cpp"]:
        raise ValueError("Unexpected Linux platform sources")
    units = shared + linux
    if len({unit.casefold() for unit in units}) != len(units):
        raise ValueError("Duplicate interactive source path")
    for unit in units:
        if not (ROOT / unit).is_file():
            raise ValueError(f"Interactive source missing: {unit}")

    compiler = shlex.split(os.environ.get("CXX", "g++"))
    if not compiler:
        raise ValueError("CXX must contain a compiler executable")
    includes = ("third_party/ffmpeg-7.1.5/include", "src/assets", "src/game", "third_party/native-json")
    return [
        *compiler, "-std=c++20", "-O2", "-fno-fast-math", "-frounding-math", "-ffp-contract=off", "-pthread",
        *("-I" + str(ROOT / directory) for directory in includes),
        *(str(ROOT / unit) for unit in units),
        "-lz", "-ldl", "-o", str(Path(output_executable).resolve()),
    ]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output_command_json", type=Path)
    parser.add_argument("output_executable", type=Path)
    args = parser.parse_args()
    if args.output_command_json.resolve() == args.output_executable.resolve():
        parser.error("command JSON and executable must use different paths")
    try:
        command = build_command(args.output_executable)
    except ValueError as error:
        parser.error(str(error))
    args.output_command_json.write_text(json.dumps(command, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
