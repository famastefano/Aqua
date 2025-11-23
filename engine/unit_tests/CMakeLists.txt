include(CTest)

aqua_add_target(unit_tests CONSOLE_EXECUTABLE)
target_link_libraries(unit_tests PRIVATE core)