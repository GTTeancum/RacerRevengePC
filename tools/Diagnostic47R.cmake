include("${RR_SHARED_ROOT}/build/diagnostic47R/Probe.cmake")
add_executable(racer_diagnostic47r_test "${RR_SHARED_ROOT}/tests/diagnostic_lines47r.cpp")
target_include_directories(racer_diagnostic47r_test PRIVATE "${RR_SHARED_ROOT}/src/legacy")
target_compile_features(racer_diagnostic47r_test PRIVATE cxx_std_20)
