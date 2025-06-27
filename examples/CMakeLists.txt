add_executable(example-01 example-01-HelloWorld.cpp)
add_executable(example-02 example-02-Cubes.cpp)

target_link_libraries(example-01 nexus-engine)
target_link_libraries(example-02 nexus-engine)