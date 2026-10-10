add_library(rr_gs_profile47o OBJECT "${RR_SHARED_ROOT}/build/gsProfile47O/gs_pcsx2_backend41n.cpp")
target_include_directories(rr_gs_profile47o PRIVATE "${RR_SHARED_ROOT}/src/legacy" "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
target_compile_options(rr_gs_profile47o PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
target_compile_definitions(rr_gs_profile47o PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
target_compile_features(rr_gs_profile47o PRIVATE cxx_std_20)
add_executable(racer_gsprofile47o_fixed_probe "${RR_SHARED_ROOT}/build/memory47N/memory-probe.cpp"
    "${RR_SHARED_ROOT}/build/leaf47M/adapter.cpp" "${RR_SHARED_ROOT}/build/fixed47F/EeScheduler.cpp"
    ${rr_original_objects} $<TARGET_OBJECTS:rr_registry47n> $<TARGET_OBJECTS:rr_gs47l> $<TARGET_OBJECTS:rr_gs_profile47o>)
target_include_directories(racer_gsprofile47o_fixed_probe PRIVATE "${RR_SHARED_ROOT}/src/core" "${RR_SHARED_ROOT}/src/legacy"
    "${RACER_PROJECT_DIR}/src" "${RR_SHARED_ROOT}/build/compact47E" "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib"
    "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib/Kernel" "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
target_compile_definitions(racer_gsprofile47o_fixed_probe PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
target_compile_options(racer_gsprofile47o_fixed_probe PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
target_compile_features(racer_gsprofile47o_fixed_probe PRIVATE cxx_std_20)
target_link_libraries(racer_gsprofile47o_fixed_probe PRIVATE ps2_runtime rr_iop47j2 rr_leaf47m)
ps2x_stage_ffmpeg_runtime_dlls(racer_gsprofile47o_fixed_probe)
