#pragma once

#include "core.h"

// =============================================
//
//
// Asset & AssetServer
//
//
// =============================================

#ifndef NDEBUG
#define VGFX_AS_ENABLE_DEBUG_CAST 1
#else
#define VGFX_AS_ENABLE_DEBUG_CAST 0
#endif

#ifndef VGFX_ASSET_CAST
#define VGFX_ASSET_CAST(asset, asset_type, var)                                \
  do {                                                                         \
    var = (typeof(var))asset->handle;                                          \
                                                                               \
    VGFX_ASSERT(asset->type == asset_type,                                     \
                "Failed to cast Asset handle to %d, found %s.", asset->type,   \
                #asset_type);                                                  \
  } while (0)
#endif

#if VGFX_AS_ENABLE_DEBUG_CAST
#define VGFX_ASSET_DEBUG_CAST VGFX_ASSET_CAST
#else
#define VGFX_ASSET_DEBUG_CAST(asset, asset_type, var)                          \
  do {                                                                         \
    var = (typeof(var))asset->handle;                                          \
  } while (0)
#endif

typedef i32 VGFX_AS_AssetType;
enum VGFX_AS_AssetType {
  VGFX_ASSET_TYPE_UNKNOWN,
  VGFX_ASSET_TYPE_TEXTURE,
  VGFX_ASSET_TYPE_FONT,
  VGFX_ASSET_TYPE_SHADER,
  VGFX_ASSET_TYPE_LAST,
};

typedef struct VGFX_AS_AssetDesc VGFX_AS_AssetDesc;
struct VGFX_AS_AssetDesc {
  VGFX_AS_AssetType type;
  union {
    // VGFX_ASSET_TYPE_TEXTURE
    struct {
      const char* texture_path;
      u32 texture_wrap;
      u32 texture_filter;
    };
    // VGFX_ASSET_TYPE_FONT
    struct {
      const char* font_path;
      u32 font_size;
      u32 font_filter;
      u32 font_range[2];
    };
    // VGFX_ASSET_TYPE_SHADER
    struct {
      const char *shader_vert_path;
      const char *shader_frag_path;
    };
  };
};

typedef struct VGFX_AS_Asset VGFX_AS_Asset;
struct VGFX_AS_Asset {
  i32 type;
  void *handle;
};

typedef struct VGFX_AS_AssetServer VGFX_AS_AssetServer;
struct VGFX_AS_AssetServer {
  VSTD_Map(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *)) assets;
};

VGFX_AS_AssetServer *vgfx_as_asset_server_new();

void vgfx_as_asset_server_free(VGFX_AS_AssetServer *as);

VGFX_AS_Asset *vgfx_as_asset_server_load(VGFX_AS_AssetServer *as,
                                         VGFX_AS_AssetDesc *desc);

void _vgfx_as_validate_asset_path(const char *path);

// =============================================
//
//
// Asset Handles
//
//
// =============================================

typedef u32 VGFX_AS_TextureHandle;

typedef struct VGFX_AS_Texture VGFX_AS_Texture;
struct VGFX_AS_Texture {
  VGFX_AS_TextureHandle handle;
  f32 size[2];
  u32 channel;
};

typedef struct _VGFX_AS_Glyph _VGFX_AS_Glyph;
struct _VGFX_AS_Glyph {
  f32 advn[2];
  f32 size[2];
  f32 brng[2];
  f32 offset;
};

typedef struct VGFX_AS_Font VGFX_AS_Font;
struct VGFX_AS_Font {
  VGFX_AS_TextureHandle handle;
  f32 size[2];
  u32 range[2];
  VSTD_Vector(_VGFX_AS_Glyph) glyphs;
  usize _average_glyph_height;
};

typedef u32 VGFX_AS_ShaderHandle;

typedef u32 VGFX_AS_ShaderProgramHandle;

typedef struct VGFX_AS_Shader VGFX_AS_Shader;
struct VGFX_AS_Shader {
  u32 handle;
};

void *_vgfx_as_load_texture(VGFX_AS_AssetDesc *desc);

void *_vgfx_as_load_font(VGFX_AS_AssetDesc *desc);

void *_vgfx_as_load_shader(VGFX_AS_AssetDesc *desc);

void _vgfx_as_free_texture(VGFX_AS_Texture *handle);

void _vgfx_as_free_font(VGFX_AS_Font *handle);

void _vgfx_as_free_shader(VGFX_AS_Shader *handle);

u32 _vgfx_as_compile_shader(u32 type, const char** path);

u32 _vgfx_as_compile_shader_program(VGFX_AS_ShaderHandle *vec, usize len);
