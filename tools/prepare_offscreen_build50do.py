#!/usr/bin/env python3
"""Write a portable source-only build command; never build or run it.

Usage: prepare_offscreen_build50do.py OUTPUT_COMMAND_JSON OUTPUT_EXECUTABLE
"""

import argparse
import json
import os
from pathlib import Path
import shlex
import sys

# Reuse the same guarded, declarative source list as prepare_submit_build50cw.
# Importing it must not leave bytecode files in the source checkout.
sys.dont_write_bytecode = True
from check_terrain_registered_drive50bv import ROOT, SOURCES as BASE_SOURCES


GAME_SOURCES = (
    "candidate_collection50bx",
    "candidate_merge48s",
    "type2_bounds50by",
    "collection_bounds48s",
    "vehicle_candidates50ca",
    "candidate_handles50cb",
    "indexed_type2_contact50cc",
    "vehicle_indexed_main50cd",
    "vehicle_indexed_secondary50cd",
    "collection_projection50cf",
    "zone_search_views50cg",
    "zone_registration50cj",
    "global_zone_chain50ck",
    "zone_lifecycle50cl",
    "vehicle_draw50cm",
    "vehicle_draw_batch50co",
    "zone_registration_events50cq",
    "vehicle_draw_submit50cs",
    "controller_ingress50cx",
    "vehicle_present50da",
)

ASSET_SOURCES = (
    "src/assets/zone_search_authoring50cn.cpp",
    "src/assets/vehicle_draw_assets50cr.cpp",
)

# 50DD owns 50CV, 50CZ and 50DC; the harness also calls 50DE and 50DG.
# Match the optional offscreen source closure in platform/linux/native/CMakeLists.txt.
LINUX_SOURCES = (
    "platform/linux/native/vehicle_draw_gl50cv.cpp",
    "platform/linux/native/vehicle_shader_gl50cz.cpp",
    "platform/linux/native/offscreen_gl_context50dc.cpp",
    "platform/linux/native/vehicle_offscreen_gl50dd.cpp",
    "platform/linux/native/vehicle_offscreen_present50de.cpp",
    "platform/linux/native/vehicle_view_camera50dg.cpp",
)


def build_command(output_executable):
    """Return compiler argv without invoking a compiler, shell, or device API."""
    sources = [
        "tests/native/terrain_offscreen_drive50dh.cpp"
        if source == "tests/native/terrain_registered_drive50bv.cpp"
        else source
        for source in BASE_SOURCES
    ]
    sources.extend("src/game/" + name + ".cpp" for name in GAME_SOURCES)
    sources.extend(ASSET_SOURCES)
    sources.extend(LINUX_SOURCES)
    sources = list(dict.fromkeys(sources))

    # CXX is an argv prefix, not a shell command. No shell expansion is applied.
    compiler = shlex.split(os.environ.get("CXX", "g++"))
    if not compiler:
        raise ValueError("CXX must contain a compiler executable")
    return compiler + [
        "-std=c++20",
        "-O2",
        "-fno-fast-math",
        "-frounding-math",
        "-ffp-contract=off",
        "-pthread",
        "-I" + str(ROOT / "src/assets"),
        "-I" + str(ROOT / "src/game"),
        "-I" + str(ROOT / "third_party/native-json"),
    ] + [str(ROOT / source) for source in sources] + [
        "-lz",
        "-ldl",
        "-o",
        str(Path(output_executable).resolve()),
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
    args.output_command_json.write_text(
        json.dumps(command, indent=2) + "\n", encoding="utf-8"
    )


if __name__ == "__main__":
    main()
