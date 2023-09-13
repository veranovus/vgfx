#pragma once

#include "common.h"
#include "input.h"

// VGFX_WindowEvent
// ================

typedef enum VGFX_WindowEventType {
  VGFX_WindowEventType_Key,
  VGFX_WindowEventType_Cursor,
  VGFX_WindowEventType_Mouse,
  VGFX_WindowEventType_WindowClose,
  VGFX_WindowEventType_WindowResize,
  VGFX_WindowEventType_WindowFramebufferResize,
} VGFX_WindowEventType;

typedef struct VGFX_WindowEvent {
  VGFX_WindowEventType type;
  // Key
  VGFX_Key key_id;
  VGFX_KeyState key_state;
  // Cursor
  vec2 cursor_pos;
  vec2 cursor_offset;
  // Mouse
  usize mouse_button;
  VGFX_ButtonState mouse_state;
  // WindowResize
  ivec2 window_size;
  // WindowFramebufferResize
  ivec2 window_framebuffer_size;
} VGFX_WindowEvent;

// VGFX_WindowHandle
// =================

typedef GLFWwindow VGFX_WindowHandle;

// VGFX_Window
// ===========

typedef struct VGFX_WindowDescriptor {
  const char *title;
  u32 width;
  u32 height;
  bool vsync;
  bool resizable;
  bool decorated;
} VGFX_WindowDescriptor;

typedef struct VGFX_Window {
  // Window
  VGFX_WindowHandle *handle;
  ivec2 framebuffer_size;
  ivec2 window_size;
  bool vsync;
  // WindowEvents
  vec2 _cursor_last;
  STD_Vector(VGFX_WindowEvent) _events;
} VGFX_Window;

// VGFX_WindowHandle
// =================

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

// Sets window's context as the current OpenGL context and sets the OpenGL
// viewport, this function applies the changes for window's framebuffer size.
void vgfx_window_make_context_current(VGFX_Window *window);

// Returns whether context of the window is current for OpenGL or not.
bool vgfx_window_is_context_current(const VGFX_Window *window);

// Helper functions
// ----------------

void vgfx_window_get_size(const VGFX_Window *window, ivec2 size);

void vgfx_window_set_size(VGFX_Window *window, const ivec2 size);

void vgfx_window_get_framebuffer_size(const VGFX_Window *window, ivec2 size);

// Sets the framebuffer size for the window, changes are only applied after
// window's context is set as current OpenGL context.
void vgfx_window_set_framebuffer_size(VGFX_Window *window, const ivec2 size);

bool vgfx_window_get_window_close(const VGFX_Window *window);

void vgfx_window_set_window_close(VGFX_Window *window, bool close);

// WindowEvent & Input functions
// -----------------------------

VGFX_KeyState vgfx_window_get_key(const VGFX_Window *window, VGFX_Key key);

VGFX_ButtonState vgfx_window_get_mouse_button(const VGFX_Window *window,
                                              usize button);

void vgfx_window_get_cursor(const VGFX_Window *window, vec2 position);

STD_Vector(VGFX_WindowEvent) vgfx_window_get_events(const VGFX_Window *window);

// GLFW callback functions
// -----------------------

void _vgfx_window_close_callback(VGFX_WindowHandle *handle);

void _vgfx_window_size_callback(VGFX_WindowHandle *handle, i32 w, i32 h);

void _vgfx_window_framebuffer_size_callback(VGFX_WindowHandle *handle, i32 w,
                                            i32 h);

void _vgfx_window_key_callback(VGFX_WindowHandle *handle, i32 key, i32 scancode,
                               i32 action, i32 mode);

void _vgfx_window_cursor_pos_callback(VGFX_WindowHandle *handle, f64 x, f64 y);

void _vgfx_window_mouse_button_callback(VGFX_WindowHandle *handle, i32 button,
                                        i32 action, i32 mods);
