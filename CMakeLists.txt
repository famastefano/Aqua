cmake_minimum_required(VERSION 4.0)

project(aqua LANGUAGES CXX VERSION 0.1.0)

include(cmake/target_helpers.cmake)
include(cmake/ide_helpers.cmake)

if(AQUA_SANITIZERS_CXX_FLAGS)
    list(APPEND CMAKE_CXX_FLAGS "${AQUA_SANITIZERS_CXX_FLAGS}")
endif()

add_subdirectory(engine)

aqua_update_compilation_database()