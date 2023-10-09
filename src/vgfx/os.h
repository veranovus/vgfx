#pragma once

#include "core.h"

// =============================================
//
//
// Window
//
//
// =============================================

typedef struct VGFX_OS_WindowDesc VGFX_OS_WindowDesc;
struct VGFX_OS_WindowDesc {
  const char *title;
  u32        width;
  u32        height;
  bool       vsync;
  bool       resizable;
  bool       decorated;
  bool       visible;
};

typedef iptr VGFX_OS_WindowHandle;

VGFX_OS_WindowHandle 
vgfx_os_window_open(VGFX_OS_WindowDesc *desc);

void 
vgfx_os_window_free(VGFX_OS_WindowHandle win);

void 
vgfx_os_window_swap_buffers(VGFX_OS_WindowHandle win);

// =============================================
//
//
// Input & Events
//
//
// =============================================

typedef i32 VGFX_OS_EventType;
enum VGFX_OS_EventType {
  VGFX_OS_EVENT_TYPE_UNKNOWN,
  VGFX_OS_EVENT_TYPE_WINDOW_POS,
  VGFX_OS_EVENT_TYPE_WINDOW_SIZE,
  VGFX_OS_EVENT_TYPE_WINDOW_FRAMEBUFFER_SIZE,
  VGFX_OS_EVENT_TYPE_WINDOW_CLOSE,
  VGFX_OS_EVENT_TYPE_WINDOW_FOCUS,
  VGFX_OS_EVENT_TYPE_CURSOR_POS,
  VGFX_OS_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
  VGFX_OS_EVENT_TYPE_MOUSE_BUTTON_PRESS,
  VGFX_OS_EVENT_TYPE_CHAR,
  VGFX_OS_EVENT_TYPE_KEY_RELEASE,
  VGFX_OS_EVENT_TYPE_KEY_PRESS,
  VGFX_OS_EVENT_TYPE_KEY_REPEAT,
};

typedef struct VGFX_OS_Event VGFX_OS_Event;
struct VGFX_OS_Event {
  VGFX_OS_EventType type;
  union {
    // VGFX_OS_EVENT_TYPE_WINDOW_POS
    struct {
      i32           window_posx;
      i32           window_posy;
    };
    // VGFX_OS_EVENT_TYPE_WINDOW_SIZE
    struct {
      u32           window_width;
      u32           window_height;
    };
    // VGFX_OS_EVENT_TYPE_WINDOW_FRAMEBUFFER_SIZE
    struct {
      u32           framebuffer_width;
      u32           framebuffer_height;
    };
    // OS_EVENT_TYPE_CURSOR_POS
    struct {
      f32           cursor_x;
      f32           cursor_y;
    };
    // OS_EVENT_TYPE_MOUSE_BUTTON
    i32             mouse_button;
    // OS_EVENT_TYPE_TEXT
    u32             char_codepoint;
    // OS_EVENT_TYPE_KEY
    struct {
      i32           key_code;
      i32           key_scancode;
    };
  };
};

void 
vgfx_os_poll_events();

VSTD_Vector(VGFX_OS_Event) 
vgfx_os_events(VGFX_OS_WindowHandle win);

void 
_vgfx_os_push_event(void *win, VGFX_OS_Event *e);

void 
_vgfx_os_window_callback_pos(void *win, i32 x, i32 y);

void 
_vgfx_os_window_callback_size(void *win, i32 w, i32 h);

void 
_vgfx_os_window_callback_framebuffer_size(void *win, i32 w, i32 h);

void 
_vgfx_os_window_callback_close(void *win);

void 
_vgfx_os_window_callback_cursor_pos(void *win, f64 x, f64 y);

void 
_vgfx_os_window_callback_mouse_button(void *win, i32 button, i32 state, i32 _m);

void 
_vgfx_os_window_callback_char(void *win, u32 cp);

void 
_vgfx_os_window_callback_key(void *win, i32 key, i32 sc, i32 state, i32 _m);
