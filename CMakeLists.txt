# Project configuration
cmake_minimum_required(VERSION 3.26)
project(cgame VERSION 0.2.4 LANGUAGES C)

if (CMAKE_HOST_WIN32)
    set(LIB_PATH dep/lib/win32-amd64)
elseif (CMAKE_HOST_LINUX)
    set(LIB_PATH dep/lib/linux-amd64)
elseif (CMAKE_HOST_APPLE)
    set(LIB_PATH dep/lib/macos-arm64)
else ()
    message(FATAL_ERROR "Unsupported operating system or architecture.")
endif ()

set(INC_PATH dep/include)

# Project settings
set(CMAKE_C_STANDARD 99)
link_directories(${LIB_PATH})
include_directories(${INC_PATH})

# Dependency files
set(DEPENDENCY_FILES
        # stb_image
        dep/include/stb/stb_image.h
        dep/include/stb/stb_image.c
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
        src/vgfx/input.c
        src/vgfx/window.h
        src/vgfx/window.c
        src/vgfx/camera.h
        src/vgfx/camera.c
        src/vgfx/shader.h
        src/vgfx/shader.c
        src/vgfx/texture.h
        src/vgfx/texture.c

        # vgfx/util
        src/vgfx/util/camera_3d.h
        src/vgfx/util/camera_3d.c

        # stb_image
        dep/include/stb/stb_image.h
        dep/include/stb/stb_image.c
)

# Export values
add_compile_definitions(DEBUG)
add_compile_definitions(PKG_VERSION=\"${CMAKE_PROJECT_VERSION}\")

# Compilation directives
add_executable(cgame ${SOURCE_FILES} ${DEPENDENCY_FILES})
find_package(OpenGL REQUIRED)
target_link_libraries(cgame OpenGL::GL glfw GLEW.2.2.0)
