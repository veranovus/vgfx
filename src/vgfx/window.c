#include "window.h"

#include "core.h"

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

  VGFX_Window *window =
      glfwCreateWindow(desc.width, desc.height, desc.title, NULL, NULL);

  vgfx_window_make_context_current(window, desc.width, desc.height);

  glfwSwapInterval(desc.vsync);

  // Set GLFW callback functions
  glfwSetFramebufferSizeCallback(window, _vgfx_framebuffer_size_callback);

  return window;
}

void vgfx_window_free(VGFX_Window *window) {
  if (vgfx_window_is_context_current(window)) {
    s_context_window = NULL;
  }

  glfwDestroyWindow(window);
}

// OpenGL context
// --------------

void vgfx_window_make_context_current(VGFX_Window *window, i32 w, i32 h) {
  s_context_window = window;

  glfwMakeContextCurrent(window);

  glViewport(0, 0, w, h);
}

bool vgfx_window_is_context_current(VGFX_Window *window) {
  if (s_context_window == window) {
    return true;
  }

  return false;
}

// Helper functions
// ----------------

void vgfx_window_get_size(VGFX_Window *window, vec2 size) {
  i32 w, h;
  glfwGetWindowSize(window, &w, &h);

  size[0] = (f32)w;
  size[1] = (f32)h;
}

void vgfx_window_set_size(VGFX_Window *window, const vec2 size) {
  glfwSetWindowSize(window, size[0], size[1]);
}
