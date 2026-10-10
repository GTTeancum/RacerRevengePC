#!/usr/bin/env python3
"""Write, but do not execute, a source-only interactive development build argv.
Usage: prepare_interactive_build50dv.py OUTPUT_COMMAND_JSON OUTPUT_EXECUTABLE
"""
import json
from pathlib import Path
import sys
sys.dont_write_bytecode = True
from prepare_offscreen_build50do import build_command, ROOT

def main():
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    destination = Path(sys.argv[1]).resolve()
    executable = Path(sys.argv[2]).resolve()
    if destination == executable:
        raise SystemExit('JSON output must differ from executable output')
    argv = build_command(executable)
    remove = {
        str(ROOT / 'platform/linux/native/offscreen_gl_context50dc.cpp'),
        str(ROOT / 'platform/linux/native/vehicle_offscreen_gl50dd.cpp'),
        str(ROOT / 'platform/linux/native/vehicle_offscreen_present50de.cpp'),
    }
    old_main = str(ROOT / 'tests/native/terrain_offscreen_drive50dh.cpp')
    new_main = str(ROOT / 'tests/native/terrain_interactive_drive50du.cpp')
    argv = [new_main if item == old_main else item for item in argv if item not in remove]
    insertion = argv.index('-lz')
    argv[insertion:insertion] = [str(ROOT / ('platform/linux/native/' + source)) for source in (
        'window_host50dq.cpp', 'keyboard_packet50dr.cpp', 'vehicle_window_gl50ds.cpp',
        'material_texture_gl50eb.cpp', 'game_host50er.cpp', 'offscreen_gl_context50dc.cpp')]
    argv.insert(argv.index('-lz'), str(ROOT / 'src/assets/material_texture_assets50ea.cpp'))
    for source in ('src/assets/audio_assets50hc.cpp', 'src/assets/material_audio50hi.cpp', 'src/game/material_audio50hi.cpp', 'src/game/gameplay_audio50he.cpp', 'src/game/pod_speech50hg.cpp', 'platform/linux/native/audio_mixer50hd.cpp', 'platform/linux/native/audio_output50hd.cpp', 'src/game/engine_flare50gv.cpp', 'src/game/wrong_way_progress50go.cpp', 'src/game/host_course_driver50gk.cpp', 'src/game/lap_progress50gf.cpp', 'src/game/lap_completion50gg.cpp', 'src/game/draw_visibility50ei.cpp', 'src/game/collision_tree_workspace48m.cpp', 'src/game/terrain_prepare48m.cpp', 'src/game/terrain_query_workspace48m.cpp',
                   'src/game/vehicle_contact48g.cpp', 'src/game/body_contact_list48r.cpp', 'src/game/controller_actions50ec.cpp', 'src/assets/repair_authoring50a.cpp',
                   'src/game/live_zone_membership50en.cpp', 'src/game/portal_geometry50eo.cpp',
                   'src/game/vehicle_live_query50eq.cpp',
                   'src/game/track_navigation50eu.cpp', 'src/game/type0_bounds50ew.cpp', 'src/game/undertrack_reset50ew.cpp', 'src/game/reset_visibility50ey.cpp', 'src/game/pod_scene_rebase50fb.cpp', 'src/game/pod_cables50fd.cpp', 'src/game/boost_controls50fk.cpp', 'src/game/undertrack_reposition50et.cpp',
                   'src/game/source_acos49t.cpp', 'src/game/chariot_scene49u.cpp'):
        absolute = str(ROOT / source)
        if absolute not in argv:
            argv.insert(argv.index('-lz'), absolute)
    # Inactive original UI modules; source registration does not enable frontend.
    for source in (
        'src/assets/original_ui_archive50kg.cpp',
        'src/assets/original_ui_atm50kg.cpp',
        'src/assets/original_ui_font50kg.cpp',
        'src/assets/original_ui_icon50kg.cpp',
        'src/assets/original_ui_localization50kg.cpp',
        'src/assets/original_ui_material50kg.cpp',
        'src/assets/original_ui_overlay_text50kg.cpp',
        'src/assets/original_ui_pages50kg.cpp',
        'src/assets/original_ui_screen_assets50kg.cpp',
        'src/game/original_ui_icon_layout50kg.cpp',
        'src/game/original_ui_main_layout50kg.cpp',
        'src/game/original_ui_navigation50kg.cpp',
        'src/game/original_ui_text50kg.cpp',
        'src/game/original_ui_text_geometry50kg.cpp',
        'src/game/original_ui_text_position50kg.cpp',
        'src/game/original_ui_text_widget50kg.cpp',
        'src/game/original_ui_static_text_page_owner50kg.cpp',
    ):
        absolute = str(ROOT / source)
        if absolute not in argv:
            argv.insert(argv.index('-lz'), absolute)
    destination.write_text(json.dumps(argv, indent=2) + '\n', encoding='utf-8')

if __name__ == '__main__':
    main()
