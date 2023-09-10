#include "input.h"

// VGFX_Input
// ==========

VGFX_Input *_vgfx_input_new() {
  VGFX_Input *input = (VGFX_Input *)malloc(sizeof(VGFX_Input));
  return input;
}

void _vgfx_input_free(VGFX_Input *input) { free(input); }

// Helper functions
// ----------------

bool vgfx_input_is_key_down(const VGFX_Input *input, VGFX_Key key) {
  VGFX_KeyState state = input->keys[key].crnt_state;
  return state == VGFX_KeyState_Press || state == VGFX_KeyState_Repeat;
}

bool vgfx_input_is_key_up(const VGFX_Input *input, VGFX_Key key) {
  VGFX_KeyState state = input->keys[key].crnt_state;
  return state == VGFX_KeyState_Release;
}

bool vgfx_input_is_key_pressed(const VGFX_Input *input, VGFX_Key key) {
  const VGFX_KeyInput *k = &input->keys[key];

  if (k->prev_state == VGFX_KeyState_Release &&
      k->crnt_state == VGFX_KeyState_Press) {
    return true;
  }

  return false;
}

bool vgfx_input_is_key_released(const VGFX_Input *input, VGFX_Key key) {
  const VGFX_KeyInput *k = &input->keys[key];

  if (k->prev_state > VGFX_KeyState_Release &&
      k->crnt_state == VGFX_KeyState_Release) {
    return true;
  }

  return false;
}

const VGFX_CursorInput *vgfx_input_get_cursor(const VGFX_Input *input) {
  return &input->cursor;
}

// Input functions
// ----------------

void _vgfx_input_clear(VGFX_Input *input) {
  for (VGFX_KeyInput *k = input->keys; k < input->keys + VGFX_Key_LAST; ++k) {
    k->prev_state = k->crnt_state;
  }

  input->cursor.offset[0] = 0.0f;
  input->cursor.offset[1] = 0.0f;
  input->cursor.moved = false;
}

void _vgfx_input_poll_keys(VGFX_Input *input, VGFX_Key key,
                           VGFX_KeyState state) {
  VGFX_KeyInput *k = &input->keys[key];

  k->key = key;
  k->crnt_state = state;
}

void _vgfx_input_poll_cursor_pos(VGFX_Input *input, f32 x, f32 y) {
  VGFX_CursorInput *cursor = &input->cursor;

  cursor->offset[0] = x - cursor->current[0];
  cursor->offset[1] = cursor->current[1] - y;

  cursor->current[0] = x;
  cursor->current[1] = y;

  cursor->moved = true;
}
