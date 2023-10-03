#include "asset.h"

#include <pthread.h>

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

  as->assets = vstd_map_new(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset),
                            vstd_map_condition_isize);

  return as;
}

void vgfx_as_asset_server_free(VGFX_AS_AssetServer *as) {

  for (usize i = 0; i < as->assets.keys.len; ++i) {
    vstd_vector_free(
        VGFX_AS_Asset,
        (&vstd_vector_get(VSTD_Vector(VGFX_AS_Asset), as->assets.vals, i)));
  }

  vstd_map_free(VGFX_AS_AssetType, VSTD_Vector(VGFX_AS_Asset), as->assets);
}

VGFX_AS_Asset vgfx_as_asset_server_load(VGFX_AS_AssetServer *as,
                                        VGFX_AS_AssetType type,
                                        const char *path) {
  VGFX_UNUSED(as);
  VGFX_UNUSED(type);
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
    break;
  case VGFX_ASSET_TYPE_FONT:
    break;
  }

  VGFX_ABORT("Unimplemented!");
}
