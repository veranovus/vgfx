# Project configuration
cmake_minimum_required(VERSION 3.26)
project(vgfx VERSION 0.3.7 LANGUAGES C)

# Project settings
set(TARGET_NAME vgfx)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(vendor/)

# Dependency files
set(DEPENDENCY_FILES
        # stb_image
        vendor/stb/stb_image.h
        vendor/stb/stb_image.c

        # glfw
        vendor/glfw/glfw_impl.m

        # glad
        vendor/glad/glad.c
)

# Source files
set(SOURCE_FILES
        # src
        src/main.c

        # vgfx
        src/vgfx/core.h
        src/vgfx/core.c
        src/vgfx/os.h
        src/vgfx/os.c
        src/vgfx/gl.h
        src/vgfx/gl.c
        src/vgfx/asset.h
        src/vgfx/asset.c
        src/vgfx/input.h
        src/vgfx/render.h
        src/vgfx/render.c
        src/vgfx/camera.h
        src/vgfx/camera.c

        # vgfx/util
        # TODO: Enable it after refactoring
        # src/vgfx/util/camera_3d.h
        # src/vgfx/util/camera_3d.c
)

# Export values
add_compile_definitions(PKG_VERSION=\"${CMAKE_PROJECT_VERSION}\")

# Compilation directives
add_executable(${TARGET_NAME} ${SOURCE_FILES} ${DEPENDENCY_FILES})

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_library(COCOA Cocoa)
find_library(IOKIT IOKit)

target_link_libraries(${TARGET_NAME} PRIVATE ${COCOA} ${IOKIT} OpenGL::GL Freetype::Freetype)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pthread)

