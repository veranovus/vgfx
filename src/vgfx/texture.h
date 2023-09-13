#pragma once

#include "common.h"
#include "shader.h"

#define VGFX_TEXTURE_DEFAULT_WRAP GL_REPEAT
#define VGFX_TEXTURE_DEFAULT_FILTER GL_LINEAR

// VGFX_Texture2D
// ==============

typedef u32 VGFX_TextureHandle;

typedef struct VGFX_Texture2D {
  VGFX_TextureHandle handle;
  u32 width;
  u32 height;
  u32 channel_count;
  u32 slot;
} VGFX_Texture2D;

// VGFX_Texture2D
// ==============

VGFX_Texture2D *vgfx_texture_new(const char *path, u32 wrap, u32 filter);

VGFX_Texture2D *vgfx_texture_new_empty(ivec2 size, u32 format, u32 wrap,
                                       u32 filter);

void vgfx_texture_free(VGFX_Texture2D *texture);

void vgfx_texture_bind(VGFX_Texture2D *texture, u32 slot);

void vgfx_texture_unbind(const VGFX_Texture2D *texture);
