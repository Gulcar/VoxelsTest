cmake_minimum_required(VERSION 3.20)

project(VoxelsTest)

add_executable(${PROJECT_NAME}
    src/Main.cpp
    src/VoxelRenderer.cpp
    src/Chunk.cpp

    deps/glad-3.3-khrdebug/src/glad.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

add_subdirectory(deps/glfw)
add_subdirectory(deps/libnoise)

find_package(OpenMP REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    libnoise
    OpenMP::OpenMP_CXX
)

target_include_directories(${PROJECT_NAME} PRIVATE
    deps
    deps/glfw/include
    deps/glad-3.3-khrdebug/include
    deps/glm
    deps/libnoise/include
)
