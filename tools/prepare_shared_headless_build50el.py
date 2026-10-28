#!/usr/bin/env python3
"""Write the shared track/textured headless client's compiler argv, without execution.
Usage: prepare_shared_headless_build50el.py OUTPUT_COMMAND_JSON OUTPUT_EXECUTABLE
"""
from pathlib import Path
import json
import sys
sys.dont_write_bytecode = True
from prepare_offscreen_build50do import build_command, ROOT

def main():
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    destination, executable = (Path(p).resolve() for p in sys.argv[1:])
    if destination == executable:
        raise SystemExit('command JSON and executable paths must differ')
    command = build_command(executable)
    old_main = str(ROOT / 'tests/native/terrain_offscreen_drive50dh.cpp')
    new_main = str(ROOT / 'tests/native/terrain_headless_drive50ek.cpp')
    remove = {str(ROOT / ('platform/linux/native/' + name)) for name in (
        'vehicle_offscreen_gl50dd.cpp', 'vehicle_offscreen_present50de.cpp')}
    command = [new_main if item == old_main else item for item in command if item not in remove]
    command[command.index('-lz'):command.index('-lz')] = [str(ROOT / name) for name in (
        # Renderer headers share these native out-of-line helper owners.
        'tests/native/material_alpha50gx_fixture.cpp',
        'tests/native/original_racer_visual50hw_fixture.cpp',
        'tests/native/native_racer_model_lifetime50la_fixture.cpp',
        'tests/native/original_ui_main_entry50kg.cpp',
        'platform/linux/native/vehicle_window_gl50ds.cpp',
        'platform/linux/native/vehicle_window_gl50ds_storage.cpp',
        'platform/linux/native/vehicle_window_gl50ds_draw.cpp',
        'platform/linux/native/vehicle_window_gl50ds_ui.cpp',
        'platform/linux/native/vehicle_window_gl50ds_world.cpp',
        'platform/linux/native/vehicle_window_gl50ds_hud.cpp',
        'platform/linux/native/vehicle_window_gl50ds_validation.cpp',
        'platform/linux/native/native_camera_view50dx.cpp',
        'platform/linux/native/material_texture_gl50eb.cpp',
        'src/assets/material_texture_assets50ea.cpp')]
    destination.write_text(json.dumps(command, indent=2) + '\n', encoding='utf-8')

if __name__ == '__main__':
    main()
