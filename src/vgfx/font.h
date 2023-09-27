#pragma once

#include "common.h"
#include "texture.h"

/*****************************************************************************
 * - Types
 * */

// VGFX Font

#define VGFX_FONT_GLYPH_COUNT 128

typedef struct VGFX_Glyph {
  vec2 advance;
  vec2 size;
  vec2 bearing;
  f32 offset;
} VGFX_Glyph;

typedef struct VGFX_Font {
  VGFX_TextureHandle handle;
  ivec2 size;
  VGFX_Glyph glyphs[VGFX_FONT_GLYPH_COUNT];
} VGFX_Font;

/*****************************************************************************
 * - VGFX Font
 * */

VGFX_Font *vgfx_font_new(const char *path);

void vgfx_font_free(VGFX_Font *font);

/*****************************************************************************
 * - Private Helper Functions
 * */

VGFX_TextureHandle _vgfx_font_new_empty_gl_texture(ivec2 size);
