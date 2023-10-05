#pragma once

#include "core.h"

// =============================================
//
//
// Buffers
//
//
// =============================================

#define VGFX_GL_MAX_BUFFER 8

#define VGFX_GL_MAX_ATTRIB 16

#define VGFX_GL_INVALID_HANDLE 0

typedef struct VGFX_GL_BufferDesc VGFX_GL_BufferDesc;
struct VGFX_GL_BufferDesc {
  u32 type;
  u32 usage;
  usize size;
  void *data;
};

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
  u32 normalize;
};

typedef struct VGFX_GL_VertexLayout VGFX_GL_VertexLayout;
struct VGFX_GL_VertexLayout {
  VGFX_GL_Buffer buff;
  usize update;
  VGFX_GL_VertexAttrib attribs[VGFX_GL_MAX_ATTRIB];
};

typedef struct VGFX_GL_VertexArrayDesc VGFX_GL_VertexArrayDesc;
struct VGFX_GL_VertexArrayDesc {
  VGFX_GL_Buffer index_buffer;
  VGFX_GL_VertexLayout layouts[VGFX_GL_MAX_BUFFER];
};

typedef struct VGFX_GL_VertexArray VGFX_GL_VertexArray;
struct VGFX_GL_VertexArray {
  u32 handle;
};

VGFX_GL_Buffer vgfx_gl_buffer_create(VGFX_GL_BufferDesc *desc);

void vgfx_gl_buffer_delete(VGFX_GL_Buffer *buff);

void vgfx_gl_buffer_data(VGFX_GL_Buffer *buff, usize size, void *data,
                         u32 usage);

void vgfx_gl_buffer_sub_data(VGFX_GL_Buffer *buff, usize offset, usize size,
                             void *data);

VGFX_GL_VertexArray vgfx_gl_vertex_array_create(VGFX_GL_VertexArrayDesc *desc);

void vgfx_gl_vertex_array_delete(VGFX_GL_VertexArray *va);

void vgfx_gl_vertex_array_layout(VGFX_GL_VertexArray *va,
                                 VGFX_GL_VertexLayout *layout);

usize _vgfx_gl_get_format_size(u32 format);

// =============================================
//
//
// Rendering Pipeline
//
//
// =============================================

// =============================================
//
//
// Asset Handles
//
//
// =============================================
