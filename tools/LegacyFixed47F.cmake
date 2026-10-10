set(rr_fixed_scheduler "${RR_SHARED_ROOT}/build/fixed47F/EeScheduler.cpp")
add_executable(racer_fixed47f_probe "${RACER_PROJECT_DIR}/src/probe.cpp" ${rr_fixed_scheduler})
target_link_libraries(racer_fixed47f_probe PRIVATE racer_generated ps2_runtime)
add_executable(racer_compact47f_probe "${RR_SHARED_ROOT}/build/compact47E/probe.cpp"
    ${rr_fixed_scheduler} ${rr_compact_sources} ${rr_original_objects})
target_link_libraries(racer_compact47f_probe PRIVATE ps2_runtime)
target_compile_definitions(racer_compact47f_probe PRIVATE RR_COMPACT_DEFINE_TABLE_METADATA)
foreach(rr_target IN ITEMS racer_fixed47f_probe racer_compact47f_probe)
    target_include_directories(${rr_target} PRIVATE
        "${RR_SHARED_ROOT}/src/core" "${RR_SHARED_ROOT}/src/legacy"
        "${RR_SHARED_ROOT}/build/compact47E" "${RACER_PROJECT_DIR}/src"
        "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib"
        "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib/Kernel"
        "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
    target_compile_definitions(${rr_target} PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
    target_compile_options(${rr_target} PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
    target_compile_features(${rr_target} PRIVATE cxx_std_20)
    ps2x_stage_ffmpeg_runtime_dlls(${rr_target})
endforeach()
