#include "font.h"

#include <freetype/freetype.h>

/*****************************************************************************
 * - VGFX Font
 * */

VGFX_Font *vgfx_font_new(const char *path, usize size) {

  VSTD_String content = vstd_fs_read_file(path);
  if (!content.ptr) {
    fprintf(stderr, "Failed to read file to load Font, `%s`.\n", path);
    return NULL;
  }

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Freetype failed to initiazlize.\n");
    return NULL;
  }

  FT_Face face;
  if (FT_New_Face(ft, path, 0, &face)) {
    fprintf(stderr, "Failed to load font from, `%s`.\n", path);
    return NULL;
  }

  if (FT_Set_Pixel_Sizes(face, 0, size)) {
    fprintf(stderr, "Failed to set font size to `%lu` pixels.\n", size);
    return NULL;
  }

  // Load flags
  const i32 load_flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);

  // Create VGFX Font
  VGFX_Font *font = (VGFX_Font *)calloc(1, sizeof(VGFX_Font));

  // Calculate font size
  for (i32 i = 32; i < VGFX_FONT_GLYPH_COUNT; ++i) {
    if (FT_Load_Char(face, i, load_flags)) {
      fprintf(stderr, "Failed to load glyph for the character, `%d`.\n", i);
      abort();
    }

    font->size[0] += face->glyph->bitmap.width;
    if (font->size[1] < (i32)face->glyph->bitmap.rows) {
      font->size[1] = face->glyph->bitmap.rows;
    }
  }

  // Create VGFX TextureHandle
  font->handle = _vgfx_font_new_empty_gl_texture(font->size);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, font->handle);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load glyphs
  i32 x_offset = 0;
  for (i32 i = 32; i < VGFX_FONT_GLYPH_COUNT; ++i) {
    if (FT_Load_Char(face, i, load_flags)) {
      fprintf(stderr, "Failed to load glyph for the character, `%d`.\n", i);
      abort();
    }

    if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
      fprintf(stderr, "Failed to render glyph for the character, `%d`.\n", i);
      abort();
    }

    font->glyphs[i].advance[0] = face->glyph->advance.x >> 6;
    font->glyphs[i].advance[1] = face->glyph->advance.y >> 6;
    font->glyphs[i].size[0] = face->glyph->bitmap.width;
    font->glyphs[i].size[1] = face->glyph->bitmap.rows;
    font->glyphs[i].bearing[0] = face->glyph->bitmap_left;
    font->glyphs[i].bearing[1] = face->glyph->bitmap_top;
    font->glyphs[i].offset = (f32)x_offset / (f32)font->size[0];

    glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, 0, font->glyphs[i].size[0],
                    font->glyphs[i].size[1], GL_RED, GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer);

    x_offset += font->glyphs[i].size[0];
  }

  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Cleanup
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  return font;
}

void vgfx_font_free(VGFX_Font *font) {
  glDeleteTextures(1, &font->handle);

  free(font);
}

/*****************************************************************************
 * - Private Helper Functions
 * */

VGFX_TextureHandle _vgfx_font_new_empty_gl_texture(ivec2 size) {

  VGFX_TextureHandle handle;
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /* FIXME: Accept a filter parameter for pixelated fonts.
   */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size[0], size[1], 0, GL_RED,
               GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  return handle;
}
