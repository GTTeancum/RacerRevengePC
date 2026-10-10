# Original control-driven authored engine-node animation checks.
# No renderer, blocked diagnostic, world-query or collision test dependency.
option(RR_NATIVE_ENGINE_ANIMATION_SANITIZERS "Require address/undefined sanitizers" OFF)
add_library(racer_native_engine_animation STATIC
    src/game/model_motion49q.cpp src/game/engine_animation49q.cpp
    src/game/scene_graph49p.cpp src/game/chariot_projection49p.cpp
    src/assets/model_geometry49o.cpp src/assets/model_bindings49o.cpp
    src/game/model_motion49o.cpp src/game/model_bounds49o.cpp
    src/game/authored_geometry49o.cpp src/game/pod_model_geometry49o.cpp
    src/assets/pod_physics49n.cpp src/game/authored_parameters49n.cpp
    src/game/gradient_noise49m.cpp src/game/secondary_body49m.cpp
    src/game/coupled_body_advancement49m.cpp src/game/pod_sphere_constraint49l.cpp
    src/game/advancement_selectors49k.cpp src/game/body_advancement49k.cpp
    src/game/body_integration48c.cpp src/game/vehicle_forces48d.cpp src/game/vehicle_aero48e.cpp
    src/game/body_scene48g.cpp src/game/body_wrapper48h.cpp
    src/game/clock_quantization49j.cpp
    src/game/frame_timing49i.cpp
    src/game/tv_camera_update49f.cpp src/game/camera_view49f.cpp src/game/camera_frame49f.cpp
    src/assets/tv_camera49e.cpp src/game/tv_camera_selection49e.cpp
    src/game/control_filter48b.cpp src/game/control_shaping48v.cpp
    src/game/control_binding48w.cpp src/game/player_input48x.cpp
    src/game/device_held48y.cpp src/game/event_dispatch48z.cpp
    src/game/player_events49a.cpp src/game/control_presets49d.cpp)
set(rr_engine_animation_targets racer_native_engine_animation)
set(rr_engine_animation_tests model_motion49q engine_animation49q engine_animation_diagnostic49q scene_graph_api49p scene_graph49p chariot_projection49p scene_frame_diagnostic49p model_geometry49o model_bindings49o model_motion49o model_bounds49o authored_geometry49o pod_model_geometry49o authored_body_diagnostic49o pod_physics49n authored_parameters_api49n authored_parameters49n authored_body_diagnostic49n gradient_noise49m secondary_body_api49m secondary_wrapper49m coupled_body_advancement49m pod_sphere_constraint_api49l pod_sphere_constraint49l advancement_selectors49k body_advancement49k physics48c physics48d physics48e body_scene48g body_wrapper_nearest49k clock_quantization49j clock_timing_diagnostics49j frame_timing49i camera_frame49f tv_camera49e control_presets49d player_events49a
    event_dispatch48z device_held48y player_input48x control_binding48w
    control_shaping48v control_filter48b)
foreach(rr_test IN LISTS rr_engine_animation_tests)
    add_executable(racer_${rr_test}_check tests/native/${rr_test}.cpp)
    target_link_libraries(racer_${rr_test}_check PRIVATE racer_native_engine_animation)
    target_compile_features(racer_${rr_test}_check PRIVATE cxx_std_20)
    list(APPEND rr_engine_animation_targets racer_${rr_test}_check)
endforeach()
foreach(rr_target IN LISTS rr_engine_animation_targets)
    target_include_directories(${rr_target} PUBLIC src/assets src/game)
    target_compile_features(${rr_target} PUBLIC cxx_std_17)
    set_target_properties(${rr_target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${rr_target} PRIVATE /W4 /permissive- /fp:strict)
        if(RR_NATIVE_ENGINE_ANIMATION_SANITIZERS)
            message(FATAL_ERROR "This sanitizer option requires GCC/Clang; no silent fallback")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${rr_target} PRIVATE -Wall -Wextra -Wpedantic -fno-fast-math -frounding-math -ffp-contract=off)
        if(RR_NATIVE_ENGINE_ANIMATION_SANITIZERS)
            target_compile_options(${rr_target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
            target_link_options(${rr_target} PRIVATE -fsanitize=address,undefined)
        endif()
    else()
        message(FATAL_ERROR "Explicit strict floating-point compiler settings are required")
    endif()
endforeach()
enable_testing()
foreach(rr_test IN LISTS rr_engine_animation_tests)
    if(rr_test STREQUAL "engine_animation_diagnostic49q")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check)
    elseif(rr_test STREQUAL "model_motion49q" OR rr_test STREQUAL "engine_animation49q")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR})
    elseif(rr_test STREQUAL "scene_graph_api49p" OR rr_test STREQUAL "chariot_projection49p")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check)
    elseif(rr_test STREQUAL "scene_graph49p")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg)
    elseif(rr_test STREQUAL "scene_frame_diagnostic49p")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/sceneProjection49P/connected/supplied-physical-scenes49p.bin ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg ${CMAKE_CURRENT_SOURCE_DIR}/evidence/sceneProjection49P/connected/exact-goldens49p.txt)
    elseif(rr_test STREQUAL "model_geometry49o")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O/asset-reader/fixtures)
    elseif(rr_test STREQUAL "model_motion49o")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O/motion)
    elseif(rr_test STREQUAL "model_bounds49o")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg)
    elseif(rr_test STREQUAL "pod_model_geometry49o")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O)
    elseif(rr_test STREQUAL "authored_body_diagnostic49o")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredInitialization49N/authoring-source/Pod01.phy ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredGeometry49O/asset-reader/fixtures)
    elseif(rr_test STREQUAL "model_bindings49o" OR rr_test STREQUAL "authored_geometry49o")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check)
    elseif(rr_test STREQUAL "pod_physics49n" OR rr_test STREQUAL "authored_body_diagnostic49n")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/authoredInitialization49N/authoring-source/Pod01.phy)
    elseif(rr_test MATCHES "^(authored_parameters_api49n|authored_parameters49n|gradient_noise49m|secondary_body_api49m|secondary_wrapper49m|coupled_body_advancement49m|pod_sphere_constraint_api49l|pod_sphere_constraint49l|advancement_selectors49k|body_advancement49k|physics48c|physics48d|physics48e|body_scene48g|body_wrapper_nearest49k|frame_timing49i|clock_quantization49j)$")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check)
    elseif(rr_test STREQUAL "tv_camera49e" OR rr_test STREQUAL "camera_frame49f" OR rr_test STREQUAL "clock_timing_diagnostics49j")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/tvCamera49E/authored/taTVCam.pos)
    else()
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    set_tests_properties(${rr_test} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()
