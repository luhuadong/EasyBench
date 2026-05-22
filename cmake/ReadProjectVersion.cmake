# Print PROJECT_VERSION from an existing build tree (for packaging scripts).
# Usage: cmake -P cmake/ReadProjectVersion.cmake -DBUILD_DIR=build

if(NOT DEFINED BUILD_DIR)
    message(FATAL_ERROR "BUILD_DIR is not set")
endif()

set(_cache "${BUILD_DIR}/CMakeCache.txt")
if(NOT EXISTS "${_cache}")
    message(FATAL_ERROR "CMake cache not found: ${_cache}. Run: cmake -B ${BUILD_DIR}")
endif()

file(READ "${_cache}" _contents)
if(_contents MATCHES "CMAKE_PROJECT_VERSION:STATIC=([0-9.]+)")
    message("${CMAKE_MATCH_1}")
else()
    message(FATAL_ERROR "CMAKE_PROJECT_VERSION not found in ${_cache}")
endif()
