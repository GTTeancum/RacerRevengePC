#pragma once
// World-stage providers only. Retained imports keep their original order; later
// renderer, HUD, native-session and source-parameter implementations stay with
// their real consumers. Existing roster storage has its own exact type seam.
// Actual owners, nested Borrow types and all world operations are unchanged.
#include <exception>
#include "../../platform/linux/native/loading_progress50du.h"
#include "cold_load_diagnostics50du.h"
#include "../../platform/linux/native/original_ta_intro_preload50la.h"
#include "original_ui_source_timing50lh.h"
#include "mc_particle_render50mc.h"
#include "../../src/game/original_entity_chains50la.h" // Inactive source-faithful Entity chain operations.
// runtime_allocations50fl.h stays in the single executable main TU.
#include "original_disc_effects50lb.h"
#include "ga_original_world50lh.h"
#include "gc_original_world50lm.h"
#include "ta_original_world50le.h"
#include "ma_sa_original_world50lm.h"
#include "gb_original_world50lm.h"
#include "gb_track_fog50gb.h"
#include "mc_track_fog50mc.h"
#include "mc_water50mc.h"
#include "mc_sky50mc.h"
#include "mc_camera_assets50mc.h"
#include "mc_positional_sound50mc.h"
#include "gb_positional_sound50gb.h"
#include "mc_cold_visibility50mc.h"
#include "gb_particle_render50gb.h"
#include "gb_particle_visual50gb.h"
#include "gb_query50gb.h"
#include "mc_query50mc.h"
#include "gb_cold_visibility50gb.h"
#include "gb_animation_binding50gb.h"
#include "gb_collision_world50gb.h"
#include "gb_particle_runtime50gb.h"
#include "gb_animation_runtime50gb.h"
#include "gb_trigger_actions50gb.h"
#include "gb_navigation50gb.h"
#include "source_grid50gb_fixture.h"
#include "gb_pod_sound50gb.h"
#include "rc_original_world50lm.h"
#include "rc_crystal50rc.h"
#include "rc_particle_visual50rc.h"
#include "tc_original_world50lm.h"
#include "mb_original_world50lm.h"
#include "mb_navigation50mb.h"
#include "mb_collision_world50mb.h"
#include "mb_source_trigger50mb.h"
#include "tc_collision_world50tc.h"
#include "tb_collision_world50tb.h"
#include "tb_door_binding50tb.h"
#include "tb_cold_visibility50tb.h"
#include "rc_collision_world50rc.h"
#include "tc_gate_visibility50tc.h"
#include "tc_cold_visibility50tc.h"
#include "tb_cold_visibility50tb.h"
#include "source_grid50tc_fixture.h"
#include "source_grid50tb_fixture.h"
#include "native_cold_model_retirement50lh.h"
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
#include "native_ta_particle_cold50la_fixture.h"
#include "rc_particle_runtime50rc.h"
#include "rc_particle_render50rc.h"
#include "rc_particle_visual50rc.h"
#include "ra_cold_aggregate50la.h" // Inactive cold owner: compile visibility only.
// Unused cold wreck-storage compile visibility lives in its dedicated TU.
#include "sc_trigger_authoring50la.h" // Inactive SC source/zero-sky/trigger authoring only.
#include "../../src/game/original_damage_state50la.h"
#include "../../src/game/original_ra_static_fragment_bytes.h" // Inactive native byte arithmetic; no renderer admission.
#include "../../src/game/original_position_stats_numeric50la.h" // Uncalled numeric subset.
#include "../../src/game/original_position_map_bounds50la.h" // Uncalled map bounds subset.
#include "original_position_stats_adapter50la.h" // Uncalled actual view-zero observation.
#include "ra_static_model_factory50la.h" // Uncalled cold ownership stage.
#include "ra_track_particle_manager50la.h" // Inactive same-Application manager stage.
#include "ra_source_camera_factory50la.h" // Inactive same-Bank camera wrapper.
#include "ra_cold_roster_storage50la.h"
#include "ra_crowd_image50la.h" // Inactive frame0 verifier; no image/lease is minted.
#include "ra_tga_source50la.h" // Inactive exact-source TGA verifier; no population.
#include "ra_fire_model_sources50la.h" // Inactive authentic payload/reference stage only.
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
#include "original_ai_physical_recovery.h"
#include "native_gc_water_session50kg_fixture.h"
#include "gc_particle_materials50kg_fixture.h"
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
#include "rc_positional_sound50rc.h"
#include "tb_positional_sound50tb.h"
#include "tb_pod_sound50tb.h"
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
#include "material_alpha50gx_fixture.h"
#include "sky50gw_fixture.h"
#include "original_track_sky50kx_fixture.h"
#include "ma_trigger_candidates_fixture.h"
#include "sc_trigger_candidates_fixture.h"
#include "sc_runtime_ground50la.h"
#include "sa_trigger_candidates_fixture.h"
#include "sa_animated_world_fixture.h"
// Interactive Linux development client. Cold ownership/setup is adapted from
// terrain_offscreen_drive50dh.cpp, retained as a separate harness. Test-only
// retained-data loading remains, but source-word validation runs separately.
#include "../../platform/linux/native/game_host50er.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_tuning50la.h" // Inactive same-source C0 ownership.
#include "original_tunnel_track50la.h" // Inactive genuine spline Entity ownership.
#include "original_world_cold_prefix50la.h" // Inactive same-scene world constructor prefix.
#include "original_race_constructor50la.h" // Inactive consumed-launch race constructor ownership.
#include "original_race_load_prefix50la.h" // Inactive actual early load-owner join.
#include "original_pause_cold_driver50la.h" // Same-Application two-phase Pause resources.
#include "original_race_world_join50la.h" // Inactive same-aggregate race world ownership.
#include "original_consumed_ra_stage50du.h" // Existing consumed RA ownership.
#include "original_consumed_gc_stage50du.h" // Distinct actual GC counted prefix.
#include "original_consumed_ta_stage50du.h" // Genuine TA race/world audio ownership.
#include "original_consumed_ma_stage50du.h" // Actual selected-track counted prefix.
#include "original_consumed_mb_stage50du.h"
#include "original_consumed_sc_stage50du.h"
#include "original_consumed_sa_stage50du.h" // Actual selected-track counted prefix.
#include "original_consumed_ga_stage50du.h" // Actual selected-track counted prefix.
#include "original_native_ta_root_audio50la.h"
#include "../../platform/linux/native/original_spectator_menu_input50la.h" // Physical spectator UI edges; inactive until lifecycle join.
#include "original_race_native_retirement50la.h" // Explicit native ownership close, inactive.
#include "original_race_pause_forward50la.h" // Actual same-world group pause forwarding, inactive.
#include "original_pause_observation50la.h" // Inactive source-only settings observer.
#include "original_race_display_lifetime50la.h"
#include "original_position_stats_primitives50la.h"
#include "original_position_stats_resources50la.h"
#include "original_position_stats_prestage50la.h"
#include "original_racer_cable_allocation50la.h" // Uncalled allocation prefix.
#include "../../src/game/original_position_stats_clip50la.h" // Uncalled numeric owner.
#include "../../src/game/original_position_stats_loader_data50la.h"
#include "../../platform/linux/native/host_race_results50il.h"
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
#include "gc_animated_candidates50ig_fixture.h"
#include "gc_trigger_candidates50kg_fixture.h"
#include "ga_animated_candidates_fixture.h"
#include "ga_trigger_authoring_fixture.h"
#include "ga_trigger_actions_fixture.h"
#include "ga_trigger_candidates_fixture.h"
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
