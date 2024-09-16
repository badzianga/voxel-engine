cmake_minimum_required(VERSION 3.26)
project(MineCppraft)

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

add_executable(MineCppraft main.cpp
        src/Shader.cpp
        src/QuadMesh.cpp)

add_dependencies(MineCppraft glfw)
add_dependencies(MineCppraft glad)
add_dependencies(MineCppraft glm)

target_include_directories(MineCppraft PRIVATE include)
target_link_libraries(MineCppraft glad glfw glm)

configure_file(shaders/quad.vert ${CMAKE_CURRENT_BINARY_DIR}/shaders/quad.vert COPYONLY)
configure_file(shaders/quad.frag ${CMAKE_CURRENT_BINARY_DIR}/shaders/quad.frag COPYONLY)
