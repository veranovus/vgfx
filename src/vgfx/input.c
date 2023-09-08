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

void _vgfx_input_poll_keys(VGFX_Input *input, VGFX_Key key,
                           VGFX_KeyState state) {
  input->keys[key] = (VGFX_KeyInput){
      .key = key,
      .state = state,
  };
}

void _vgfx_input_poll_cursor_pos(VGFX_Input *input, f32 x, f32 y) {
  input->cursor.offset[0] = x - input->cursor.current[0];
  input->cursor.offset[1] = input->cursor.current[1] - y;

  input->cursor.current[0] = x;
  input->cursor.current[0] = y;
}
