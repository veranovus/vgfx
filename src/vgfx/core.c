#include "core.h"

#include <stb/stb_image.h>

// VGFX_Core
// =========

void vgfx_initialize() {
  // Initialize GLFW
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");

    abort();
  }

  // Set flip_vertical_on_load for stb_image
  stbi_set_flip_vertically_on_load(true);
}

void vgfx_glew_initialize() {
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "ERROR: Failed to initialize GLEW.\n");

    glfwTerminate();

    abort();
  }
}

void vgfx_terminate() {

  // Terminate GLFW
  glfwTerminate();
}

// GLFW callback functions
// -----------------------

void _vgfx_framebuffer_size_callback(VGFX_WindowHandle *window, i32 w, i32 h) {
  glViewport(0, 0, w, h);
}
