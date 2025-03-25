cmake_minimum_required(VERSION 3.26)
project(VoxelEngine)

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_MakeAvailable(glfw)
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
endif()

FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_MakeAvailable(glad)
    set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
    set(GLAD_API "gl=4.6" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest")
    set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")
endif()

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_MakeAvailable(glm)
endif()

set(CMAKE_CXX_STANDARD 20)

set(IMGUI_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/_deps/imgui-src)

FetchContent_Populate(imgui
        URL https://github.com/ocornut/imgui/archive/docking.zip
        SOURCE_DIR ${IMGUI_SRC_DIR}
)

add_library(imgui STATIC
        ${IMGUI_SRC_DIR}/imgui.cpp
        ${IMGUI_SRC_DIR}/imgui_draw.cpp
        ${IMGUI_SRC_DIR}/imgui_demo.cpp
        ${IMGUI_SRC_DIR}/imgui_tables.cpp
        ${IMGUI_SRC_DIR}/imgui_widgets.cpp

        ${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui PUBLIC glfw)

target_include_directories(imgui
        PUBLIC
        ${IMGUI_SRC_DIR}
        ${IMGUI_SRC_DIR}/backends
)

add_executable(VoxelEngine main.cpp
        src/Shader.cpp
        src/QuadMesh.cpp
        src/CubeMesh.cpp
        src/Chunk.cpp
        src/DebugWindow.cpp
        src/Texture.cpp
        src/World.cpp
        src/Timer.cpp
        src/ChunkId.cpp
        src/Logger.cpp)

add_dependencies(VoxelEngine glfw)
add_dependencies(VoxelEngine glad)
add_dependencies(VoxelEngine glm)

target_include_directories(VoxelEngine PRIVATE include)
target_link_libraries(VoxelEngine glad glfw glm imgui)

file(COPY shaders DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
