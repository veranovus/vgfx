#pragma once

#include "common.h"
#include "input.h"

typedef struct VGFX_WindowDescriptor {
  const char *title;
  u32 width;
  u32 height;
  bool vsync;
  bool resizable;
  bool decorated;
} VGFX_WindowDescriptor;

typedef GLFWwindow VGFX_WindowHandle;

typedef struct VGFX_Window {
  VGFX_WindowHandle *handle;
  u32 width;
  u32 height;
  bool vsync;
  VGFX_Input *input;
} VGFX_Window;

// VGFX_WindowHandle
// -----------------

// Returns a pointer to window that given handle is bound to. This function will
// panic if it fails to retrieve the window.
VGFX_Window *_vgfx_window_handle_get_instance(VGFX_WindowHandle *handle);

// VGFX_Window
// ===========

// Creates a new GLFW window and makes the context of that window current for
// OpenGL.
VGFX_Window *vgfx_window_new(VGFX_WindowDescriptor desc);

void vgfx_window_free(VGFX_Window *window);

void vgfx_window_swap_buffers(VGFX_Window *window);

void vgfx_window_poll_events(VGFX_Window *window);

// This function is called when VGFX_Core is terminated. Frees the whole windows
// and window related systems.
void _vgfx_window_terminate();

// OpenGL context
// --------------

// Makes the context of the window current for OpenGL and sets the OpenGL
// viewport.
void vgfx_window_make_context_current(VGFX_Window *window, u32 w, u32 h);

// Returns whether context of the window is current for OpenGL or not.
bool vgfx_window_is_context_current(const VGFX_Window *window);

// Helper functions
// ----------------

void vgfx_window_get_size(const VGFX_Window *window, ivec2 size);

void vgfx_window_set_size(VGFX_Window *window, const ivec2 size);

bool vgfx_window_get_window_close(const VGFX_Window *window);

void vgfx_window_set_window_close(VGFX_Window *window, bool close);

// GLFW callback functions
// -----------------------

void _vgfx_window_framebuffer_size_callback(VGFX_WindowHandle *handle, i32 w,
                                            i32 h);

void _vgfx_window_key_callback(VGFX_WindowHandle *handle, i32 key, i32 scancode,
                               i32 action, i32 mode);

void _vgfx_window_cursor_pos_callback(VGFX_WindowHandle *handle, f64 x, f64 y);
