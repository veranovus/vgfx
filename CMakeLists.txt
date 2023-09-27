# Project configuration
cmake_minimum_required(VERSION 3.26)
project(cgame VERSION 0.2.7 LANGUAGES C)

# Project settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(vendor/)

# Dependency files
set(DEPENDENCY_FILES
        # stb_image
        vendor/stb/stb_image.h
        vendor/stb/stb_image.c
)

# Source files
set(SOURCE_FILES
        # src
        src/main.c

        # vgfx
        src/vgfx/vgfx.h
        src/vgfx/common.h
        src/vgfx/core.h
        src/vgfx/core.c
        src/vgfx/input.h
        src/vgfx/window.h
        src/vgfx/window.c
        src/vgfx/camera.h
        src/vgfx/camera.c
        src/vgfx/shader.h
        src/vgfx/shader.c
        src/vgfx/texture.h
        src/vgfx/texture.c
        src/vgfx/font.h
        src/vgfx/font.c

        # vgfx/util
        src/vgfx/util/camera_3d.h
        src/vgfx/util/camera_3d.c
)

# Export values
add_compile_definitions(DEBUG)
add_compile_definitions(PKG_VERSION=\"${CMAKE_PROJECT_VERSION}\")

# Compilation directives
add_executable(cgame ${SOURCE_FILES} ${DEPENDENCY_FILES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(cgame OpenGL::GL glfw GLEW::glew)
