#include "texture.h"

#include <stb/stb_image.h>

VGFX_Texture *vgfx_texture_new(const char *path, u32 wrap, u32 filter) {
  u32 handle;
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  u32 min_filter = (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR
                                         : GL_NEAREST_MIPMAP_NEAREST;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

  i32 width, height, channel;
  u8 *data = stbi_load(path, &width, &height, &channel, 0);

  if (!data) {
    fprintf(stderr, "ERROR: Failed to load texture from `%s`.\n", path);

    glDeleteTextures(1, &handle);

    return NULL;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  VGFX_Texture *texture = (VGFX_Texture *)malloc(sizeof(VGFX_Texture));
  *texture = (VGFX_Texture){
      .handle = handle,
      .width = width,
      .height = height,
      .channel_count = channel,
  };

  return texture;
}

void vgfx_texture_free(VGFX_Texture *texture) {
  glDeleteTextures(1, &texture->handle);

  free(texture);
}