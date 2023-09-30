#include "core.h"
#include "window.h"

#define B_STACKTRACE_IMPL
#include <b_stacktrace.h>

// =============================================
//
//
// Abort & Assertions
//
//
// =============================================

_Noreturn void _vgfx_abort(const char *file, i32 line, const char *msg, ...) {
  fprintf(stderr, "abort: ");

  va_list va_args;
  va_start(va_args, msg);
  vfprintf(stderr, msg, va_args);
  va_end(va_args);

  fprintf(stderr, "\nfile: %s:%u\n", file, line);
  char *stacktrace = b_stacktrace_get_string();
  fprintf(stderr, "stacktrace:\n%s\n\n", stacktrace);
  abort();
}

void _vgfx_assert_failed(const char *cond, const char *file, i32 line,
                         const char *msg, ...) {
  fprintf(stderr, "assertion failed: %s\nmessage: ", cond);

  va_list va_args;
  va_start(va_args, msg);
  vfprintf(stderr, msg, va_args);
  va_end(va_args);

  fprintf(stderr, "\nfile: %s:%u\n", file, line);
  char *stacktrace = b_stacktrace_get_string();
  fprintf(stderr, "stacktrace:\n%s\n\n", stacktrace);
  abort();
}

// =============================================
//
//
// Debug Print
//
//
// =============================================

void _vgfx_debug_print(const char *msg, ...) {
  printf("debug: ");

  va_list va_args;
  va_start(va_args, msg);
  vprintf(msg, va_args);
  va_end(va_args);
}

/*****************************************************************************
 * - Static Variables
 * */

static bool s_vgfx_core_glew_initialized = false;

/*****************************************************************************
 * - VGFX Core
 * */

void vgfx_initialize() {
  // Initialize GLFW
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: Failed to initialize GLFW.\n");

    abort();
  }

  // NOTE: Disabled due to UV changes.
  // Set flip_vertical_on_load for stb_image
  // stbi_set_flip_vertically_on_load(true);
}

void vgfx_terminate() {
  // Terminate VGFX_Window
  _vgfx_window_terminate();

  // Set GLEW initialized flag to false
  s_vgfx_core_glew_initialized = false;

  // Terminate GLFW
  glfwTerminate();
}

/*****************************************************************************
 * - OpenGL Helper Functions
 * */

// TODO: Make this function set the upper bound for `vgfx_texture_handle_bind`.
void _vgfx_dump_hardware_info() {
  // Context info
  const u8 *vend = glGetString(GL_VENDOR);
  const u8 *rend = glGetString(GL_RENDERER);

  printf("DEBUG: Context Info: %s :: %s\n", vend, rend);

  // Maximum vertex attribute count
  i32 max_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

  printf("DEBUG: Maximum available vertex attribute count: %d\n", max_attribs);

  // Texture image units per stage
  i32 image_units;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &image_units);

  printf("DEBUG: Available image units per shader stage: %d\n", image_units);

  // Total number of texture that can be bound
  i32 max_image_units;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_image_units);

  printf("DEBUG: Combined image units available: %d\n", max_image_units);
}

void _vgfx_glew_initialize() {
  if (s_vgfx_core_glew_initialized) {
    return;
  }

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Dump hardware info
#ifdef DEBUG
  _vgfx_dump_hardware_info();
#endif

  // Set s_vgfx_core_glew_initialized to true
  s_vgfx_core_glew_initialized = true;
}
