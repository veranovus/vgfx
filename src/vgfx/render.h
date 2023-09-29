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

#define VGFX_MAX_BUFFER 8

typedef struct VGFX_VertexAttrib {
  u32 size;
  u32 format;
  u32 normalize;
} VGFX_VertexAttrib;

typedef struct VGFX_VertexLayout {
  VGFX_Buffer buffer;
  u32 update;
  VGFX_VertexAttrib attrib[VGFX_MAX_VERTEX_ATTRIB];
} VGFX_VertexLayout;

typedef struct VGFX_VertexArrayDesc {
  VGFX_VertexLayout layouts[VGFX_MAX_BUFFER];
  VGFX_Buffer index_buffer;
} VGFX_VertexArrayDesc;

typedef u32 VGFX_VertexArray;

/*****************************************************************************
 * - VGFX Pipeline
 * */

VGFX_VertexArray vgfx_vertex_array_new(VGFX_VertexArrayDesc *desc);

void vgfx_vertex_array_free(VGFX_VertexArray va);

void vgfx_vertex_array_layout(VGFX_VertexArray va, VGFX_VertexLayout *layout);

void vgfx_vertex_array_index_buffer(VGFX_VertexArray va, VGFX_Buffer buff);

/*****************************************************************************
 * - VGFX Buffer
 * */

VGFX_Buffer vgfx_buffer_new(VGFX_BufferDesc *buffer_desc);

void vgfx_buffer_free(VGFX_Buffer buff);

void vgfx_buffer_data(VGFX_Buffer buff, u32 type, usize size, const void *data,
                      u32 usage);

void vgfx_buffer_sub_data(VGFX_Buffer buff, u32 type, usize offset, usize size,
                          const void *data);

/*****************************************************************************
 * - Helper Functions
 * */

usize vgfx_get_gl_format_size(u32 type);
