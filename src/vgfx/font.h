#pragma once

#include "common.h"

/*****************************************************************************
 * - Types
 * */

typedef struct VGFX_Font {

} VGFX_Font;

/*****************************************************************************
 * - VGFX Font
 * */

VGFX_Font *vgfx_font_new(const char *path);

void vgfx_font_free(VGFX_Font *font);
