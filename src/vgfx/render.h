#pragma once

#include "core.h"
#include "asset.h"
#include "gl.h"

// =============================================
//
//
// Pipeline
//
//
// =============================================

#define VGFX_RD_MAX_BOUND_TEXTURE  16

#define VGFX_RD_MAX_QUAD_COUNT     15000

#define VGFX_RD_MAX_INDEX_COUNT    (VGFX_RD_MAX_QUAD_COUNT * 6)

#define VGFX_RD_MAX_VERTEX_COUNT   (VGFX_RD_MAX_QUAD_COUNT * 4)

typedef struct VGFX_RD_Pipeline VGFX_RD_Pipeline;
struct VGFX_RD_Pipeline {
  struct {
    VGFX_AS_TextureHandle     texture;
    bool                      internal_flush;
  }                           _cache;
  VGFX_GL_Buffer              vb;
  VGFX_GL_Buffer              ib;
  VGFX_GL_VertexArray         va;
  usize                       max_vertex_count;
  usize                       crn_vertex_count;
  VSTD_Vector(VGFX_RD_Vertex) cpu_vb;
  usize                       crn_index_count;
  usize                       crn_texture;
  VGFX_AS_TextureHandle       textures[VGFX_RD_MAX_BOUND_TEXTURE];
};

typedef struct VGFX_RD_Vertex VGFX_RD_Vertex;
struct VGFX_RD_Vertex {
  f32 texture;
  f32 pos[3];
  f32 tex[2];
  f32 col[4];
};

VGFX_RD_Pipeline *
vgfx_rd_pipeline_new(VGFX_AS_AssetServer *as);

void 
vgfx_rd_piepline_free(VGFX_RD_Pipeline *pipeline);

void 
vgfx_rd_pipeline_begin(VGFX_RD_Pipeline *pipeline, VGFX_AS_Asset *shader);

void 
vgfx_rd_pipeline_flush();

// =============================================
//
//
// Render
//
//
// =============================================

#define VGFX_RD_NO_SUB_TEXTURE (vec4) {0.0f, 0.0f, 1.0f, 1.0f}

#ifndef VGFX_RD_SUB_TEXTURE
#define VGFX_RD_SUB_TEXTURE(w, h, i)                                           \
  (vec4) {                                                                     \
    (f32)((i32)i % (i32)w) * (1.0f / (f32)w),                                  \
    (f32)((i32)i / (i32)w) * (1.0f / (f32)h),                                  \
    1.0f / (f32)w,                                                             \
    1.0f / (f32)h,                                                             \
  }
#endif

void 
vgfx_rd_send_vert(f32 texture, vec3 pos, vec2 tex, vec4 col);

void 
vgfx_rd_send_quad(f32 texture, vec3 pos, vec2 scl, vec4 tex, vec4 col);

void 
vgfx_rd_send_texture(VGFX_AS_Texture *handle, vec3 pos, vec2 scl, vec4 tex, vec4 col);

void 
vgfx_rd_send_text(VGFX_AS_Font *handle, const char* str, vec3 pos, vec4 col);

vec2s 
vgfx_rd_font_render_size(VGFX_AS_Font *handle, const char *str, bool fh);
