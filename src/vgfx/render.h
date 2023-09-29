#pragma once

#include "common.h"
#include "shader.h"

/*****************************************************************************
 * - Types
 * */

// VGFX Buffer

typedef struct VGFX_BufferDesc {
  u32 type;
  u32 usage;
  usize size;
  void *data;
} VGFX_BufferDesc;

typedef u32 VGFX_Buffer;

// VGFX VertexArray

#define VGFX_MAX_VERTEX_ATTRIB 16

typedef struct VGFX_VertexAttrib {
  u32 size;
  u32 format;
  u32 normalize;
} VGFX_VertexAttrib;

typedef struct VGFX_VertexLayout {
  VGFX_Buffer buffer;
  VGFX_VertexAttrib attrib[VGFX_MAX_VERTEX_ATTRIB];
  usize update;
} VGFX_VertexLayout;

typedef u32 VGFX_VertexArray;

/*****************************************************************************
 * - VGFX Pipeline
 * */

VGFX_VertexArray vgfx_vertex_array_new();

void vgfx_vertex_array_free(VGFX_VertexArray va);

void vgfx_vertex_array_layout(VGFX_VertexArray va, VGFX_VertexLayout *layout);

void vgfx_vertex_array_index_buffer(VGFX_VertexArray va, VGFX_Buffer buff);

/*****************************************************************************
 * - VGFX Buffer
 * */

VGFX_Buffer vgfx_buffer_new(VGFX_BufferDesc *buffer_desc);

void vgfx_buffer_free(VGFX_Buffer buff);

/*****************************************************************************
 * - Helper Functions
 * */

usize vgfx_get_gl_format_size(u32 type);
