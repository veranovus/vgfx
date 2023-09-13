#include "core.h"

#include "window.h"

#include <stb/stb_image.h>

// Static variables
// ----------------

static bool s_vgfx_core_glew_initialized = false;

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

void vgfx_terminate() {
  // Terminate VGFX_Window
  _vgfx_window_terminate();

  // Set GLEW initialized flag to false
  s_vgfx_core_glew_initialized = false;

  // Terminate GLFW
  glfwTerminate();
}

// OpenGL helper functions
// -----------------------

void _vgfx_glew_initialize() {
  if (s_vgfx_core_glew_initialized) {
    return;
  }

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "ERROR: Failed to initialize GLEW.\n");

    glfwTerminate();

    abort();
  }

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set s_vgfx_core_glew_initialized to true
  s_vgfx_core_glew_initialized = true;
}
