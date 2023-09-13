#pragma once

#include "common.h"

// VGFX_Font
// =========

typedef struct VGFX_Font {
  i32 test;
} VGFX_Font;

// VGFX_Font
// =========

VGFX_Font *vgfx_font_new(const char *path);

void vgfx_font_free(VGFX_Font *font);
