#include "asset.h"
#include "vstd/vstd.h"

// =============================================
//
//
// Asset & AssetServer
//
//
// =============================================

struct VGFX_AS_AssetThread {
  pthread_t thread;
  i32 return_code;
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

    for (usize i = 0; i < vec->len; ++i) {
      VGFX_AS_Asset *a = vstd_vector_get(VGFX_AS_Asset *, (*vec), i);

      // FIXME: Call dedicated free function for each asset
      free(a);
    }
  }

  // Free assets map
  vstd_map_free(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets);
}

VGFX_AS_Asset *vgfx_as_asset_server_load(VGFX_AS_AssetServer *as,
                                         VGFX_AS_AssetType type,
                                         const char *path) {
  VGFX_UNUSED(path);

  if (type < 0 || type >= VGFX_ASSET_TYPE_LAST) {
    type = VGFX_ASSET_TYPE_UNKNOWN;
  }

  void *(*asset_fnc)(void *) = NULL;
  switch (type) {
  case VGFX_ASSET_TYPE_UNKNOWN:
    VGFX_ABORT("Failed to load asset, asset type `%s`.",
               VSTD_STRINGIFY(VGFX_ASSET_TYPE_UNKNOWN));
    break;
  case VGFX_ASSET_TYPE_TEXTURE:
    asset_fnc = _vgfx_as_load_texture;
    break;
  case VGFX_ASSET_TYPE_FONT:
    asset_fnc = _vgfx_as_load_font;
    break;
  }

  // Create new asset handle
  VGFX_AS_Asset *handle = (VGFX_AS_Asset *)malloc(sizeof(VGFX_AS_Asset));
  *handle = (VGFX_AS_Asset){
      .type = type,
      .handle = NULL,
  };

  VSTD_Vector(VGFX_AS_Asset *) * vec;
  vstd_map_get(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset *), as->assets,
               type, vec);

  VGFX_ASSERT(vec, "Asset Server is not properly initialized, this should've "
                   "never happened.");

  vstd_vector_push(VGFX_AS_Asset *, vec, handle);

  // Create asset loading thread
  vstd_vector_push(VGFX_AS_AssetThread, (&as->threads),
                   ((VGFX_AS_AssetThread){.handle = handle}));

  VGFX_AS_AssetThread *at =
      &vstd_vector_get(VGFX_AS_AssetThread, as->threads, (as->threads.len - 1));

  VGFX_ASSERT(!pthread_create(&at->thread, NULL, asset_fnc, &at->return_code),
              "Failed to create thread to load Asset.");

  return handle;
}

// =============================================
//
//
// Asset Loading
//
//
// =============================================

void *_vgfx_as_load_texture(void *ptr) {

  VGFX_UNUSED(ptr);

  VGFX_ABORT("Unimplemented.");
}

void *_vgfx_as_load_font(void *ptr) {

  VGFX_UNUSED(ptr);

  VGFX_ABORT("Unimplemented.");
}
