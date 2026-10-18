#!/usr/bin/env python3
"""Proposed direct-disc launcher; not a validated release. No asset conversion."""
import argparse
import os
from pathlib import Path
import subprocess


def missing_windows_libraries(binary):
    """File presence only; also independently testable on non-Windows hosts."""
    return [name for name in ('SDL2.dll', 'avcodec-61.dll', 'avutil-59.dll')
            if not (binary.parent / name).is_file()]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--assets', required=True, type=Path,
                        help='Your separately supplied extracted original disc directory')
    parser.add_argument('--executable', type=Path,
                        default=Path(__file__).resolve().parent / ('RacerRevengePC.exe' if os.name == 'nt' else 'racer-revenge'))
    parser.add_argument('--check', action='store_true', help='Check paths only; do not launch or claim runtime compatibility')
    args = parser.parse_args()
    try:
        root = args.assets.resolve(strict=True)
    except (OSError, RuntimeError) as error:
        parser.error('Original asset directory unavailable: ' + str(error))
    try:
        binary = args.executable.resolve(strict=True)
    except (OSError, RuntimeError) as error:
        parser.error('Executable unavailable: ' + str(error))
    if not root.is_dir() or not binary.is_file():
        parser.error('Original asset root must be a directory and executable must be a regular file')
    if os.name != 'nt' and not os.access(binary, os.X_OK):
        parser.error('Executable does not have execute permission')
    # Deliberately shallow presence checks. Native source readers authenticate
    # individual resources; this is not a full-disc hash or coverage claim.
    for filename in ('SLUS_202.68', 'APPSTART.RES', 'PODS/POD01/POD01.RES', 'TRACKS/TA.RES'):
        if not (root / filename).is_file():
            parser.error('Missing original source file: ' + filename)
    if os.name == 'nt':
        missing = missing_windows_libraries(binary)
        if missing:
            parser.error('Missing required libraries beside executable: ' + ', '.join(missing))
    if args.check:
        print('Paths present. Binary format, library ABI, complete disc and gameplay remain unverified.')
        return 0
    # No prepared runtime, corpus argument, rider cache, automated route, all-AI
    # override or developer evidence output enters this clean release launch.
    env = {k: v for k, v in os.environ.items() if not k.startswith('RR_')}
    # RR_START_MENU controls the separate host menu, not original Main.
    # Original Main remains enabled by the --assets entry.
    env.update(RR_RACE_MODE='2', RR_ORIGINAL_MAIN_AUDIO='muted', RR_AUDIO='0',
               RR_AUDIO_DEVICE='0', RR_HUMAN_INPUT_DEVICE='keyboard', RR_START_MENU='0')
    try:
        return subprocess.call([str(binary), '--assets', str(root)], cwd=binary.parent, env=env)
    except OSError as error:
        parser.error('Unable to launch executable: ' + str(error))


if __name__ == '__main__':
    raise SystemExit(main())
