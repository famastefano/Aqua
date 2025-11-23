project(engine LANGUAGES CXX)

add_library(_lib_engine INTERFACE)
add_library(aqua::engine ALIAS _lib_engine)

aqua_add_subtarget(core aqua::engine SHARED_LIBRARY)

if(AQUA_ENABLE_TESTS)
    add_subdirectory(unit_tests)
endif()