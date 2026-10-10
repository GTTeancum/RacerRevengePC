# Portable numerical checks only. No viewer, importer, Direct3D or platform stubs.
# Configure: cmake -S . -B ../build-spring -DRR_NATIVE_SPRING_ONLY=ON
# Build: cmake --build ../build-spring --parallel 1
# Test: ctest --test-dir ../build-spring --output-on-failure
option(RR_NATIVE_SPRING_SANITIZERS "Enable address/undefined sanitizers for spring checks" OFF)

foreach(rr_fixture IN ITEMS
        evidence/spring48J/friction-fixtures.bin
        evidence/spring48K/impact-fixtures.bin
        evidence/springOrchestration48K/coordinator-native-fixtures.json
        evidence/springResponse48L/coordinator-fixtures.json
        evidence/springResponse48L/spring-force-fixtures.bin
        evidence/springResponse48L/secondary-fixtures.bin)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${rr_fixture}")
        message(FATAL_ERROR "BLOCKED: required spring fixture is missing: ${rr_fixture}. Restore verified evidence before configuring; fixture tests must not be skipped.")
    endif()
endforeach()

add_library(racer_native_spring_game STATIC
    src/game/spring_contact48j.cpp
    src/game/spring_impulse48k.cpp
    src/game/spring_model48k.cpp
    src/game/spring_force48l.cpp)
target_include_directories(racer_native_spring_game PUBLIC src/game)
target_compile_features(racer_native_spring_game PUBLIC cxx_std_17)

add_executable(racer_native_spring_contact_check tests/native/spring_contact48j.cpp)
add_executable(racer_native_spring_impulse_check tests/native/spring_impulse48k.cpp)
add_executable(racer_native_spring_model_check tests/native/spring_model48k.cpp)
add_executable(racer_native_spring_force_check tests/native/spring_force48l.cpp)
add_executable(racer_native_spring_response_check tests/native/spring_response48l.cpp)
set(rr_spring_checks
    racer_native_spring_contact_check
    racer_native_spring_impulse_check
    racer_native_spring_model_check
    racer_native_spring_force_check
    racer_native_spring_response_check)

# Ninja honors this pool even if its caller requests a parallel build. Other
# generators should use --parallel 1, as shown above. CTest is always serial.
set_property(GLOBAL APPEND PROPERTY JOB_POOLS rr_native_spring_serial=1)
foreach(rr_target IN ITEMS racer_native_spring_game ${rr_spring_checks})
    set_target_properties(${rr_target} PROPERTIES
        CXX_EXTENSIONS OFF
        JOB_POOL_COMPILE rr_native_spring_serial
        JOB_POOL_LINK rr_native_spring_serial)
    if(MSVC)
        target_compile_options(${rr_target} PRIVATE /W4 /permissive- /fp:strict)
        if(RR_NATIVE_SPRING_SANITIZERS)
            message(FATAL_ERROR "RR_NATIVE_SPRING_SANITIZERS requires GCC or Clang; no silent sanitizer fallback is allowed.")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${rr_target} PRIVATE
            -Wall -Wextra -Wpedantic -fno-fast-math -frounding-math -ffp-contract=off)
        if(RR_NATIVE_SPRING_SANITIZERS)
            target_compile_options(${rr_target} PRIVATE
                -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
            target_link_options(${rr_target} PRIVATE -fsanitize=address,undefined)
        endif()
    else()
        message(FATAL_ERROR "Spring checks require MSVC, GCC or Clang with explicit strict floating-point options.")
    endif()
endforeach()
foreach(rr_target IN LISTS rr_spring_checks)
    target_link_libraries(${rr_target} PRIVATE racer_native_spring_game)
    target_include_directories(${rr_target} PRIVATE third_party/native-json)
endforeach()

enable_testing()
add_test(NAME native_spring_contact COMMAND racer_native_spring_contact_check "${CMAKE_SOURCE_DIR}")
add_test(NAME native_spring_impact COMMAND racer_native_spring_impulse_check "${CMAKE_SOURCE_DIR}")
add_test(NAME native_spring_model COMMAND racer_native_spring_model_check "${CMAKE_SOURCE_DIR}")
add_test(NAME native_spring_force COMMAND racer_native_spring_force_check "${CMAKE_SOURCE_DIR}")
add_test(NAME native_spring_response COMMAND racer_native_spring_response_check "${CMAKE_SOURCE_DIR}")
set_tests_properties(native_spring_contact native_spring_impact native_spring_model native_spring_force native_spring_response
    PROPERTIES RUN_SERIAL TRUE TIMEOUT 120)
