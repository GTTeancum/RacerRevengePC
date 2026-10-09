# Include after NativePc48E.cmake (or equivalent decoder target setup).
target_sources(racer_native_import_decode PRIVATE
    src/assets/import_world48f.cpp src/assets/world_asset48f.cpp)
add_executable(racer_native_world_check tests/native/import_world48f.cpp)
target_link_libraries(racer_native_world_check PRIVATE racer_native_import_decode)
if(WIN32)
    target_link_libraries(racer_native_world_check PRIVATE bcrypt)
endif()
target_include_directories(racer_native_world_check PRIVATE third_party/native-json)
target_compile_features(racer_native_world_check PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(racer_native_world_check PRIVATE /W4 /permissive- /fp:strict)
    target_compile_options(racer_native_import_decode PRIVATE /fp:strict)
else()
    target_compile_options(racer_native_world_check PRIVATE -ffp-contract=off)
    target_compile_options(racer_native_import_decode PRIVATE -ffp-contract=off)
endif()
add_test(NAME native_world_decoder COMMAND racer_native_world_check ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/world-report48f.json ${CMAKE_BINARY_DIR}/world-fixtures48f)
