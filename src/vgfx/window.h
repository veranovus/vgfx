#pragma once

#include "common.h"

typedef struct VGFX_WindowDescriptor {
  const char *title;
  u32 width;
  u32 height;
  bool vsync;
  bool resizable;
} VGFX_WindowDescriptor;

typedef GLFWwindow VGFX_Window;

// VGFX_Window
// ===========

// Creates a new GLFW window and makes the context of that window current for
// OpenGL.
VGFX_Window *vgfx_window_new(VGFX_WindowDescriptor desc);

void vgfx_window_free(VGFX_Window *window);

// OpenGL context
// --------------

// Makes the context of the window current for OpenGL and sets the OpenGL
// viewport.
void vgfx_window_make_context_current(VGFX_Window *window, i32 w, i32 h);

// Returns whether context of the window is current for OpenGL or not.
bool vgfx_window_is_context_current(VGFX_Window *window);

// Helper functions
// ----------------

void vgfx_window_get_size(VGFX_Window *window, vec2 size);

void vgfx_window_set_size(VGFX_Window *window, const vec2 size);
