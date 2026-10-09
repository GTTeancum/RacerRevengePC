include("${RR_SHARED_ROOT}/build/event47S/Probe.cmake")
add_executable(racer_event47s_test "${RR_SHARED_ROOT}/tests/event_gate47s.cpp"
    "${RR_SHARED_ROOT}/build/event47S/EeScheduler.cpp" ${rr_original_objects}
    $<TARGET_OBJECTS:rr_registry47n> $<TARGET_OBJECTS:rr_gs47l> $<TARGET_OBJECTS:rr_gs_profile47o>)
target_include_directories(racer_event47s_test PRIVATE "${RR_SHARED_ROOT}/src/core" "${RR_SHARED_ROOT}/src/legacy"
    "${RACER_PROJECT_DIR}/src" "${RR_SHARED_ROOT}/build/compact47E"
    "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib" "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib/Kernel"
    "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
target_compile_definitions(racer_event47s_test PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
target_compile_options(racer_event47s_test PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
target_compile_features(racer_event47s_test PRIVATE cxx_std_20)
target_link_libraries(racer_event47s_test PRIVATE ps2_runtime rr_iop47q rr_leaf47m)
ps2x_stage_ffmpeg_runtime_dlls(racer_event47s_test)
