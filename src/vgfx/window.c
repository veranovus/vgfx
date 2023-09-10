#include "window.h"

#include "core.h"

// Static variables
// ================

static VGFX_Window *s_vgfx_window_context_window = NULL;

static STD_Map(VGFX_WindowHandle *, VGFX_Window *) s_vgfx_window_windows;
static bool s_vgfx_window_initialized = false;

// VGFX_WindowHandle
// -----------------

// Returns a pointer to window that given handle is bound to.
VGFX_Window *_vgfx_window_handle_get_instance(VGFX_WindowHandle *handle) {
  VGFX_Window **window = NULL;
  std_map_get(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows, handle,
              window);

  if (!window) {
    fprintf(stderr, "ERROR: Failed to get window instance from handle.\n");
    abort();
  }

  return *window;
}

// VGFX_Window
// ===========

VGFX_Window *vgfx_window_new(VGFX_WindowDescriptor desc) {
  // Initialize the windows map if it's not initialized already
  if (!s_vgfx_window_initialized) {
    s_vgfx_window_windows =
        std_map_new(VGFX_WindowHandle *, VGFX_Window *, std_map_condition_void);

    s_vgfx_window_initialized = true;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, desc.resizable);
  glfwWindowHint(GLFW_DECORATED, desc.decorated);

  VGFX_WindowHandle *handle = glfwCreateWindow(
      (i32)desc.width, (i32)desc.height, desc.title, NULL, NULL);

  if (!handle) {
    fprintf(stderr, "ERROR: Failed to create new GLFW Window.\n");
    abort();
  }

  VGFX_Window *window = (VGFX_Window *)malloc(sizeof(VGFX_Window));
  *window = (VGFX_Window){
      .handle = handle,
      .width = desc.width,
      .height = desc.height,
      .vsync = desc.vsync,
      .input = _vgfx_input_new(),
  };

  // Make window context current
  s_vgfx_window_context_window = window;
  glfwMakeContextCurrent(window->handle);

  // Try to initialize GLEW
  _vgfx_glew_initialize();

  // Set GLViewport
  glViewport(0, 0, (i32)desc.width, (i32)desc.height);

  // Set VSYNC
  glfwSwapInterval(desc.vsync);

  // Add window to s_vgfx_window_windows map
  std_map_set(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows,
              window->handle, window);

  // Set GLFW callback functions
  glfwSetFramebufferSizeCallback(window->handle,
                                 _vgfx_window_framebuffer_size_callback);
  glfwSetKeyCallback(window->handle, _vgfx_window_key_callback);
  glfwSetCursorPosCallback(window->handle, _vgfx_window_cursor_pos_callback);

  return window;
}

void vgfx_window_free(VGFX_Window *window) {
  if (vgfx_window_is_context_current(window)) {
    s_vgfx_window_context_window = NULL;
  }

  // Remove self from windows map
  std_map_remove(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows,
                 window->handle);

  _vgfx_input_free(window->input);

  glfwDestroyWindow(window->handle);

  free(window);
}

void _vgfx_window_terminate() {
  s_vgfx_window_initialized = false;

  // Free all the remaining windows
  std_map_iter(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows,
               { vgfx_window_free(*_iter.val); });

  // Free the windows map
  std_map_free(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows);
}

void vgfx_window_swap_buffers(VGFX_Window *window) {
  glfwSwapBuffers(window->handle);
}

void vgfx_window_poll_events(VGFX_Window *window) {
  _vgfx_input_clear(window->input);
  glfwPollEvents();
}

// OpenGL context
// --------------

void vgfx_window_make_context_current(VGFX_Window *window, u32 w, u32 h) {
  s_vgfx_window_context_window = window;
  glfwMakeContextCurrent(window->handle);

  window->width = w;
  window->height = h;

  glViewport(0, 0, (i32)window->width, (i32)window->height);

  glfwSwapInterval(window->vsync);
}

bool vgfx_window_is_context_current(const VGFX_Window *window) {
  return s_vgfx_window_context_window == window;
}

// Helper functions
// ----------------

void vgfx_window_get_size(const VGFX_Window *window, ivec2 size) {
  size[0] = (i32)window->width;
  size[1] = (i32)window->height;
}

void vgfx_window_set_size(VGFX_Window *window, const ivec2 size) {
  window->width = size[0];
  window->height = size[1];

  glfwSetWindowSize(window->handle, (i32)window->width, (i32)window->height);
}

bool vgfx_window_get_window_close(const VGFX_Window *window) {
  return glfwWindowShouldClose(window->handle);
}

void vgfx_window_set_window_close(VGFX_Window *window, bool close) {
  glfwSetWindowShouldClose(window->handle, close);
}

// GLFW callback functions
// -----------------------

void _vgfx_window_framebuffer_size_callback(VGFX_WindowHandle *handle, i32 w,
                                            i32 h) {
  glViewport(0, 0, w, h);
}

void _vgfx_window_key_callback(VGFX_WindowHandle *handle, i32 key, i32 scancode,
                               i32 action, i32 mode) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);
  _vgfx_input_poll_keys(window->input, key, action);
}

void _vgfx_window_cursor_pos_callback(VGFX_WindowHandle *handle, f64 x, f64 y) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);
  _vgfx_input_poll_cursor_pos(window->input, (f32)x, (f32)y);
}
