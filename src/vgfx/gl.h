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

void vgfx_gl_bind_texture_handle(VGFX_AS_TextureHandle handle, u32 slot);

void vgfx_gl_unbind_texture_handle(u32 slot);

void vgfx_gl_bind_shader_program(VGFX_AS_ShaderProgramHandle handle);

void vgfx_gl_unbind_shader_program();

// =============================================
//
//
// Uniforms
//
//
// =============================================

#ifndef NDEBUG
#define VGFX_GL_ENABLE_DEBUG_UNIFORM_WARNING 1
#else
#define VGFX_GL_ENABLE_DEBUG_UNIFORM_WARNING 0
#endif

#if VGFX_GL_ENABLE_DEBUG_UNIFORM_WARNING
#define VGFX_GL_DEBUG_UNIFORM_WARNING(loc, name)                               \
  do {                                                                         \
    if (loc == -1)                                                             \
      VGFX_DEBUG_WARN("Invalid uniform location, `%s`.\n", name);              \
  } while (0)
#else
#define VGFX_GL_DEBUG_UNIFORM_WARNING(loc, name)                               \
  do {                                                                         \
    (void)(loc);                                                               \
    (void)(name);                                                              \
  } while (0)
#endif

void vgfx_gl_uniform_fv(const char* name, usize count, const f32 *v);

void vgfx_gl_uniform_iv(const char* name, usize count, const i32 *v);

void vgfx_gl_uniform_uv(const char* name, usize count, const u32 *v);

void vgfx_gl_uniform_mat2fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat3fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat4fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat2x3fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat3x2fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat2x4fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat4x2fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat3x4fv(const char *name, usize count, bool trans, const f32 *v);

void vgfx_gl_uniform_mat4x3fv(const char *name, usize count, bool trans, const f32 *v);
