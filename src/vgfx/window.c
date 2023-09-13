#include "window.h"

#include "core.h"

// Constant variables

const usize VGFX_WINDOW_EVENTS_INITIAL_CAP = 512;

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
      // Window
      .handle = handle,
      .window_size = {(i32)desc.width, (i32)desc.height},
      .vsync = desc.vsync,
      // WindowEvents
      ._events = std_vector_with_capacity(VGFX_WindowEvent,
                                          VGFX_WINDOW_EVENTS_INITIAL_CAP),
      ._cursor_last = {0.0f, 0.0f},
  };

  // Make window context current
  s_vgfx_window_context_window = window;
  glfwMakeContextCurrent(window->handle);

  // Try to initialize GLEW
  _vgfx_glew_initialize();

  // Set window's framebuffer size and GLViewport
  glfwGetFramebufferSize(window->handle, &window->framebuffer_size[0],
                         &window->framebuffer_size[1]);
  glViewport(0, 0, window->framebuffer_size[0], window->framebuffer_size[1]);

  // Set VSYNC
  glfwSwapInterval(desc.vsync);

  // Add window to s_vgfx_window_windows map
  std_map_set(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows,
              window->handle, window);

  // Set GLFW callback functions
  glfwSetWindowCloseCallback(window->handle, _vgfx_window_close_callback);
  glfwSetWindowSizeCallback(window->handle, _vgfx_window_size_callback);
  glfwSetFramebufferSizeCallback(window->handle,
                                 _vgfx_window_framebuffer_size_callback);
  glfwSetKeyCallback(window->handle, _vgfx_window_key_callback);
  glfwSetCursorPosCallback(window->handle, _vgfx_window_cursor_pos_callback);
  glfwSetMouseButtonCallback(window->handle,
                             _vgfx_window_mouse_button_callback);

  return window;
}

void vgfx_window_free(VGFX_Window *window) {
  if (vgfx_window_is_context_current(window)) {
    s_vgfx_window_context_window = NULL;
  }

  // Remove self from windows map
  std_map_remove(VGFX_WindowHandle *, VGFX_Window *, s_vgfx_window_windows,
                 window->handle);

  // Free WindowEvents
  std_vector_free(VGFX_WindowEvent, window->_events);

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
  // Clear window events
  std_vector_clear(VGFX_WindowEvent, window->_events);

  glfwPollEvents();
}

// OpenGL context
// --------------

void vgfx_window_make_context_current(VGFX_Window *window) {
  s_vgfx_window_context_window = window;
  glfwMakeContextCurrent(window->handle);

  glViewport(0, 0, (i32)window->framebuffer_size[0],
             (i32)window->framebuffer_size[1]);

  glfwSwapInterval(window->vsync);
}

bool vgfx_window_is_context_current(const VGFX_Window *window) {
  return s_vgfx_window_context_window == window;
}

// Helper functions
// ----------------

void vgfx_window_get_size(const VGFX_Window *window, ivec2 size) {
  size[0] = window->window_size[0];
  size[1] = window->window_size[1];
}

void vgfx_window_set_size(VGFX_Window *window, const ivec2 size) {
  window->window_size[0] = size[0];
  window->window_size[1] = size[1];

  glfwSetWindowSize(window->handle, window->window_size[0],
                    window->window_size[1]);
}

void vgfx_window_get_framebuffer_size(const VGFX_Window *window, ivec2 size) {
  size[0] = window->framebuffer_size[0];
  size[1] = window->framebuffer_size[1];
}

void vgfx_window_set_framebuffer_size(VGFX_Window *window, const ivec2 size) {
  window->framebuffer_size[0] = size[0];
  window->framebuffer_size[1] = size[1];
}

bool vgfx_window_get_window_close(const VGFX_Window *window) {
  return glfwWindowShouldClose(window->handle);
}

void vgfx_window_set_window_close(VGFX_Window *window, bool close) {
  glfwSetWindowShouldClose(window->handle, close);
}

// WindowEvent & Input functions
// -----------------------------

VGFX_KeyState vgfx_window_get_key(const VGFX_Window *window, VGFX_Key key) {
  return glfwGetKey(window->handle, key);
}

VGFX_ButtonState vgfx_window_get_mouse_button(const VGFX_Window *window,
                                              usize button) {
  return glfwGetMouseButton(window->handle, (i32)button);
}

void vgfx_window_get_cursor(const VGFX_Window *window, vec2 position) {
  f64 x, y;
  glfwGetCursorPos(window->handle, &x, &y);

  position[0] = (f32)x;
  position[1] = (f32)y;
}

STD_Vector(VGFX_WindowEvent) vgfx_window_get_events(const VGFX_Window *window) {
  return window->_events;
}

// GLFW callback functions
// -----------------------

void _vgfx_window_close_callback(VGFX_WindowHandle *handle) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);

  std_vector_push(VGFX_WindowEvent, window->_events,
                  ((VGFX_WindowEvent){
                      .type = VGFX_WindowEventType_WindowClose,
                  }));
}

void _vgfx_window_size_callback(VGFX_WindowHandle *handle, i32 w, i32 h) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);

  window->window_size[0] = w;
  window->window_size[1] = h;

  std_vector_push(
      VGFX_WindowEvent, window->_events,
      ((VGFX_WindowEvent){
          .type = VGFX_WindowEventType_WindowResize,
          .window_size = {window->window_size[0], window->window_size[1]},
      }));

  printf("WINDOW_SIZE : (%d, %d)\n", window->window_size[0],
         window->window_size[1]);
}

void _vgfx_window_framebuffer_size_callback(VGFX_WindowHandle *handle, i32 w,
                                            i32 h) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);

  window->framebuffer_size[0] = w;
  window->framebuffer_size[1] = h;

  glViewport(0, 0, window->framebuffer_size[0], window->framebuffer_size[1]);

  std_vector_push(VGFX_WindowEvent, window->_events,
                  ((VGFX_WindowEvent){
                      .type = VGFX_WindowEventType_WindowFramebufferResize,
                      .window_framebuffer_size = {window->framebuffer_size[0],
                                                  window->framebuffer_size[1]},
                  }));

  printf("WINDOW_FRAMEBUFFER_SIZE : (%d, %d)\n", window->framebuffer_size[0],
         window->framebuffer_size[1]);
}

void _vgfx_window_key_callback(VGFX_WindowHandle *handle, i32 key, i32 scancode,
                               i32 action, i32 mode) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);

  std_vector_push(VGFX_WindowEvent, window->_events,
                  ((VGFX_WindowEvent){
                      .type = VGFX_WindowEventType_Key,
                      .key_id = key,
                      .key_state = action,
                  }));
}

void _vgfx_window_cursor_pos_callback(VGFX_WindowHandle *handle, f64 x, f64 y) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);

  f32 x_offset = (f32)x - window->_cursor_last[0];
  f32 y_offset = window->_cursor_last[1] - (f32)y;

  window->_cursor_last[0] = (f32)x;
  window->_cursor_last[1] = (f32)y;

  std_vector_push(VGFX_WindowEvent, window->_events,
                  ((VGFX_WindowEvent){
                      .type = VGFX_WindowEventType_Cursor,
                      .cursor_pos = {x, y},
                      .cursor_offset = {x_offset, y_offset},
                  }));
}

void _vgfx_window_mouse_button_callback(VGFX_WindowHandle *handle, i32 button,
                                        i32 action, i32 mods) {
  VGFX_Window *window = _vgfx_window_handle_get_instance(handle);

  std_vector_push(VGFX_WindowEvent, window->_events,
                  ((VGFX_WindowEvent){
                      .type = VGFX_WindowEventType_Mouse,
                      .mouse_button = button,
                      .mouse_state = action,
                  }));
}
