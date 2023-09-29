#include "texture.h"

#include <stb/stb_image.h>

/*****************************************************************************
 * - VGFX Texture2D
 * */

VGFX_Texture2D *vgfx_texture_new(const char *path, u32 wrap, u32 filter) {

  // Validate texture path
  VSTD_String content = vstd_fs_read_file(path);
  if (!content.ptr) {
    fprintf(stderr, "Failed to read file to load texture, `%s`.\n", path);
    return NULL;
  }
  vstd_string_free(&content);

  // Create texture
  VGFX_TextureHandle handle;
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (i32)wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (i32)wrap);

  u32 min_filter = (filter == GL_LINEAR) ? GL_LINEAR_MIPMAP_LINEAR
                                         : GL_NEAREST_MIPMAP_NEAREST;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (i32)min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (i32)filter);

  // Load and validate texture data
  i32 width, height, channel;
  u8 *data = stbi_load(path, &width, &height, &channel, 0);

  if (!data) {
    fprintf(stderr, "ERROR: Failed to load texture from `%s`.\n", path);

    glDeleteTextures(1, &handle);

    return NULL;
  }

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
    fprintf(stderr, "Encountered unknown format when creating texture, `%d`.\n",
            channel);
    abort();
    break;
  }

  // Upload the texture to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  // Create VGFX Texture2D
  VGFX_Texture2D *texture = (VGFX_Texture2D *)malloc(sizeof(VGFX_Texture2D));
  *texture = (VGFX_Texture2D){
      .handle = handle,
      .width = width,
      .height = height,
      .channel_count = channel,
  };

  return texture;
}

void vgfx_texture_free(VGFX_Texture2D *texture) {
  glDeleteTextures(1, &texture->handle);

  free(texture);
}

/*****************************************************************************
 * - VGFX TextureHandle
 * */

void vgfx_texture_handle_bind(VGFX_TextureHandle handle, u32 slot) {
  glActiveTexture(GL_TEXTURE0 + slot);

  glBindTexture(GL_TEXTURE_2D, handle);
}

void vgfx_texture_handle_unbind(u32 slot) {
  glActiveTexture(GL_TEXTURE0 + slot);

  glBindTexture(GL_TEXTURE_2D, 0);
}
