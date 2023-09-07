#include "window.h"

#include "core.h"

// Static variables
// ================

static VGFX_Window *s_context_window = NULL;

// VGFX_Window
// ===========

VGFX_Window *vgfx_window_new(VGFX_WindowDescriptor desc) {

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
  };

  vgfx_window_make_context_current(window, (i32)desc.width, (i32)desc.height);

  glfwSwapInterval(desc.vsync);

  // Set GLFW callback functions
  glfwSetFramebufferSizeCallback(window->handle,
                                 _vgfx_framebuffer_size_callback);

  return window;
}

void vgfx_window_free(VGFX_Window *window) {
  if (vgfx_window_is_context_current(window)) {
    s_context_window = NULL;
  }

  glfwDestroyWindow(window->handle);

  free(window);
}

// OpenGL context
// --------------

void vgfx_window_make_context_current(VGFX_Window *window, u32 w, u32 h) {
  s_context_window = window;

  glfwMakeContextCurrent(window->handle);

  glViewport(0, 0, (i32)w, (i32)h);

  // TODO: Reset glfwSwapInterval to VGFX_Window's vsync flag.
}

bool vgfx_window_is_context_current(const VGFX_Window *window) {
  return s_context_window == window;
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
