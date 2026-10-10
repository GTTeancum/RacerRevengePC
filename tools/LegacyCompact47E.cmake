set(rr_compact_sources
    "${RR_SHARED_ROOT}/build/compact47E/ps2_runtime.cpp"
    "${RR_SHARED_ROOT}/build/compact47E/registry_data47e.cpp"
    "${RR_SHARED_ROOT}/src/core/compact_registry.cpp"
    "${RR_SHARED_ROOT}/src/legacy/registry_adapter47e.cpp")
add_executable(racer_compact47e_contract "${RR_SHARED_ROOT}/tests/legacy_registry47e.cpp" ${rr_compact_sources})
target_link_libraries(racer_compact47e_contract PRIVATE racer_generated ps2_runtime)
# Retain every original game instruction object, excluding only three table
# initializer objects. The compact data provides their exact original mapping.
set(rr_original_objects "$<FILTER:$<TARGET_OBJECTS:racer_generated>,EXCLUDE,(batch_055[.]cpp|register[.]cpp)[.]obj$>")
file(GENERATE OUTPUT "${RR_SHARED_ROOT}/evidence/47E/all-generated-objects.txt"
    CONTENT "$<JOIN:$<TARGET_OBJECTS:racer_generated>,\n>\n")
file(GENERATE OUTPUT "${RR_SHARED_ROOT}/evidence/47E/compact-generated-objects.txt"
    CONTENT "$<JOIN:${rr_original_objects},\n>\n")
add_executable(racer_compact47e_probe "${RR_SHARED_ROOT}/build/compact47E/probe.cpp"
    ${rr_compact_sources} ${rr_original_objects})
target_link_libraries(racer_compact47e_probe PRIVATE ps2_runtime)
target_compile_definitions(racer_compact47e_probe PRIVATE RR_COMPACT_DEFINE_TABLE_METADATA)
if(MSVC)
    target_link_options(racer_compact47e_probe PRIVATE "/MAP:${RR_SHARED_ROOT}/evidence/47E/compact-probe.map")
endif()
foreach(rr_target IN ITEMS racer_compact47e_contract racer_compact47e_probe)
    target_include_directories(${rr_target} PRIVATE
        "${RR_SHARED_ROOT}/src/core" "${RR_SHARED_ROOT}/src/legacy"
        "${RR_SHARED_ROOT}/build/compact47E" "${RACER_PROJECT_DIR}/src"
        "${CMAKE_SOURCE_DIR}/ps2xRuntime/src/lib"
        "$<TARGET_PROPERTY:ps2_runtime,INCLUDE_DIRECTORIES>")
    target_compile_definitions(${rr_target} PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_DEFINITIONS>")
    target_compile_options(${rr_target} PRIVATE "$<TARGET_PROPERTY:ps2_runtime,COMPILE_OPTIONS>")
    target_compile_features(${rr_target} PRIVATE cxx_std_20)
    ps2x_stage_ffmpeg_runtime_dlls(${rr_target})
endforeach()
