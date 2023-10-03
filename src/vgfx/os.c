#include "os.h"

#include <glfw/glfw3.h>

static bool s_os_glfw_init = false;

static bool s_os_glad_proc = false;

static bool s_os_event_map_init = false;

static VSTD_Map(void *, VSTD_Vector(VGFX_OS_Event)) s_os_event_map;

// =============================================
//
//
// Window
//
//
// =============================================

VGFX_OS_WindowHandle vgfx_os_window_open(VGFX_OS_WindowDesc *desc) {

  if (!s_os_event_map_init) {
    s_os_event_map_init = true;

    s_os_event_map = vstd_map_new(void *, VSTD_Vector(VGFX_OS_Event),
                                  vstd_map_condition_void);
  }

  if (!s_os_glfw_init) {
    s_os_glfw_init = true;

    VGFX_ASSERT(glfwInit(), "Failed to initialize GLFW.");
  }

  // Create GLFW window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, desc->resizable);
  glfwWindowHint(GLFW_DECORATED, desc->decorated);
  glfwWindowHint(GLFW_VISIBLE, desc->visible);

  GLFWwindow *win =
      glfwCreateWindow(desc->width, desc->height, desc->title, NULL, NULL);
  VGFX_ASSERT(win, "Failed to create GLFW window.");

  vstd_map_set(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map, (void *)win,
               (vstd_vector_new(VGFX_OS_Event)));

  glfwMakeContextCurrent(win);

  // Get OpenGL function pointers
  if (!s_os_glad_proc) {
    s_os_glad_proc = true;

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
  }

  glfwSwapInterval(desc->vsync);

  // Set GLFW callbacks
  glfwSetWindowPosCallback(win, (void *)_vgfx_os_window_callback_pos);
  glfwSetWindowSizeCallback(win, (void *)_vgfx_os_window_callback_size);
  glfwSetWindowCloseCallback(win, (void *)_vgfx_os_window_callback_close);
  glfwSetCursorPosCallback(win, (void *)_vgfx_os_window_callback_cursor_pos);
  glfwSetMouseButtonCallback(win,
                             (void *)_vgfx_os_window_callback_mouse_button);
  glfwSetCharCallback(win, (void *)_vgfx_os_window_callback_char);
  glfwSetKeyCallback(win, (void *)_vgfx_os_window_callback_key);

  return (VGFX_OS_WindowHandle)win;
}

void vgfx_os_window_free(VGFX_OS_WindowHandle win) {

  VSTD_Vector(VGFX_OS_Event) *result = NULL;
  vstd_map_get(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map, (void *)win,
               result);

  VGFX_ASSERT(result,
              "Failed to get event list for window, invalid handle `%s`.", win);

  vstd_vector_free(VGFX_OS_Event, result);

  vstd_map_remove(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map,
                  (void *)win);
  if (s_os_event_map.keys.len == 0) {
    vstd_map_free(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map);
  }

  glfwDestroyWindow((GLFWwindow *)win);
}

void vgfx_os_window_swap_buffers(VGFX_OS_WindowHandle win) {

  glfwSwapBuffers((GLFWwindow *)win);
}

// =============================================
//
//
// Input & Events
//
//
// =============================================

void vgfx_os_poll_events() {

  vstd_map_iter(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map,
                { vstd_vector_clear(VSTD_Vector(VGFX_OS_Event), _$iter.val); });

  glfwPollEvents();
}

VSTD_Vector(VGFX_OS_Event) vgfx_os_events(VGFX_OS_WindowHandle win) {

  VSTD_Vector(VGFX_OS_Event) *result = NULL;
  vstd_map_get(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map, (void *)win,
               result);

  VGFX_ASSERT(result,
              "Failed to get event list for window, invalid handle `%s`.", win);

  return *result;
}

void _vgfx_os_push_event(void *win, VGFX_OS_Event *e) {

  VSTD_Vector(VGFX_OS_Event) *ev = NULL;
  vstd_map_get(void *, VSTD_Vector(VGFX_OS_Event), s_os_event_map, win, ev);

  VGFX_ASSERT(
      ev,
      "Failed to get event list for window to push event, invalid handle `%s`.",
      win);

  vstd_vector_push(VGFX_OS_Event, ev, *e);
}

void _vgfx_os_window_callback_pos(void *win, i32 x, i32 y) {

  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = VGFX_OS_EVENT_TYPE_WINDOW_POS,
                               .window_posx = x,
                               .window_posy = y,
                           });
}

void _vgfx_os_window_callback_size(void *win, i32 w, i32 h) {

  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = VGFX_OS_EVENT_TYPE_WINDOW_SIZE,
                               .window_posx = w,
                               .window_posy = h,
                           });
}

void _vgfx_os_window_callback_framebuffer_size(void *win, i32 w, i32 h) {

  _vgfx_os_push_event(win,
                      &(VGFX_OS_Event){
                          .type = VGFX_OS_EVENT_TYPE_WINDOW_FRAMEBUFFER_SIZE,
                          .window_posx = w,
                          .window_posy = h,
                      });
}

void _vgfx_os_window_callback_close(void *win) {

  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = VGFX_OS_EVENT_TYPE_WINDOW_CLOSE,
                           });
}

void _vgfx_os_window_callback_cursor_pos(void *win, f64 x, f64 y) {

  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = VGFX_OS_EVENT_TYPE_CURSOR_POS,
                               .cursor_x = (f32)x,
                               .cursor_y = (f32)y,
                           });
}

void _vgfx_os_window_callback_mouse_button(void *win, i32 button, i32 state,
                                           i32 _m) {
  VGFX_UNUSED(_m);

  i32 type = (state == GLFW_RELEASE) ? VGFX_OS_EVENT_TYPE_MOUSE_BUTTON_RELEASE
                                     : VGFX_OS_EVENT_TYPE_MOUSE_BUTTON_PRESS;

  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = type,
                               .mouse_button = button,
                           });
}

void _vgfx_os_window_callback_char(void *win, u32 cp) {
  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = VGFX_OS_EVENT_TYPE_CHAR,
                               .char_codepoint = cp,
                           });
}

void _vgfx_os_window_callback_key(void *win, i32 key, i32 sc, i32 state,
                                  i32 _m) {
  VGFX_UNUSED(_m);

  // TODO: Calculate this with math instaed
  i32 type;
  switch (state) {
  case GLFW_RELEASE:
    type = VGFX_OS_EVENT_TYPE_KEY_RELEASE;
    break;
  case GLFW_PRESS:
    type = VGFX_OS_EVENT_TYPE_KEY_PRESS;
    break;
  case GLFW_REPEAT:
    type = VGFX_OS_EVENT_TYPE_KEY_REPEAT;
    break;
  }

  _vgfx_os_push_event(win, &(VGFX_OS_Event){
                               .type = type,
                               .key_code = key,
                               .key_scancode = sc,
                           });
}
