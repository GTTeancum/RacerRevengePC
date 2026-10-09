# Original secondary wrapper and coupled bodies and retained body/timing/input/camera checks.
# No renderer, blocked diagnostic, world-query or collision test dependency.
option(RR_NATIVE_SECONDARY_WRAPPER_SANITIZERS "Require address/undefined sanitizers" OFF)
add_library(racer_native_secondary_wrapper STATIC
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
set(rr_secondary_wrapper_targets racer_native_secondary_wrapper)
set(rr_secondary_wrapper_tests gradient_noise49m secondary_body_api49m secondary_wrapper49m coupled_body_advancement49m pod_sphere_constraint_api49l pod_sphere_constraint49l advancement_selectors49k body_advancement49k physics48c physics48d physics48e body_scene48g body_wrapper_nearest49k clock_quantization49j clock_timing_diagnostics49j frame_timing49i camera_frame49f tv_camera49e control_presets49d player_events49a
    event_dispatch48z device_held48y player_input48x control_binding48w
    control_shaping48v control_filter48b)
foreach(rr_test IN LISTS rr_secondary_wrapper_tests)
    add_executable(racer_${rr_test}_check tests/native/${rr_test}.cpp)
    target_link_libraries(racer_${rr_test}_check PRIVATE racer_native_secondary_wrapper)
    target_compile_features(racer_${rr_test}_check PRIVATE cxx_std_20)
    list(APPEND rr_secondary_wrapper_targets racer_${rr_test}_check)
endforeach()
foreach(rr_target IN LISTS rr_secondary_wrapper_targets)
    target_include_directories(${rr_target} PUBLIC src/assets src/game)
    target_compile_features(${rr_target} PUBLIC cxx_std_17)
    set_target_properties(${rr_target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${rr_target} PRIVATE /W4 /permissive- /fp:strict)
        if(RR_NATIVE_SECONDARY_WRAPPER_SANITIZERS)
            message(FATAL_ERROR "This sanitizer option requires GCC/Clang; no silent fallback")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${rr_target} PRIVATE -Wall -Wextra -Wpedantic -fno-fast-math -frounding-math -ffp-contract=off)
        if(RR_NATIVE_SECONDARY_WRAPPER_SANITIZERS)
            target_compile_options(${rr_target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
            target_link_options(${rr_target} PRIVATE -fsanitize=address,undefined)
        endif()
    else()
        message(FATAL_ERROR "Explicit strict floating-point compiler settings are required")
    endif()
endforeach()
enable_testing()
foreach(rr_test IN LISTS rr_secondary_wrapper_tests)
    if(rr_test MATCHES "^(gradient_noise49m|secondary_body_api49m|secondary_wrapper49m|coupled_body_advancement49m|pod_sphere_constraint_api49l|pod_sphere_constraint49l|advancement_selectors49k|body_advancement49k|physics48c|physics48d|physics48e|body_scene48g|body_wrapper_nearest49k|frame_timing49i|clock_quantization49j)$")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check)
    elseif(rr_test STREQUAL "tv_camera49e" OR rr_test STREQUAL "camera_frame49f" OR rr_test STREQUAL "clock_timing_diagnostics49j")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/tvCamera49E/authored/taTVCam.pos)
    else()
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    set_tests_properties(${rr_test} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
endforeach()
