# Project configuration
cmake_minimum_required(VERSION 3.26)
project(cgame VERSION 0.1.5 LANGUAGES C)

if (CMAKE_HOST_WIN32) 
    set(LIB_PATH dep/lib/win32-amd64)
elseif(CMAKE_HOST_LINUX)
    set(LIB_PATH dep/lib/linux-amd64)
elseif(CMAKE_HOST_APPLE)
    set(LIB_PATH dep/lib/macos-arm64)
else()
    message(FATAL_ERROR "Unsupported operating system or architecture.")
endif()

set(INC_PATH dep/include)

# Project settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
link_directories(${LIB_PATH})
include_directories(${INC_PATH})
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../${CMAKE_BUILD_TYPE})

# Libraries

# Dependency files

# Source files
set(SOURCE_FILES
    # src
    src/main.c

    # vgfx
    src/vgfx/vgfx.h
    src/vgfx/common.h
    src/vgfx/core.h
    src/vgfx/core.c
    src/vgfx/window.h
    src/vgfx/window.c
    src/vgfx/shader.h
    src/vgfx/shader.c
    src/vgfx/texture.h
    src/vgfx/texture.c

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
