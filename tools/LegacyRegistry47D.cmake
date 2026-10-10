# Optional offline linked validation, using the unchanged generated objects.
# It is not included by the native game hook until explicitly selected.
add_executable(racer_registry47d_check
    "${RR_SHARED_ROOT}/tests/legacy_registry47d.cpp"
    "${RR_SHARED_ROOT}/src/core/compact_registry.cpp"
    "${RR_SHARED_ROOT}/build/registry47D/bindings.cpp")
target_include_directories(racer_registry47d_check PRIVATE "${RR_SHARED_ROOT}/src/core")
target_link_libraries(racer_registry47d_check PRIVATE racer_generated ps2_runtime)
ps2x_stage_ffmpeg_runtime_dlls(racer_registry47d_check)
