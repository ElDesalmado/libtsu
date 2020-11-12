
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_executable(test0 test0.cpp)
target_link_libraries(test0 PRIVATE libtsu)
