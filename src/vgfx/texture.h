#pragma once

#include "common.h"

typedef struct VGFX_Texture {
  u32 handle;
  u32 width;
  u32 height;
  u32 channel_count;
} VGFX_Texture;

#define VGFX_TEXTURE_DEFAULT_WRAP GL_REPEAT
#define VGFX_TEXTURE_DEFAULT_FILTER GL_LINEAR

VGFX_Texture *vgfx_texture_new(const char *path, u32 wrap, u32 filter);

void vgfx_texture_free(VGFX_Texture *texture);
