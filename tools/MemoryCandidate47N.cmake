add_library(rr_registry47n OBJECT ${rr_compact_sources})
target_include_directories(rr_registry47n PRIVATE "${RR_SHARED_ROOT}/src/core" "${RR_SHARED_ROOT}/src/legacy" "${RR_SHARED_ROOT}/build/compact47E" "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib" "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
target_compile_definitions(rr_registry47n PRIVATE RR_COMPACT_DEFINE_TABLE_METADATA "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
target_compile_options(rr_registry47n PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
target_compile_features(rr_registry47n PRIVATE cxx_std_20)
foreach(variant IN ITEMS registry memory)
    set(target racer_${variant}47n_fixed_probe)
    add_executable(${target} "${RR_SHARED_ROOT}/build/memory47N/${variant}-probe.cpp"
        "${RR_SHARED_ROOT}/build/fixed47F/EeScheduler.cpp" ${rr_original_objects}
        $<TARGET_OBJECTS:rr_registry47n> $<TARGET_OBJECTS:rr_gs47l>)
    target_include_directories(${target} PRIVATE "${RR_SHARED_ROOT}/src/core" "${RR_SHARED_ROOT}/src/legacy"
        "${RACER_PROJECT_DIR}/src" "${RR_SHARED_ROOT}/build/compact47E"
        "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib" "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib/Kernel"
        "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
    target_compile_definitions(${target} PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
    target_compile_options(${target} PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
    target_compile_features(${target} PRIVATE cxx_std_20)
    target_link_libraries(${target} PRIVATE ps2_runtime rr_iop47j2)
    if(variant STREQUAL "memory")
        target_sources(${target} PRIVATE "${RR_SHARED_ROOT}/build/leaf47M/adapter.cpp")
        target_link_libraries(${target} PRIVATE rr_leaf47m)
    endif()
    target_link_options(${target} PRIVATE "/MAP:${RR_SHARED_ROOT}/build/memory47N/${variant}.map")
    ps2x_stage_ffmpeg_runtime_dlls(${target})
endforeach()
