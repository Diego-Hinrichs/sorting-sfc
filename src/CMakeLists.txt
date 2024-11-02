cmake_minimum_required(VERSION 3.24)
project(main LANGUAGES CXX)
include(FetchContent)
add_executable(${PROJECT_NAME})

# Agrega los archivos fuente al target
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    hilbert.cpp
    morton.cpp
    qtnode.cpp
    quad.cpp
    window.cpp
    nbody.cpp
)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS    OFF)
set(GLFW_BUILD_DOCS     OFF)
set(GLFW_BUILD_WAYLAND  OFF) # Do not include Wayland support in GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
    GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(glfw)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)

# Link libraries (actualizando para GLEW)
target_link_libraries(${PROJECT_NAME} PUBLIC
    OpenGL::GL
    GLEW
    glfw
    OpenMP::OpenMP_CXX
)

# Set C++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
