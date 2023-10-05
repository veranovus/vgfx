#pragma once

#include "core.h"
#include "asset.h"

// =============================================
//
//
// Buffers
//
//
// =============================================

#define VGFX_GL_INVALID_HANDLE 0

#define VGFX_GL_MAX_ATTRIBUTES 16

typedef struct VGFX_GL_Buffer VGFX_GL_Buffer;
struct VGFX_GL_Buffer {
  u32 handle;
  u32 type;
  usize size;
};

typedef struct VGFX_GL_VertexAttrib VGFX_GL_VertexAttrib;
struct VGFX_GL_VertexAttrib {
  u32 size;
  u32 format;
  u32 norm;
};

typedef struct VGFX_GL_VertexAttribLayout VGFX_GL_VertexAttribLayout;
struct VGFX_GL_VertexAttribLayout {
  VGFX_GL_Buffer buffer;
  VGFX_GL_VertexAttrib attribs[VGFX_GL_MAX_ATTRIBUTES];
  usize update_freq;
};

typedef struct VGFX_GL_VertexArray VGFX_GL_VertexArray;
struct VGFX_GL_VertexArray {
  u32 handle;
  u32 _cached_id;
};

VGFX_GL_Buffer vgfx_gl_buffer_create(u32 type);

void vgfx_gl_buffer_delete(VGFX_GL_Buffer *buff);

void vgfx_gl_buffer_data(VGFX_GL_Buffer *buff, u32 usage, usize size, void *data);

void vgfx_gl_buffer_sub_data(VGFX_GL_Buffer *buff, usize offset, usize size, void *data);

VGFX_GL_VertexArray vgfx_gl_vertex_array_create();

void vgfx_gl_vertex_array_delete(VGFX_GL_VertexArray *va);

void vgfx_gl_vertex_array_layout(VGFX_GL_VertexArray *va, VGFX_GL_VertexAttribLayout *layout);

void vgfx_gl_vertex_array_index_buffer(VGFX_GL_VertexArray *va, VGFX_GL_Buffer *buff);

void vgfx_gl_vertex_array_reset(VGFX_GL_VertexArray *va);

usize _vgfx_gl_get_format_size(u32 format);

// =============================================
//
//
// Asset Handles
//
//
// =============================================

void vgfx_gl_bind_texture_handle(VGFX_AS_Asset *as, u32 slot);

void vgfx_gl_unbind_texture_handle(u32 slot);

void vgfx_gl_bind_shader_program(VGFX_AS_Asset *as);

void vgfx_gl_unbind_shader_program();
