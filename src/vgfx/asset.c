#include "asset.h"

#include <pthread.h>
#include <stb/stb_image.h>

// =============================================
//
//
// Asset & AssetServer
//
//
// =============================================

struct VGFX_AS_AssetThread {
  pthread_t thread;
  VGFX_AS_AssetLoadState load_status;
  VGFX_AS_Asset *handle;
};

VGFX_AS_AssetServer *vgfx_as_asset_server_new() {

  VGFX_AS_AssetServer *as =
      (VGFX_AS_AssetServer *)malloc(sizeof(VGFX_AS_AssetServer));

  as->assets = vstd_map_new(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *),
                            vstd_map_condition_isize);

  for (usize i = VGFX_ASSET_TYPE_UNKNOWN + 1; i < VGFX_ASSET_TYPE_LAST; ++i) {
    vstd_map_set(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets, i,
                 (vstd_vector_new(VGFX_AS_Asset *)));
  }

  as->threads = vstd_vector_new(VGFX_AS_AssetThread);

  return as;
}

void vgfx_as_asset_server_free(VGFX_AS_AssetServer *as) {

  // If there are active threads wait for them to finish
  for (usize i = 0; i < as->threads.len; ++i) {
    VGFX_AS_AssetThread *at =
        &vstd_vector_get(VGFX_AS_AssetThread, as->threads, i);

    pthread_join(at->thread, NULL);
  }

  // Free threads vector
  vstd_vector_free(VGFX_AS_AssetThread, (&as->threads));

  // Free assets
  for (usize i = 0; i < as->assets.keys.len; ++i) {
    VSTD_Vector(VGFX_AS_Asset *) *vec =
        &vstd_vector_get(VSTD_Vector(VGFX_AS_Asset *), as->assets.vals, i);

    vstd_vector_iter(VGFX_AS_Asset *, (*vec), {
      i32 type = (*_$iter)->type;
      if (type < 0 || type >= VGFX_ASSET_TYPE_LAST) {
        type = VGFX_ASSET_TYPE_UNKNOWN;
      }

      switch (type) {
      case VGFX_ASSET_TYPE_UNKNOWN:
        VGFX_ABORT("Failed to free asset, asset type is `%s`.",
                   VSTD_STRINGIFY(VGFX_ASSET_TYPE_UNKNOWN));
        break;
      case VGFX_ASSET_TYPE_TEXTURE:
        _vgfx_as_texture_free((*_$iter)->handle);
        break;
      case VGFX_ASSET_TYPE_FONT:
        _vgfx_as_font_free((*_$iter)->handle);
        break;
      }

      free((*_$iter));
    });
  }

  // Free assets map
  vstd_map_free(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets);
}

VGFX_AS_Asset *vgfx_as_asset_server_load(VGFX_AS_AssetServer *as,
                                         VGFX_AS_AssetType type,
                                         const char *path) {
  _vgfx_as_validate_asset_path(path);

  // Create new asset handle
  VGFX_AS_Asset *handle = (VGFX_AS_Asset *)malloc(sizeof(VGFX_AS_Asset));
  *handle = (VGFX_AS_Asset){
      .type = type,
      .handle = NULL,
  };

  VSTD_Vector(VGFX_AS_Asset *) * vec;
  vstd_map_get(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets,
               type, vec);

  VGFX_ASSERT(vec, "AssetServer for this asset type is NULL.");

  vstd_vector_push(VGFX_AS_Asset *, vec, handle);

  // Prepare the asset loading func and descriptor
  if (type < 0 || type >= VGFX_ASSET_TYPE_LAST) {
    type = VGFX_ASSET_TYPE_UNKNOWN;
  }

  void *(*asset_fnc)(void *) = NULL;

  _VGFX_AS_AssetLoadDesc *asset_desc =
      (_VGFX_AS_AssetLoadDesc *)malloc(sizeof(_VGFX_AS_AssetLoadDesc));

  asset_desc->asset = handle;
  asset_desc->path = path;

  switch (type) {
  case VGFX_ASSET_TYPE_UNKNOWN:
    VGFX_ABORT("Failed to load asset, asset type is `%s`.",
               VSTD_STRINGIFY(VGFX_ASSET_TYPE_UNKNOWN));
    break;
  case VGFX_ASSET_TYPE_TEXTURE:
    asset_fnc = _vgfx_as_load_texture;

    asset_desc->texture_filter = GL_LINEAR;
    asset_desc->texture_wrap = GL_REPEAT;
    break;
  case VGFX_ASSET_TYPE_FONT:
    asset_fnc = _vgfx_as_load_font;

    asset_desc->font_size = 64;
    break;
  }

  // Prepare the asset loading thread
  vstd_vector_push(VGFX_AS_AssetThread, (&as->threads),
                   ((VGFX_AS_AssetThread){.handle = handle}));

  VGFX_AS_AssetThread *at =
      &vstd_vector_get(VGFX_AS_AssetThread, as->threads, (as->threads.len - 1));

  at->load_status = VGFX_AS_ASSET_LOAD_STATE_PENDING;

  // Bind load status to load desc
  asset_desc->load_status = &at->load_status;

  // Create the thread and send load desc
  VGFX_ASSERT(!pthread_create(&at->thread, NULL, asset_fnc, asset_desc),
              "Failed to create thread to load Asset.");

  return handle;
}

VGFX_AS_AssetLoadState vgfx_as_asset_server_load_status(VGFX_AS_AssetServer *as,
                                                        VGFX_AS_Asset *asset) {
  VGFX_AS_AssetLoadState status;

  bool remove_asset = false;
  isize remove_index = -1;

  vstd_vector_iter(VGFX_AS_AssetThread, as->threads, {
    if (_$iter->handle != asset) {
      continue;
    }

    switch (_$iter->load_status) {
    case VGFX_AS_ASSET_LOAD_STATE_LOADED:
      remove_index = _$i;
      break;
    case VGFX_AS_ASSET_LOAD_STATE_ERROR:
      remove_index = _$i;
      remove_asset = true;
      break;
    }

    status = _$iter->load_status;
    break;
  });

  if (remove_index != -1) {
    // Remove the completed thread
    vstd_vector_remove(VGFX_AS_AssetThread, (&as->threads), remove_index);

    // If asset failed to load remove it too
    if (remove_asset) {
      VSTD_Vector(VGFX_AS_Asset *) *vec = NULL;
      vstd_map_get(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets,
                   asset->type, vec);

      VGFX_ASSERT(vec, "AssetServer for this asset type is NULL.");

      // Find the asset's position
      isize remove_index = -1;
      vstd_vector_iter(VGFX_AS_Asset *, (*vec), {
        if ((*_$iter) != asset) {
          continue;
        }

        remove_index = _$i;
        break;
      });

      VGFX_ASSERT(remove_index != -1, "Asset has already been deleted.");

      vstd_vector_remove(VGFX_AS_Asset *, vec, remove_index);

      free(asset);
    }
  }

  return status;
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

void _vgfx_as_texture_free(VGFX_AS_Texture *handle) {

  glDeleteTextures(1, &handle->handle);

  free(handle);
}

void _vgfx_as_font_free(VGFX_AS_Font *handle) {

  VGFX_UNUSED(handle);

  VGFX_ABORT("Unimplemented.");
}

// =============================================
//
//
// Asset Loading
//
//
// =============================================

void *_vgfx_as_load_texture(void *ptr) {

  _VGFX_AS_AssetLoadDesc *desc = (_VGFX_AS_AssetLoadDesc *)ptr;

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
  u8 *data = stbi_load(desc->path, &width, &height, &channel, 0);

  VGFX_ASSERT(data, "Failed to load texture from, `%s`.", desc->path);

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
               desc->path);
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

  // Set handle of Asset
  desc->asset->handle = handle;

  // Set load status
  *(desc->load_status) = VGFX_AS_ASSET_LOAD_STATE_LOADED;

  VGFX_DEBUG_PRINT("Asset loaded successfully.\n");

  return NULL;
}

void *_vgfx_as_load_font(void *ptr) {

  _VGFX_AS_AssetLoadDesc *desc = (_VGFX_AS_AssetLoadDesc *)ptr;

  VGFX_UNUSED(desc);

  VGFX_ABORT("Unimplemented.");
}
