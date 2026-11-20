#pragma once
// Original root declaration/include environment, shared without duplicating owners.
#include <exception>
#include "../../platform/linux/native/loading_progress50du.h"
#include "cold_load_diagnostics50du.h"
#include "../../platform/linux/native/original_ta_intro_preload50la.h"
#include "original_ui_source_timing50lh.h"
#include "../../src/game/original_entity_chains50la.h" // Inactive source-faithful Entity chain operations.
// runtime_allocations50fl.h stays in the single executable main TU.
#include "original_disc_effects50lb.h"
#include "track_animation_binding50an.h"
#include "track_uv_animation50uv_fixture.h"
#include "../../src/game/original_pause_storage50la.h" // Inactive concrete Pause cold storage.
#include "../../src/game/original_pause_text_geometry50la.h" // Inactive original CPU glyph leaf.
#include "../../src/game/original_pause_texture_geometry50la.h" // Inactive original CPU texture leaf.
#include "../../src/game/original_pause_material_admission50la.h" // Inactive canonical FINAL admission.
#include "../../src/game/original_pause_native_resources50la.h" // Inactive genuine pixels/native policies.
#include "../../src/assets/original_pause_source_data50la.h" // Inactive original Pause cold data.
#include "../../src/assets/original_pause_authored_materials50la.h" // Compile visibility only.
#include "../../src/assets/original_pause_primitive_references50la.h" // No runtime Pause activation.
#include "original_racer_selection50ij.h"
#include "original_course_roster50kg_fixture.h"
#include "original_course_race_targets50kx.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_contact_bank50hw_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "../../src/game/dynamic_impact_events50iw.h"
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_handling50hw_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_grid50hn_fixture.h"
#include "native_racer_draw50hn_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "race_session50hm_fixture.h"
#include "original_camera_assets50kx.h"
#include "original_wreck_rig_inventory50la.h" // Inactive authentic ordered asset join.
// Unused cold wreck-storage compile visibility lives in its dedicated TU.
#include "../../src/game/original_damage_state50la.h"
#include "../../src/game/original_position_stats_numeric50la.h" // Uncalled numeric subset.
#include "../../src/game/original_position_map_bounds50la.h" // Uncalled map bounds subset.
#include "original_position_stats_adapter50la.h" // Uncalled actual view-zero observation.
#include "original_camera_input50kx.h"
#include "native_ta_particle_cold50la_fixture.h" // W0: the one particle prepare (R7)
#include "gc_particle_materials50kg_fixture.h"
#include "original_time_stats_adapter50la.h"
#include "original_damage_observation50la.h"
#include "original_speed_boost_adapter50la.h"
#include "native_query_identities50kx.h"
#include "original_disc_query_bootstrap50le.h"
#include "../../platform/linux/native/original_camera_presentation.h"
#include "../../platform/linux/native/original_ui_controller_navigation50la.h"
#include "../../platform/linux/native/original_video_blocking_native.h"
#include "race_audio50hq_fixture.h"
#include "../../src/game/original_ui_audio_options50kg.h"
#include "race_mode1_host50id_fixture.h"
#include "source_announcer50ie_fixture.h"
#include "race_music50ie_fixture.h"
#include "opponent_engine_audio50hx_fixture.h"
#include "original_ai_audio_owner.h"
#include "original_ai_physical_recovery.h"
#include "original_human_physical_recovery.h"
#include "native_gc_water_session50kg_fixture.h"
#include "../../src/game/original_gc_particle_render50kg.h"
#include "native_gc_other_particle_children50kg_fixture.h"
#include "../../src/game/original_native_collision_categories.h"
#include "engine_damage_audio50ia.h"
#include "wrong_way_progress50go.h"
#include "host_course_driver50gk.h"
#include "player_finish50ia.h"
#include "phase_timing50gk_fixture.h"
#include "track_fps_baseline50la.h"
#include "render_diagnostics50gq_fixture.h"
#include "lap_drive50gj_fixture.h"
#include "live_audio50hf_fixture.h"
#include "pod_speech_authoring50hg_fixture.h"
#include "material_audio_provider50hi_fixture.h"
#include "host_chase_camera50fn_fixture.h"
#include "capture_visibility50fo_fixture.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "boost_controls50fk_fixture.h"
#include "checkpoint_recovery50fi_fixture.h"
#include "pod_cables50fd_fixture.h"
#include "source_texture50fc_fixture.h"
#include "pod_scene_rebase50fb.h"
#include "track_materials50fa_fixture.h"
#include "track_normals50hk_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "sky50gw_fixture.h"
#include "original_track_sky50kx_fixture.h"
#include "track_animation_course50an.h"
#include "track_animation_window50an.h"
// Interactive Linux development client. Cold ownership/setup is adapted from
// terrain_offscreen_drive50dh.cpp, retained as a separate harness. Test-only
// retained-data loading remains, but source-word validation runs separately.
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/host_session_menu50if.h"
#include "original_ui_main_entry50kg.h"
#include "original_ta_camera_issuer_candidate.h" // Native single-Session custody; source constructor remains uncalled.
#include "original_race_tuning50la.h" // Inactive same-source C0 ownership.
#include "original_tunnel_track50la.h" // Inactive genuine spline Entity ownership.
#include "original_world_cold_prefix50la.h" // Inactive same-scene world constructor prefix.
#include "original_race_constructor50la.h" // Inactive consumed-launch race constructor ownership.
#include "original_race_load_prefix50la.h" // Inactive actual early load-owner join.
#include "original_pause_cold_driver50la.h" // Same-Application two-phase Pause resources.
#include "original_race_world_join50la.h" // Inactive same-aggregate race world ownership.
#include "original_consumed_stage50du.h" // W0: the one consumed race stage (R1).
#include "original_native_ta_root_audio50la.h"
#include "../../platform/linux/native/original_spectator_menu_input50la.h" // Physical spectator UI edges; inactive until lifecycle join.
#include "original_race_native_retirement50la.h" // Explicit native ownership close, inactive.
#include "original_race_pause_forward50la.h" // Actual same-world group pause forwarding, inactive.
#include "original_pause_observation50la.h" // Inactive source-only settings observer.
#include "original_race_display_lifetime50la.h"
#include "original_position_stats_primitives50la.h"
#include "original_position_stats_resources50la.h"
#include "original_position_stats_hud50la.h"
#include "original_position_stats_prestage50la.h"
#include "original_racer_cable_allocation50la.h" // Uncalled allocation prefix.
#include "original_cable_selection50la.h" // Uncalled ordinary source update.
#include "../../src/game/original_position_stats_clip50la.h" // Uncalled numeric owner.
#include "../../src/game/original_position_stats_loader_data50la.h"
#include "../../platform/linux/native/keyboard_packet50dr.h"
#include "../../platform/linux/native/selected_control_input50ky.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../platform/linux/native/fixed_step_clock50dt.h"
#include "../../platform/linux/native/vehicle_view_camera50dg.h"
#include "control_presets49d.h"
#include "pod_materials50ed_fixture.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include "vehicle_present50da.h"
#include "controller_ingress50cx.h"
#include "controller_actions50ec.h"
#include "original_human_reset_timers50lc.h"
#include "vehicle_draw_submit50cs.h"
#include "vehicle_draw_assets50cr.h"
#include "zone_registration_events50cq.h"
#include "zone_lifecycle50cl.h"
#include "live_portals50ep_fixture.h"
#include "vehicle_live_query50eq.h"
#include "collision_normal_mode50ja.h"
#include "zone_search_views50cg.h"
#include "vehicle_candidates50ca.h"
#include "candidate_handles50cb.h"
#include "collection_projection50cf.h"
#include "query_prepare48o.h"
#include "vehicle_indexed_query50cd.h"
#include "search_cold50bu.h"
#include "search_view50bt.h"
#include "effect_names50bl.h"
#include "effect_cold50bj.h"
#include "reset_effects50bk.h"
#include "reset_model_bounds50ba.h"
#include "reset_lifecycle50at.h"
#include "main_reset50ar.h"
#include "authored_runtime_ground50cp_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include "track_static_candidates50ig_fixture.h"
#include "track_animation_candidates50an.h"
#include "gameplay_binder_window_candidate_fixture.h"
#include "source_grid50ig_fixture.h"
#include "selected_grid_navigation50ig.h"
#include "../../src/game/track_navigation_view50mb.h"
#include <type_traits>
#include <variant>
#include "track_session_assets50ig_fixture.h"
#include "track_start50ad.h"
#include "track_navigation50eu.h"
#include <memory>
#include <cstdlib>
#include <chrono>
#include <cfenv>
#include <cmath>
#include <thread>
#include <limits>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include "vehicle_base_finish50y.h"
#include "pod_flame50w.h"
#include "engine_flare50gv.h"
// type2_fixture_io50i.h defines main-only diagnostic readers.
#include "vehicle_static_query50h.h"
#include "movement_static_query50l.h"
#include "vehicle_secondary_contact50j.h"
#include "vehicle_static_response50g.h"
#include "../../src/assets/spring_authoring50f.h"
#include "../../src/assets/repair_authoring50a.h"
namespace f=runtime_startup50fl;
using namespace rr::game;using J=nlohmann::json;
#include "terrain_interactive_storage50du.h"
