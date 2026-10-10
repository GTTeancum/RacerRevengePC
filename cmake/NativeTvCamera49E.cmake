# Authenticated TV-camera loading/selection plus retained native input checks.
# No renderer, ownership diagnostic, world-query or collision test dependency.
option(RR_NATIVE_TV_CAMERA_SANITIZERS "Require address/undefined sanitizers" OFF)
add_library(racer_native_tv_camera STATIC
    src/assets/tv_camera49e.cpp src/game/tv_camera_selection49e.cpp
    src/game/control_filter48b.cpp src/game/control_shaping48v.cpp
    src/game/control_binding48w.cpp src/game/player_input48x.cpp
    src/game/device_held48y.cpp src/game/event_dispatch48z.cpp
    src/game/player_events49a.cpp src/game/control_presets49d.cpp)
set(rr_camera_targets racer_native_tv_camera)
set(rr_camera_tests tv_camera49e control_presets49d player_events49a
    event_dispatch48z device_held48y player_input48x control_binding48w
    control_shaping48v control_filter48b)
foreach(rr_test IN LISTS rr_camera_tests)
    add_executable(racer_${rr_test}_check tests/native/${rr_test}.cpp)
    target_link_libraries(racer_${rr_test}_check PRIVATE racer_native_tv_camera)
    list(APPEND rr_camera_targets racer_${rr_test}_check)
endforeach()
foreach(rr_target IN LISTS rr_camera_targets)
    target_include_directories(${rr_target} PUBLIC src/assets src/game)
    target_compile_features(${rr_target} PUBLIC cxx_std_17)
    set_target_properties(${rr_target} PROPERTIES CXX_EXTENSIONS OFF)
    if(MSVC)
        target_compile_options(${rr_target} PRIVATE /W4 /permissive- /fp:strict)
        if(RR_NATIVE_TV_CAMERA_SANITIZERS)
            message(FATAL_ERROR "This sanitizer option requires GCC/Clang; no silent fallback")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${rr_target} PRIVATE -Wall -Wextra -Wpedantic -fno-fast-math -frounding-math -ffp-contract=off)
        if(RR_NATIVE_TV_CAMERA_SANITIZERS)
            target_compile_options(${rr_target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
            target_link_options(${rr_target} PRIVATE -fsanitize=address,undefined)
        endif()
    else()
        message(FATAL_ERROR "Explicit strict floating-point compiler settings are required")
    endif()
endforeach()
enable_testing()
foreach(rr_test IN LISTS rr_camera_tests)
    if(rr_test STREQUAL "tv_camera49e")
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR}/evidence/tvCamera49E/authored/taTVCam.pos)
    else()
        add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    set_tests_properties(${rr_test} PROPERTIES RUN_SERIAL TRUE TIMEOUT 180)
endforeach()
