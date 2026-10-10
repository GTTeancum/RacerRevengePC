# Focused portable outer-group limited-list type3 query; no renderer dependency.
option(RR_NATIVE_GROUP_QUERY_SANITIZERS "Require address/undefined sanitizers" OFF)
foreach(rr_fixture IN ITEMS
        evidence/groupQuery48U/native-fixtures.json)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${rr_fixture}")
        message(FATAL_ERROR "BLOCKED: required original-derived collection fixture is missing: ${rr_fixture}")
    endif()
endforeach()
add_library(racer_native_group_query STATIC
    src/game/spring_records48n.cpp
    src/game/spring_callback48n.cpp
    src/game/spring_type3_callback48p.cpp
    src/game/collision_type3_leaf48p.cpp
    src/game/collision_type3_node48p.cpp
    src/game/type3_prepare48p.cpp
    src/game/type3_query48p.cpp
    src/game/group_shape48t.cpp
    src/game/group_query48u.cpp)
set(rr_group_query_targets racer_native_group_query)
foreach(rr_test IN ITEMS group_query48u)
    add_executable(racer_${rr_test}_check tests/native/${rr_test}.cpp)
    target_link_libraries(racer_${rr_test}_check PRIVATE racer_native_group_query)
    target_include_directories(racer_${rr_test}_check PRIVATE third_party/native-json)
    list(APPEND rr_group_query_targets racer_${rr_test}_check)
endforeach()
set_property(GLOBAL APPEND PROPERTY JOB_POOLS rr_group_query_serial=1)
foreach(rr_target IN LISTS rr_group_query_targets)
    target_include_directories(${rr_target} PUBLIC src/game src/assets)
    target_compile_features(${rr_target} PUBLIC cxx_std_17)
    set_target_properties(${rr_target} PROPERTIES CXX_EXTENSIONS OFF JOB_POOL_COMPILE rr_group_query_serial JOB_POOL_LINK rr_group_query_serial)
    if(MSVC)
        target_compile_options(${rr_target} PRIVATE /W4 /permissive- /fp:strict)
        if(RR_NATIVE_GROUP_QUERY_SANITIZERS)
            message(FATAL_ERROR "This sanitizer option requires GCC/Clang; no silent fallback")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${rr_target} PRIVATE -Wall -Wextra -Wpedantic -fno-fast-math -frounding-math -ffp-contract=off)
        if(RR_NATIVE_GROUP_QUERY_SANITIZERS)
            target_compile_options(${rr_target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
            target_link_options(${rr_target} PRIVATE -fsanitize=address,undefined)
        endif()
    else()
        message(FATAL_ERROR "Explicit strict floating-point compiler settings are required")
    endif()
endforeach()
enable_testing()
foreach(rr_test IN ITEMS group_query48u)
    set(rr_argument "${CMAKE_SOURCE_DIR}/evidence/groupQuery48U/native-fixtures.json")
    add_test(NAME ${rr_test} COMMAND racer_${rr_test}_check ${rr_argument})
    set_tests_properties(${rr_test} PROPERTIES RUN_SERIAL TRUE TIMEOUT 120)
endforeach()
