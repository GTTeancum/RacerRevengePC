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
        'original_ui_garage_canonical_submission50kg.cpp',
        'original_ui_garage_raw_surface50kg.cpp',
        'original_ui_garage_model_lowering50kg.cpp',
        'original_ui_garage_model_context50kg.cpp',
        'original_ui_garage_queued_model_geometry50kg.cpp',
        'original_ui_garage_collector50kg.cpp',
        'material_texture_gl50eb.cpp', 'game_host50er.cpp', 'offscreen_gl_context50dc.cpp')]
    argv.insert(argv.index('-lz'), str(ROOT / 'src/assets/material_texture_assets50ea.cpp'))
    for source in ('src/assets/audio_assets50hc.cpp', 'src/assets/material_audio50hi.cpp', 'src/game/material_audio50hi.cpp', 'src/game/gameplay_audio50he.cpp', 'src/game/pod_speech50hg.cpp', 'platform/linux/native/audio_mixer50hd.cpp', 'platform/linux/native/audio_output50hd.cpp', 'src/game/engine_flare50gv.cpp', 'src/game/wrong_way_progress50go.cpp', 'src/game/host_course_driver50gk.cpp', 'src/game/lap_progress50gf.cpp', 'src/game/lap_completion50gg.cpp', 'src/game/draw_visibility50ei.cpp', 'src/game/collision_tree_workspace48m.cpp', 'src/game/terrain_prepare48m.cpp', 'src/game/terrain_query_workspace48m.cpp',
                   'src/game/vehicle_contact48g.cpp', 'src/game/body_contact_list48r.cpp', 'src/game/controller_actions50ec.cpp', 'src/assets/repair_authoring50a.cpp',
                   'src/game/live_zone_membership50en.cpp', 'src/game/portal_geometry50eo.cpp',
                   'src/game/vehicle_live_query50eq.cpp',
                   'src/game/track_navigation50eu.cpp', 'src/game/type0_bounds50ew.cpp', 'src/game/undertrack_reset50ew.cpp', 'src/game/reset_visibility50ey.cpp', 'src/game/reset_ground_query50iz.cpp', 'src/game/original_gc_particle_manager50kg.cpp', 'src/game/original_gc_generator_fire50kg.cpp', 'src/game/original_gc_spray50kg.cpp', 'src/game/original_gc_fire_emission50kg.cpp', 'src/game/original_gc_pod_dust50kg.cpp', 'src/game/original_gc_pod_fire50kg.cpp', 'src/game/original_gc_pod_sparks50kg.cpp', 'src/assets/original_gc_particle_material50kg.cpp', 'src/game/original_gc_particle_render50kg.cpp', 'src/game/pod_scene_rebase50fb.cpp', 'src/game/pod_cables50fd.cpp', 'src/game/boost_controls50fk.cpp', 'src/game/undertrack_reposition50et.cpp',
                   'src/game/source_acos49t.cpp', 'src/game/chariot_scene49u.cpp'):
        absolute = str(ROOT / source)
        if absolute not in argv:
            argv.insert(argv.index('-lz'), absolute)
    # Original camera kernels and concrete native display/intro integration.
    for source in (
        'src/game/original_attached_camera.cpp',
        'src/game/original_camera_query_cold.cpp',
        'src/game/original_ai_engine_emitter.cpp',
        'src/game/original_camera_query_owner.cpp',
        'src/game/original_camera_racer_history.cpp',
        'src/game/original_camera_scene_owner.cpp',
        'src/game/original_camera_scene_tick.cpp',
        'src/game/original_camera_sphere_query.cpp',
        'src/game/original_camera_world_query.cpp',
        'src/game/original_death_camera_query.cpp',
        'src/game/original_death_camera50kg.cpp',
        'src/game/original_race_hud50kg.cpp',
        'src/game/original_wreck_explosion50la.cpp',
        'src/game/original_wreck_splash50la.cpp',
        'src/game/original_wreck_effects50la.cpp',
        'src/game/original_follow_cam.cpp',
        'src/game/original_follow_camera.cpp',
        'src/game/original_follow_collision.cpp',
        'src/game/original_race_start_camera.cpp',
        'src/game/original_camera_start_effects.cpp',
        'src/game/original_camera_start_authoring.cpp',
        'platform/linux/native/original_video_blocking_native.cpp',
        'platform/linux/native/original_gameplay_blur_display.cpp',
        'platform/linux/native/native_camera_view50dx.cpp',
    ):
        absolute = str(ROOT / source)
        if absolute not in argv:
            argv.insert(argv.index('-lz'), absolute)
    # Immutable audio metadata; source registration does not bind live providers.
    audio_directory = str(ROOT / 'src/assets/original_audio_directory50kx.cpp')
    if audio_directory not in argv:
        argv.insert(argv.index('-lz'), audio_directory)
    stream_metadata = str(ROOT / 'src/assets/original_stream_metadata50kz.cpp')
    if stream_metadata not in argv:
        argv.insert(argv.index('-lz'), stream_metadata)
    # Detached cold ownership only; no event attachment or playback admission.
    cold_audio = str(ROOT / 'src/game/cold_logical_audio_domain50ky.cpp')
    if cold_audio not in argv:
        argv.insert(argv.index('-lz'), cold_audio)
    # Native selected lighting defaults on in matching fresh gameplay. Build
    # registration does not itself execute the client or enable a menu policy.
    gc_lighting = str(ROOT / 'src/game/original_gc_gameplay_lighting_candidate.cpp')
    if gc_lighting not in argv:
        argv.insert(argv.index('-lz'), gc_lighting)
    gameplay_binder = str(ROOT / 'src/game/original_gameplay_binder_candidate.cpp')
    if gameplay_binder not in argv:
        argv.insert(argv.index('-lz'), gameplay_binder)
    # Canonical material/resource closure for the existing native TA consumer.
    for source in (
        'src/assets/original_standard_model_resource50la.cpp',
        'src/assets/original_standard_program_table50la.cpp',
        'src/assets/original_standard_material_source50la.cpp',
        'src/assets/original_standard_material_object50la.cpp',
        'src/assets/original_standard_material_table50la.cpp',
        'src/assets/original_standard_screen_material_reference50la.cpp',
        'src/assets/original_standard_texture_table50la.cpp',
        'src/assets/original_standard_texture_resource50la.cpp',
    ):
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
        'src/assets/original_ui_material_equivalence50kg.cpp',
        'src/assets/original_ui_material_binding50kg.cpp',
        'src/assets/original_ui_main_material_schedule50kg.cpp',
        'src/assets/original_ui_overlay_text50kg.cpp',
        'src/assets/original_ui_pages50kg.cpp',
        'src/assets/original_ui_screen_assets50kg.cpp',
        'src/assets/original_ui_image_bank50kg.cpp',
        'src/assets/original_ui_rider_model_materials50kg.cpp',
        'src/assets/original_ui_ipum_texture_cache50kg.cpp',
        'src/game/original_ui_icon_layout50kg.cpp',
        'src/game/original_ui_main_layout50kg.cpp',
        'src/game/original_ui_navigation50kg.cpp',
        'src/game/original_ui_text50kg.cpp',
        'src/game/original_ui_text_geometry50kg.cpp',
        'src/game/original_ui_text_position50kg.cpp',
        'src/game/original_ui_text_widget50kg.cpp',
        'src/game/original_ui_static_text_page_owner50kg.cpp',
        'src/game/original_ui_transition_owner50kg.cpp',
        'src/game/original_ui_route_decisions50kg.cpp',
        'src/game/original_ui_aperture_owner50kg.cpp',
        'src/assets/original_ui_movement_control50kg.cpp',
        'src/assets/original_ui_main_camera_config50kg.cpp',
        'src/game/original_ui_movement_widget50kg.cpp',
        'src/game/original_ui_main_camera50kg.cpp',
        'src/game/original_ui_main_particle_camera50kg.cpp',
        'src/game/original_ui_lit_geometry50km.cpp',
        'src/game/original_ui_rider_camera50kg.cpp',
        'src/game/original_ui_rider_render_context50kg.cpp',
        'src/game/original_ui_rider_model_fog50kg.cpp',
        'src/game/original_ui_rider_clip_camera50kg.cpp',
        'src/game/original_ui_options_particle_render_state50kg.cpp',
        'src/game/original_ui_exit_text50kg.cpp',
        'src/game/original_ui_main_scene50kg.cpp',
        'src/game/original_ui_main_fire50kg.cpp',
        'src/game/original_ui_main_particle_lighting50kg.cpp',
        'src/assets/original_ui_main_particle_material50kg.cpp',
        'src/game/original_ui_main_particle_geometry50kg.cpp',
        'src/game/original_ui_main_particle_render_state50kg.cpp',
        'platform/linux/native/original_ui_main_audio50kg.cpp',
        'platform/linux/native/original_ui_main_draw50kg.cpp',
        'platform/linux/native/original_ui_main_session50kg.cpp',
        'platform/linux/native/original_ui_frontend50kg.cpp',
        # Concrete isolated Garage owners; route admission remains separate.
        'platform/linux/native/original_application_localization50kg.cpp',
        'platform/linux/native/original_audio_pending_waves50kg.cpp',
        'platform/linux/native/original_audio_sample_cache50kg.cpp',
        'platform/linux/native/original_localization_renderer_lease50kg.cpp',
        'platform/linux/native/original_ui_garage_actors50gn.cpp',
        'platform/linux/native/original_ui_garage_audio_backing50kg.cpp',
        'platform/linux/native/original_ui_garage_audio_dispatch50kg.cpp',
        'platform/linux/native/original_ui_garage_droid_audio50gn.cpp',
        'platform/linux/native/original_ui_garage_fog_color_lease50kg.cpp',
        'platform/linux/native/original_ui_garage_font50kg.cpp',
        'platform/linux/native/original_ui_garage_manager_fog50kg.cpp',
        'platform/linux/native/original_ui_garage_manager_search50kg.cpp',
        'platform/linux/native/original_ui_garage_movement_owner50kg.cpp',
        'platform/linux/native/original_ui_garage_page_owner50kg.cpp',
        'platform/linux/native/original_ui_garage_failed_late_camera50kg.cpp',
        'platform/linux/native/original_ui_garage_failed_late_children50kg.cpp',
        'platform/linux/native/original_ui_garage_failed_camera50kg.cpp',
        'platform/linux/native/original_ui_garage_page_input50kg.cpp',
        'platform/linux/native/original_ui_garage_page_timers50kg.cpp',
        'platform/linux/native/original_ui_garage_page_panel_lifecycle50kg.cpp',
        'platform/linux/native/original_ui_garage_shared_page_render50kg.cpp',
        'platform/linux/native/original_ui_garage_page_aperture_lifecycle50kg.cpp',
        'platform/linux/native/original_ui_garage_page_runtime_controls50kg.cpp',
        'platform/linux/native/original_ui_garage_aperture_draw50kg.cpp',
        'platform/linux/native/original_ui_garage_collector_screen50kg.cpp',
        'platform/linux/native/original_ui_garage_collector_drain50kg.cpp',
        'platform/linux/native/original_ui_garage_collector_aperture50kg.cpp',
        'platform/linux/native/original_ui_garage_particle_render50kg.cpp',
        'platform/linux/native/original_ui_garage_particle_project50kg.cpp',
        'platform/linux/native/original_ui_garage_particle_light50kg.cpp',
        'platform/linux/native/original_ui_garage_particle_packet50kg.cpp',
        'platform/linux/native/original_ui_garage_particle_packet_geometry50kg.cpp',
        'platform/linux/native/original_ui_garage_collector_particle50kg.cpp',
        'platform/linux/native/original_ui_garage_particle_binding50kg.cpp',
        'platform/linux/native/original_ui_garage_model_bank_binding50kg.cpp',
        'platform/linux/native/original_ui_garage_page_draw50kg.cpp',
        'platform/linux/native/original_ui_garage_panel50kg.cpp',
        'platform/linux/native/original_ui_garage_pod_scene50kg.cpp',
        'platform/linux/native/original_ui_garage_quad50kg.cpp',
        'platform/linux/native/original_ui_garage_renderer_lease50kg.cpp',
        'platform/linux/native/original_ui_garage_draw_contribution50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_collision50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_config50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_dispatch50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_emitters50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_final_load50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_lighting50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_load50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_particles50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_payload_retirement50kg.cpp',
        'platform/linux/native/original_ui_garage_scene_setup50kg.cpp',
        'platform/linux/native/original_ui_garage_screen_text50kg.cpp',
        'platform/linux/native/original_ui_garage_session50kg.cpp',
        'platform/linux/native/original_ui_garage_manager_frame50kg.cpp',
        'platform/linux/native/original_ui_garage_frontend_bridge50kg.cpp',
        'platform/linux/native/original_ui_garage_render_return50kg.cpp',
        'platform/linux/native/original_ui_shared_dialogs50kg.cpp',
        'platform/linux/native/original_ui_shared_dialog_material_history50kg.cpp',
        'platform/linux/native/original_ui_shared_menu_warning50kg.cpp',
        'platform/linux/native/original_ui_shared_menu_raster50kg.cpp',
        'platform/linux/native/original_ui_shared_menu_callbacks50kg.cpp',
        'platform/linux/native/original_ui_shared_warning_dim50kg.cpp',
        'platform/linux/native/original_ui_warning_draw50kg.cpp',
        'platform/linux/native/original_ui_warning_geometry50kg.cpp',
        'platform/linux/native/original_application_aural50kg.cpp',
        'platform/linux/native/native_source_reverb50kg.cpp',
        'platform/linux/native/original_ui_garage_shared_page50kg.cpp',
        'platform/linux/native/original_ui_garage_static_models50kg.cpp',
        'platform/linux/native/original_ui_garage_text_overlay50kg.cpp',
        'platform/linux/native/original_ui_garage_toy_dispatch50kg.cpp',
        'platform/linux/native/original_ui_garage_ui_lights50kg.cpp',
        'platform/linux/native/original_ui_garage_visibility50kg.cpp',
        'platform/linux/native/original_ui_garage_watto_audio50kg.cpp',
        'platform/linux/native/original_ui_garage_watto_owner50kg.cpp',
        'platform/linux/native/original_ui_garage_watto_sound_group50kg.cpp',
        'platform/linux/native/original_ui_garage_widget1_owner50kg.cpp',
        'platform/linux/native/original_ui_light_manager_owner50kg.cpp',
        'platform/linux/native/original_video_preflip50kg.cpp',
        'src/assets/original_ui_garage_model_materials50kg.cpp',
        'src/assets/original_ui_garage_page50kg.cpp',
        'src/assets/original_ui_garage_panel_material50kg.cpp',
        'src/assets/original_ui_garage_prompt_material50kg.cpp',
        'src/assets/original_ui_garage_scene_assets50kg.cpp',
        'src/assets/original_ui_garage_watto_assets50kg.cpp',
        'src/game/original_garage_character50gn.cpp',
        'src/game/original_garage_sequence50gn.cpp',
        'src/game/original_garage_sphere_bounds50gn.cpp',
        'src/game/original_garage_timed_actions50gn.cpp',
        'src/game/original_ui_garage_camera50kg.cpp',
        'src/game/original_ui_garage_model_construction50kg.cpp',
        'src/game/original_ui_garage_particles50kg.cpp',
        'src/game/original_ui_garage_prelit50kg.cpp',
        'src/game/original_ui_garage_diffuse50kg.cpp',
        'src/game/original_ui_garage_palette_packet50kg.cpp',
        'src/game/original_ui_garage_model_view50kg.cpp',
        'src/game/original_ui_garage_specular50kg.cpp',
        'src/game/original_ui_garage_state50kg.cpp',
        'src/game/original_ui_garage_watto_lifecycle50kg.cpp',
        'platform/linux/native/original_ui_options_session50kg.cpp',
        'src/game/original_tournament_result50kg.cpp',
        'src/game/original_ui_tournament_input50kg.cpp',
        'src/assets/original_ui_tournament_page50kg.cpp',
        'platform/linux/native/original_ui_tournament_career_resources50kg.cpp',
        'src/game/original_ui_tournament_camera50kg.cpp',
        'src/game/original_ui_tournament_clean_lifecycle50kg.cpp',
        'src/game/original_ui_tournament_pod_owner50kg.cpp',
        'platform/linux/native/original_ui_tournament_widget_resources50kg.cpp',
        'platform/linux/native/original_ui_tournament_widgets50kg.cpp',
        'platform/linux/native/original_ui_tournament_draw_contribution50kg.cpp',
        'platform/linux/native/original_ui_tournament_session50kg.cpp',
        'src/assets/original_ui_screen_material_reference50kg.cpp',
        'src/game/original_world_environment50kg.cpp',
        'platform/linux/native/original_ui_audio_options50kg.cpp',
        'src/game/original_ui_audio_options50kg.cpp',
        'src/game/original_ui_main_widget_manager50kg.cpp',
        'src/game/original_ui_options_menu_controller50kg.cpp',
        'src/game/original_ui_controls_controller50kg.cpp',
        'src/game/original_ui_credits_controller50kg.cpp',
        'platform/linux/native/original_ui_hall_draw_contribution50kg.cpp',
        'platform/linux/native/original_ui_hall_draw_resources50kg.cpp',
        'platform/linux/native/original_ui_hall_session50kg.cpp',
        'src/assets/original_ui_hall_background50kg.cpp',
        'src/game/original_ui_hall_camera50kg.cpp',
        'src/game/original_ui_hall_controller50kg.cpp',
        'src/game/original_ui_controller_popup50kg.cpp',
        'src/game/original_ui_controller_popup_geometry50kg.cpp',
        'platform/linux/native/original_ui_controller_popup_resources50kg.cpp',
        'src/game/original_save_payload_envelope50kg.cpp',
        'src/game/original_save_career_projection50kg.cpp',
        'src/game/original_save_records_projection50kg.cpp',
        'src/game/original_save_profile_stage50kg.cpp',
        'platform/linux/native/original_save_profile_owner50kg.cpp',
        'platform/linux/native/original_save_native_reader50kg.cpp',
        'platform/linux/native/original_save_native_read_stage50kg.cpp',
        'src/game/original_save_service_protocol50kg.cpp',
        'src/game/original_save_identity_continuation50kg.cpp',
        'src/game/original_ui_memory_card50kg.cpp',
        'src/game/original_ui_memory_card_geometry50kg.cpp',
        'platform/linux/native/original_ui_memory_card_resources50kg.cpp',
        'src/game/original_ui_hall_records50kg.cpp',
        'src/game/original_ui_hall_widgets50kg.cpp',
        'src/game/original_ui_controls_model50kg.cpp',
        'src/assets/original_ui_controls_model_assets50kg.cpp',
        'src/assets/original_ui_controls_specmap50kg.cpp',
        'src/game/original_ui_game_options_controller50kg.cpp',
        'src/game/original_ui_options_menu_widgets50kg.cpp',
        'src/game/original_ui_options_menu_layout50kg.cpp',
        'src/game/original_ui_options_camera50kg.cpp',
        'platform/linux/native/original_ui_options_draw_resources50kg.cpp',
        'platform/linux/native/original_ui_rider_draw_resources50kg.cpp',
        'platform/linux/native/original_ui_rider_profile50kg.cpp',
        'platform/linux/native/original_ui_rider_stat_draw50kg.cpp',
        'platform/linux/native/original_ui_rider_draw_contribution50kg.cpp',
        'platform/linux/native/original_ui_rider_session50kg.cpp',
        'platform/linux/native/original_ui_course_draw_contribution50kg.cpp',
        'platform/linux/native/original_ui_course_draw_resources50kg.cpp',
        'platform/linux/native/original_ui_course_hologram_draw50kg.cpp',
        'platform/linux/native/original_ui_course_session50kg.cpp',
        'src/assets/original_ui_course_background50kg.cpp',
        'src/assets/original_ui_course_hologram_assets50kg.cpp',
        'src/game/original_ui_course_camera50kg.cpp',
        'src/game/original_ui_course_clip_camera50kg.cpp',
        'src/game/original_ui_course_hologram_scene50kg.cpp',
        'src/game/original_ui_course_lighting50kg.cpp',
        'src/game/original_ui_course_page_draw50kg.cpp',
        'src/game/original_ui_course_render_context50kg.cpp',
        'src/game/original_ui_course_selection50kg.cpp',
        'src/game/original_ui_course_mode2_entry50kg.cpp',
        'src/game/original_ui_course_widget_controller50kg.cpp',
        'src/game/original_ui_course_widgets50kg.cpp',
        'src/game/original_ui_rider_voice_choice50kg.cpp',
        'platform/linux/native/original_ui_options_draw_contribution50kg.cpp',
        'src/game/original_ui_rider_selection50kg.cpp',
        'src/game/original_ui_rider_widgets50kg.cpp',
        'src/game/original_ui_rider_visibility50kg.cpp',
        'src/game/original_ui_rider_backend50kg.cpp',
        'src/game/original_ui_rider_material_lifetime50kg.cpp',
        'src/game/original_ui_rider_panel50kg.cpp',
        'src/game/original_ui_rider_selected_packet50kg.cpp',
        'src/game/original_ui_light_manager_lifetime50kg.cpp',
        'src/game/original_ui_rider_stat_owner50kg.cpp',
        'src/game/original_ui_rider_model_packet50kg.cpp',
        'src/game/original_ui_rider_dynamic_owner50kg.cpp',
        'src/assets/original_ui_rider_stat_assets50kg.cpp',
        'src/assets/original_ui_rider_character_assets50kg.cpp',
        'src/game/original_ui_rider_stat_projection50kg.cpp',
        'src/game/model_playback50kg.cpp',
        'src/game/original_ui_rider_character50kg.cpp',
        'src/game/original_ui_user_rotate50kg.cpp',
        'src/assets/original_ui_rider_pod_assets50kg.cpp',
        'src/game/original_ui_rider_binder50kg.cpp',
        'src/assets/original_ui_rider_binder_assets50kg.cpp',
        'src/game/original_ui_rider_binder_animation50kg.cpp',
        'src/game/original_ui_rider_model_scene50kg.cpp',
        'src/game/original_ui_rider_lighting50kg.cpp',
    ):
        absolute = str(ROOT / source)
        if absolute not in argv:
            argv.insert(argv.index('-lz'), absolute)
    # Public version-matched FFmpeg headers; decoder dynamically loads native
    # libraries. No command-line converter or generated texture pack is used.
    argv.insert(1, '-I' + str(ROOT / 'third_party/ffmpeg-7.1.5/include'))
    argv.insert(argv.index('-lz'), str(ROOT / 'src/assets/original_ipu_decoder.cpp'))
    destination.write_text(json.dumps(argv, indent=2) + '\n', encoding='utf-8')

if __name__ == '__main__':
    main()
