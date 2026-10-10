include(${CMAKE_CURRENT_LIST_DIR}/NativePc48E.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/NativeWorld48F.cmake)
target_sources(racer_native_pc PRIVATE platform/windows/native/import_service48f.cpp)
target_compile_options(racer_native_pc PRIVATE /fp:strict)
