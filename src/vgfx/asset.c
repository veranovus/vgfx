#include "asset.h"
#include "gl.h"

#include <freetype/freetype.h>
#include <stb/stb_image.h>

// =============================================
//
//
// Asset & AssetServer
//
//
// =============================================

VGFX_AS_AssetServer *vgfx_as_asset_server_new() {

  VGFX_AS_AssetServer *as =
      (VGFX_AS_AssetServer *)malloc(sizeof(VGFX_AS_AssetServer));

  as->assets = vstd_map_new(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *),
                            vstd_map_condition_isize);

  for (usize i = VGFX_ASSET_TYPE_UNKNOWN + 1; i < VGFX_ASSET_TYPE_LAST; ++i) {
    vstd_map_set(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets, i,
                 (vstd_vector_new(VGFX_AS_Asset *)));
  }

  return as;
}

void vgfx_as_asset_server_free(VGFX_AS_AssetServer *as) {

  VGFX_ASSERT_NON_NULL(as);

  // Free assets
  for (usize i = 0; i < as->assets.keys.len; ++i) {
    VSTD_Vector(VGFX_AS_Asset *) *vec =
        &vstd_vector_get(VSTD_Vector(VGFX_AS_Asset *), as->assets.vals, i);

    vstd_vector_iter(VGFX_AS_Asset *, (*vec), {
      VGFX_AS_Asset *asset = (*_$iter);

      // Validate asset type
      i32 type = asset->type;
      if (type < 0 || type >= VGFX_ASSET_TYPE_LAST) {
        type = VGFX_ASSET_TYPE_UNKNOWN;
      }

      // Free asset handle
      switch (type) {
      case VGFX_ASSET_TYPE_UNKNOWN:
        VGFX_ABORT("Failed to free asset, unknown asset type `%d`.",
                   asset->type);
        break;
      case VGFX_ASSET_TYPE_TEXTURE:
        _vgfx_as_free_texture(asset->handle);
        break;
      case VGFX_ASSET_TYPE_FONT:
        _vgfx_as_free_font(asset->handle);
        break;
      case VGFX_ASSET_TYPE_SHADER:
        _vgfx_as_free_shader(asset->handle);
        break;
      }

      // Free asset
      free(asset);
    });
  }

  // Free assets map
  vstd_map_free(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets);
}

VGFX_AS_Asset *vgfx_as_asset_server_load(VGFX_AS_AssetServer *as,
                                         VGFX_AS_AssetDesc *desc) {
  VGFX_ASSERT_NON_NULL(as);
  VGFX_ASSERT_NON_NULL(desc);

  // Validate asset type
  VGFX_AS_AssetType type = desc->type;
  if (type < 0 || type >= VGFX_ASSET_TYPE_LAST) {
    type = VGFX_ASSET_TYPE_UNKNOWN;
  }

  // Create asset's hanlde
  void *handle = NULL;
  switch (type) {
  case VGFX_ASSET_TYPE_UNKNOWN:
    VGFX_ABORT("Failed to load asset, unknown asset type `%d`.", desc->type);
    break;
  case VGFX_ASSET_TYPE_TEXTURE:
    handle = _vgfx_as_load_texture(desc);
    break;
  case VGFX_ASSET_TYPE_FONT:
    handle = _vgfx_as_load_font(desc);
    break;
  case VGFX_ASSET_TYPE_SHADER:
    handle = _vgfx_as_load_shader(desc);
    break;
  default:
    VGFX_ABORT("Load function for this type is missing.");
    break;
  }

  // Create asset
  VGFX_AS_Asset *asset = (VGFX_AS_Asset *)malloc(sizeof(VGFX_AS_Asset));
  *asset = (VGFX_AS_Asset){
      .type = type,
      .handle = handle,
  };

  // Set asset handle
  VSTD_Vector(VGFX_AS_Asset *) *vec = NULL;
  vstd_map_get(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets,
               type, vec);

  VGFX_ASSERT(vec, "AssetServer for this asset type is NULL.");

  vstd_vector_push(VGFX_AS_Asset *, vec, asset);

  return asset;
}

void _vgfx_as_validate_asset_path(const char *path) {

  VSTD_String tmp = vstd_fs_read_file(path);

  VGFX_ASSERT(tmp.ptr, "Invalid asset path, `%s`.\n", path);

  vstd_string_free(&tmp);
}

// =============================================
//
//
// Asset Handles
//
//
// =============================================

void *_vgfx_as_load_texture(VGFX_AS_AssetDesc *desc) {

  VGFX_ASSERT_NON_NULL(desc);
  VGFX_ASSERT_NON_ZERO(desc->texture_wrap);
  VGFX_ASSERT_NON_ZERO(desc->texture_filter);

  // Validate asset paths
  _vgfx_as_validate_asset_path(desc->texture_path);

  // Create OpenGL texture
  VGFX_AS_TextureHandle th;
  glGenTextures(1, &th);
  glBindTexture(GL_TEXTURE_2D, th);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (i32)desc->texture_wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (i32)desc->texture_wrap);

  u32 min_filter = (desc->texture_filter == GL_LINEAR)
                       ? GL_LINEAR_MIPMAP_LINEAR
                       : GL_NEAREST_MIPMAP_NEAREST;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (i32)min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                  (i32)desc->texture_filter);

  // Load and validate texture data
  i32 width, height, channel;
  u8 *data = stbi_load(desc->texture_path, &width, &height, &channel, 0);

  VGFX_ASSERT(data, "Failed to load texture from, `%s`.", desc->texture_path);

  // Validate format
  u32 format;
  switch (channel) {
  case 1:
    format = GL_RED;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  default:
    VGFX_ABORT("Encountered unknown format when creating texture, `%d`.",
               desc->texture_path);
    break;
  }

  // Upload the texture to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  // Create texture handle
  VGFX_AS_Texture *handle = (VGFX_AS_Texture *)malloc(sizeof(VGFX_AS_Texture));
  *handle = (VGFX_AS_Texture){
      .handle = th,
      .size = {width, height},
      .channel = channel,
  };

  return handle;
}

void *_vgfx_as_load_font(VGFX_AS_AssetDesc *desc) {

  VGFX_ASSERT_NON_NULL(desc);
  VGFX_ASSERT_NON_ZERO(desc->font_size);
  VGFX_ASSERT_NON_ZERO(desc->font_filter);

  // Validate asset paths
  _vgfx_as_validate_asset_path(desc->font_path);

  // Setup Freetype
  FT_Library ft;
  VGFX_ASSERT(FT_Init_FreeType(&ft), "Freetype failed to initiazlize.");

  FT_Face face;
  VGFX_ASSERT(!FT_New_Face(ft, desc->font_path, 0, &face),
              "Failed to load font from, `%s`.", desc->font_path);

  VGFX_ASSERT(!FT_Set_Pixel_Sizes(face, 0, desc->font_size),
              "Failed to set font size to `%u` pixels.", desc->font_size);

  // Load flags
  const i32 load_flags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);

  // Create font handle
  VGFX_AS_Font *font = (VGFX_AS_Font *)calloc(1, sizeof(VGFX_AS_Font));

  font->range[0] = desc->font_range[0];
  font->range[1] = desc->font_range[1];

  // Calculate font size
  for (u32 i = font->range[0]; i < font->range[1]; ++i) {
    if (FT_Load_Char(face, i, load_flags)) {
      fprintf(stderr, "Failed to load glyph for the character, `%d`.\n", i);
      abort();
    }

    font->size[0] += face->glyph->bitmap.width;
    if (font->size[1] < (i32)face->glyph->bitmap.rows) {
      font->size[1] = face->glyph->bitmap.rows;
    }
  }

  // Create font's glyph vector
  i32 cap = font->range[1] - font->range[0];
  VGFX_ASSERT(cap > 0, "Invalid font range, upper bound has to be bigger.");

  font->glyphs = vstd_vector_with_capacity(_VGFX_AS_Glyph, cap);

  // Create texture handle
  glGenTextures(1, &font->handle);
  glBindTexture(GL_TEXTURE_2D, font->handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc->font_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc->font_filter);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->size[0], font->size[1], 0,
               GL_RED, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, font->handle);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load glyphs
  i32 x_offset = 0;
  for (i32 i = 0; i < cap; ++i) {
    _VGFX_AS_Glyph *glyph = &vstd_vector_get(_VGFX_AS_Glyph, font->glyphs, i);

    if (FT_Load_Char(face, i, load_flags)) {
      fprintf(stderr, "Failed to load glyph for the character, `%d`.\n", i);
      abort();
    }

    if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
      fprintf(stderr, "Failed to render glyph for the character, `%d`.\n", i);
      abort();
    }

    glyph->advn[0] = face->glyph->advance.x >> 6;
    glyph->advn[1] = face->glyph->advance.y >> 6;
    glyph->size[0] = face->glyph->bitmap.width;
    glyph->size[1] = face->glyph->bitmap.rows;
    glyph->brng[0] = face->glyph->bitmap_left;
    glyph->brng[1] = face->glyph->bitmap_top;
    glyph->offset = (f32)x_offset / (f32)font->size[0];

    glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, 0, glyph->size[0],
                    glyph->size[1], GL_RED, GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer);

    x_offset += glyph->size[0];
  }

  // Unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Cleanup
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  return font;
}

void *_vgfx_as_load_shader(VGFX_AS_AssetDesc *desc) {
  
  VGFX_ASSERT_NON_NULL(desc);

  // Validate asset paths
  _vgfx_as_validate_asset_path(desc->shader_vert_path);
  _vgfx_as_validate_asset_path(desc->shader_frag_path);

  // Compile shaders
  VSTD_String vert_source = vstd_fs_read_file(desc->shader_vert_path);
  VGFX_AS_ShaderHandle vs = _vgfx_as_compile_shader(
      GL_VERTEX_SHADER, (const char**)&vert_source.ptr);
  vstd_string_free(&vert_source);
  
  VSTD_String frag_source = vstd_fs_read_file(desc->shader_frag_path);
  VGFX_AS_ShaderHandle fs = _vgfx_as_compile_shader(
      GL_FRAGMENT_SHADER, (const char**)&frag_source.ptr);
  vstd_string_free(&frag_source);

  if (!(vs && fs)) {
    VGFX_ABORT("Shader failed to compile.");
  }

  // Link shaders
  VGFX_AS_ShaderHandle vec[2] = {vs, fs};
  VGFX_AS_ShaderProgramHandle sp = _vgfx_as_compile_shader_program(vec, 2);

  
  if (!(vs && fs)) {
    VGFX_ABORT("Shader Program failed to link.");
  }

  VGFX_AS_Shader *handle = (VGFX_AS_Shader*)malloc(sizeof(VGFX_AS_Shader));

  handle->handle = sp;

  return handle;
}

void _vgfx_as_free_texture(VGFX_AS_Texture *handle) {

  VGFX_ASSERT_NON_NULL(handle);

  glDeleteTextures(1, &handle->handle);

  free(handle);
}

void _vgfx_as_free_font(VGFX_AS_Font *handle) {

  VGFX_ASSERT_NON_NULL(handle);

  glDeleteTextures(1, &handle->handle);

  vstd_vector_free(_VGFX_AS_Glyph, (&handle->glyphs));

  free(handle);
}

void _vgfx_as_free_shader(VGFX_AS_Shader *handle) {
  
  VGFX_ASSERT_NON_NULL(handle);

  glDeleteProgram(handle->handle);

  free(handle);
}

u32 _vgfx_as_compile_shader(u32 type, const char** source) {

  VGFX_ASSERT_NON_NULL(source);

  VGFX_AS_ShaderHandle handle = glCreateShader(type);

  glShaderSource(handle, 1, source, NULL);
  glCompileShader(handle);

  int success;
  char info_log[512];
  glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

  if (!success) {
    VSTD_String tmp;
    switch (type) {
    case GL_VERTEX_SHADER:
      tmp = vstd_string_from(VSTD_STRINGIFY(GL_VERTEX_SHADER));
      break;
    // case GL_TESS_CONTROL_SHADER:
    //   tmp = vstd_string_from(VSTD_STRINGIFY(GL_TESS_CONTROL_SHADER));
    //   break;
    // case GL_TESS_EVALUATION_SHADER:
    //   tmp = vstd_string_from(VSTD_STRINGIFY(GL_TESS_EVALUATION_SHADER));
    //   break;
    case GL_GEOMETRY_SHADER:
      tmp = vstd_string_from(VSTD_STRINGIFY(GL_GEOMETRY_SHADER));
      break;
    case GL_FRAGMENT_SHADER:
      tmp = vstd_string_from(VSTD_STRINGIFY(GL_FRAGMENT_SHADER));
      break;
    // case GL_COMPUTE_SHADER:
    //   tmp = vstd_string_from(VSTD_STRINGIFY(GL_COMPUTE_SHADER));
    //   break;
    default:
      VGFX_ABORT("Failed to compile shader, unknown shader type `%d`.", type);
      break;
    }

    // TODO: Switch to VGFX_DEBUG_ERROR
    VGFX_DEBUG_WARN("Failed to compile `%s`:\n%s", tmp.ptr, info_log);

    vstd_string_free(&tmp);
    glDeleteShader(handle);

    return VGFX_GL_INVALID_HANDLE;
  }

  return handle;
}

u32 _vgfx_as_compile_shader_program(VGFX_AS_ShaderHandle *vec, usize len) {
  
  VGFX_AS_ShaderProgramHandle handle = glCreateProgram();

  for (usize i = 0; i < len; ++i) {
    glAttachShader(handle, vec[i]);
  }

  glLinkProgram(handle);

  for (usize i = 0; i < len; ++i) {
    glDeleteShader(vec[i]);
  }

  int success;
  char info_log[512];
  glGetProgramiv(handle, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(handle, 512, NULL, info_log);

    // TODO: Switch to VGFX_DEBUG_ERROR
    VGFX_DEBUG_WARN("Failed to link shader program:\n%s", info_log);

    glDeleteProgram(handle);

    return VGFX_GL_INVALID_HANDLE;
  }

  return handle;
}
