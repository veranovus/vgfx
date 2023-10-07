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

#define VGFX_RD_MAX_BOUND_TEXTURE 16

typedef struct VGFX_RD_Pipeline VGFX_RD_Pipeline;
struct VGFX_RD_Pipeline {
  VGFX_AS_Asset *shader;
  VGFX_GL_Buffer vb;
  VGFX_GL_VertexArray va;
  usize max_render_count;
  usize crn_render_count;
  VSTD_Vector(VGFX_RD_Vertex) cpu_vb;
  usize crn_texture;
  VGFX_AS_TextureHandle textures[VGFX_RD_MAX_BOUND_TEXTURE];
};

typedef struct VGFX_RD_Vertex VGFX_RD_Vertex;
struct VGFX_RD_Vertex {
  f32 shader;
  f32 pos[3];
  f32 col[4];
};

VGFX_RD_Pipeline *vgfx_rd_pipeline_new(VGFX_AS_AssetServer *as);

void vgfx_rd_piepline_free(VGFX_RD_Pipeline *pipeline);

void vgfx_rd_pipeline_begin(VGFX_RD_Pipeline *pipeline);

void vgfx_rd_pipeline_flush();

// =============================================
//
//
// Render
//
//
// =============================================

void vgfx_rd_send_vert(f32 shader, f32 pos[3], f32 col[4]);

void vgfx_rd_send_quad(f32 shader, f32 pos[3], f32 scl[2], f32 col[4]);
