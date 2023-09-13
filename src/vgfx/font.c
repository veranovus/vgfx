#include "font.h"

#include <freetype/freetype.h>

VGFX_Font *vgfx_font_new(const char *path) {

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    fprintf(stderr, "ERROR: Failed to initialize FreeType.\n");
    return NULL;
  }

  return NULL;
}

void vgfx_font_free(VGFX_Font *font) { return; }
