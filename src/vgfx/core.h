#pragma once

#include "common.h"
#include "window.h"

// VGFX_Core
// =========

void vgfx_initialize();

void vgfx_glew_initialize();

void vgfx_terminate();

// GLFW callback functions
// -----------------------

void _vgfx_framebuffer_size_callback(VGFX_Window *window, i32 w, i32 h);
