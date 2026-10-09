# Retain authored metadata alongside the existing native visual world buffers.
# Prior target configurations and evidence are preserved, without query changes.
include(${CMAKE_CURRENT_LIST_DIR}/NativePc48U.cmake)
target_sources(racer_native_import_decode PRIVATE src/assets/world_index49b.cpp)
