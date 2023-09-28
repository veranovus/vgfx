#pragma once

#include "common.h"
#include "shader.h"

/*****************************************************************************
 * - Types
 * */

// VGFX Texture2D

#define VGFX_TEXTURE_DEFAULT_WRAP GL_REPEAT

#define VGFX_TEXTURE_DEFAULT_FILTER GL_LINEAR

typedef u32 VGFX_TextureHandle;

typedef struct VGFX_Texture2D {
  VGFX_TextureHandle handle;
  u32 width;
  u32 height;
  u32 channel_count;
  u32 slot;
} VGFX_Texture2D;

/*****************************************************************************
 * - VGFX Texture2D
 * */

VGFX_Texture2D *vgfx_texture_new(const char *path, u32 wrap, u32 filter);

void vgfx_texture_free(VGFX_Texture2D *texture);

/*****************************************************************************
 * - VGFX TextureHandle
 * */

void vgfx_texture_handle_bind(VGFX_TextureHandle handle, u32 slot);

void vgfx_texture_handle_unbind(u32 slot);

/*****************************************************************************
 * - Inline Functions
 * */

// VGFX Texture2D

VSTD_INLINE void vgfx_texture_bind(VGFX_Texture2D *texture, u32 slot) {
  texture->slot = slot;
  vgfx_texture_handle_bind(texture->handle, texture->slot);
}

VSTD_INLINE void vgfx_texture_unbind(const VGFX_Texture2D *texture) {
  vgfx_texture_handle_unbind(texture->slot);
}
