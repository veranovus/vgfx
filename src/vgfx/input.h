#pragma once

#include "common.h"

// VGFX_WindowEventType_Key
// ========================

typedef enum VGFX_Key {
  VGFX_Key_UNKNOWN = -1,

  // Printable keys
  VGFX_Key_SPACE = 32,
  VGFX_Key_APOSTROPHE = 39, /* ' */
  VGFX_Key_COMMA = 44,      /* , */
  VGFX_Key_MINUS = 45,      /* - */
  VGFX_Key_PERIOD = 46,     /* . */
  VGFX_Key_SLASH = 47,      /* / */
  VGFX_Key_0 = 48,
  VGFX_Key_1 = 49,
  VGFX_Key_2 = 50,
  VGFX_Key_3 = 51,
  VGFX_Key_4 = 52,
  VGFX_Key_5 = 53,
  VGFX_Key_6 = 54,
  VGFX_Key_7 = 55,
  VGFX_Key_8 = 56,
  VGFX_Key_9 = 57,
  VGFX_Key_SEMICOLON = 59, /* ; */
  VGFX_Key_EQUAL = 61,     /* = */
  VGFX_Key_A = 65,
  VGFX_Key_B = 66,
  VGFX_Key_C = 67,
  VGFX_Key_D = 68,
  VGFX_Key_E = 69,
  VGFX_Key_F = 70,
  VGFX_Key_G = 71,
  VGFX_Key_H = 72,
  VGFX_Key_I = 73,
  VGFX_Key_J = 74,
  VGFX_Key_K = 75,
  VGFX_Key_L = 76,
  VGFX_Key_M = 77,
  VGFX_Key_N = 78,
  VGFX_Key_O = 79,
  VGFX_Key_P = 80,
  VGFX_Key_Q = 81,
  VGFX_Key_R = 82,
  VGFX_Key_S = 83,
  VGFX_Key_T = 84,
  VGFX_Key_U = 85,
  VGFX_Key_V = 86,
  VGFX_Key_W = 87,
  VGFX_Key_X = 88,
  VGFX_Key_Y = 89,
  VGFX_Key_Z = 90,
  VGFX_Key_LEFT_BRACKET = 91,  /* [ */
  VGFX_Key_BACKSLASH = 92,     /* \ */
  VGFX_Key_RIGHT_BRACKET = 93, /* ] */
  VGFX_Key_GRAVE_ACCENT = 96,  /* ` */
  VGFX_Key_WORLD_1 = 161,      /* non-US #1 */
  VGFX_Key_WORLD_2 = 162,      /* non-US #2 */

  // Function keys
  VGFX_Key_ESCAPE = 256,
  VGFX_Key_ENTER = 257,
  VGFX_Key_TAB = 258,
  VGFX_Key_BACKSPACE = 259,
  VGFX_Key_INSERT = 260,
  VGFX_Key_DELETE = 261,
  VGFX_Key_RIGHT = 262,
  VGFX_Key_LEFT = 263,
  VGFX_Key_DOWN = 264,
  VGFX_Key_UP = 265,
  VGFX_Key_PAGE_UP = 266,
  VGFX_Key_PAGE_DOWN = 267,
  VGFX_Key_HOME = 268,
  VGFX_Key_END = 269,
  VGFX_Key_CAPS_LOCK = 280,
  VGFX_Key_SCROLL_LOCK = 281,
  VGFX_Key_NUM_LOCK = 282,
  VGFX_Key_PRINT_SCREEN = 283,
  VGFX_Key_PAUSE = 284,
  VGFX_Key_F1 = 290,
  VGFX_Key_F2 = 291,
  VGFX_Key_F3 = 292,
  VGFX_Key_F4 = 293,
  VGFX_Key_F5 = 294,
  VGFX_Key_F6 = 295,
  VGFX_Key_F7 = 296,
  VGFX_Key_F8 = 297,
  VGFX_Key_F9 = 298,
  VGFX_Key_F10 = 299,
  VGFX_Key_F11 = 300,
  VGFX_Key_F12 = 301,
  VGFX_Key_F13 = 302,
  VGFX_Key_F14 = 303,
  VGFX_Key_F15 = 304,
  VGFX_Key_F16 = 305,
  VGFX_Key_F17 = 306,
  VGFX_Key_F18 = 307,
  VGFX_Key_F19 = 308,
  VGFX_Key_F20 = 309,
  VGFX_Key_F21 = 310,
  VGFX_Key_F22 = 311,
  VGFX_Key_F23 = 312,
  VGFX_Key_F24 = 313,
  VGFX_Key_F25 = 314,
  VGFX_Key_KP_0 = 320,
  VGFX_Key_KP_1 = 321,
  VGFX_Key_KP_2 = 322,
  VGFX_Key_KP_3 = 323,
  VGFX_Key_KP_4 = 324,
  VGFX_Key_KP_5 = 325,
  VGFX_Key_KP_6 = 326,
  VGFX_Key_KP_7 = 327,
  VGFX_Key_KP_8 = 328,
  VGFX_Key_KP_9 = 329,
  VGFX_Key_KP_DECIMAL = 330,
  VGFX_Key_KP_DIVIDE = 331,
  VGFX_Key_KP_MULTIPLY = 332,
  VGFX_Key_KP_SUBTRACT = 333,
  VGFX_Key_KP_ADD = 334,
  VGFX_Key_KP_ENTER = 335,
  VGFX_Key_KP_EQUAL = 336,
  VGFX_Key_LEFT_SHIFT = 340,
  VGFX_Key_LEFT_CONTROL = 341,
  VGFX_Key_LEFT_ALT = 342,
  VGFX_Key_LEFT_SUPER = 343,
  VGFX_Key_RIGHT_SHIFT = 344,
  VGFX_Key_RIGHT_CONTROL = 345,
  VGFX_Key_RIGHT_ALT = 346,
  VGFX_Key_RIGHT_SUPER = 347,
  VGFX_Key_MENU = 348,

  VGFX_Key_LAST = VGFX_Key_MENU,
} VGFX_Key;

typedef enum VGFX_KeyState {
  VGFX_KeyState_Invalid = -1,
  VGFX_KeyState_Release = 0,
  VGFX_KeyState_Press,
  VGFX_KeyState_Repeat,
} VGFX_KeyState;

// VGFX_WindowEventType_Button
// ===========================

typedef enum VGFX_ButtonState {
  VGFX_ButtonState_Invalid = -1,
  VGFX_ButtonState_Release = 0,
  VGFX_ButtonState_Press,
} VGFX_ButtonState;
