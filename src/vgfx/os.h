#pragma once

#include "core.h"

// =============================================
//
//
// Window
//
//
// =============================================

typedef iptr VGFX_OS_WindowHandle;

void vgfx_os_window_new();

void vgfx_os_window_free();

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
  VGFX_OS_EVENT_TYPE_WINDOW_CLOSE,
  VGFX_OS_EVENT_TYPE_WINDOW_FOCUS,
  VGFX_OS_EVENT_TYPE_WINDOW_SIZE,
  VGFX_OS_EVENT_TYPE_WINDOW_FRAMEBUFFER_SIZE,
  VGFX_OS_EVENT_TYPE_KEY_PRESS,
  VGFX_OS_EVENT_TYPE_KEY_RELEASE,
  VGFX_OS_EVENT_TYPE_CURSOR_POS,
};

typedef struct VGFX_OS_Event VGFX_OS_Event;
struct VGFX_OS_Event {
  VGFX_OS_EventType type;
  VGFX_OS_WindowHandle window_handle;
  union {
    // VGFX_OS_EVENT_TYPE_WINDOW_SIZE
    struct {
      u32 window_width;
      u32 window_height;
    };
    // VGFX_OS_EVENT_TYPE_WINDOW_FRAMEBUFFER_SIZE
    struct {
      u32 framebuffer_width;
      u32 framebuffer_height;
    };
    // OS_EVENT_TYPE_KEY_PRESS & OS_EVENT_TYPE_KEY_RELEASE
    isize key;
    // OS_EVENT_TYPE_CURSOR_POS
    struct {
      f32 cursor_x;
      f32 cursor_y;
    };
  };
};

void vgfx_os_poll_events();
