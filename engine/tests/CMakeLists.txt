include(CTest)

add_executable(_test_engine main.cpp)
target_link_libraries(_test_engine PRIVATE aqua::engine)