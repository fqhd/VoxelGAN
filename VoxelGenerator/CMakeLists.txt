cmake_minimum_required(VERSION 3.12)
project(VoxelGenerator VERSION 1.0.0)

add_executable(voxel_generator src/main.cpp)

add_subdirectory(deps/glfw)
add_subdirectory(deps/glm)

target_link_libraries(voxel_generator PUBLIC glfw)
target_link_libraries(voxel_generator PUBLIC glm)