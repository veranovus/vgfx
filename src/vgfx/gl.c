#include "gl.h"

// =============================================
//
//
// Buffers & Rendering Pipeline
//
//
// =============================================

VGFX_GL_Buffer vgfx_gl_buffer_create(VGFX_GL_BufferDesc *desc) {

  VGFX_ASSERT_NON_NULL(desc);

  u32 handle;
  glGenBuffers(1, &handle);
  glBindBuffer(desc->type, handle);
  glBufferData(desc->type, desc->size, desc->data, desc->usage);
  glBindBuffer(desc->type, 0);

  return (VGFX_GL_Buffer){
      .handle = handle,
      .type = desc->type,
      .size = desc->size,
  };
}

void vgfx_gl_buffer_delete(VGFX_GL_Buffer *buff) {

  glDeleteBuffers(1, &buff->handle);

  buff->handle = VGFX_GL_INVALID_HANDLE;
}

void vgfx_gl_buffer_data(VGFX_GL_Buffer *buff, usize size, void *data,
                         u32 usage) {

  glBindBuffer(buff->type, buff->handle);
  glBufferData(buff->type, size, data, usage);
  glBindBuffer(buff->type, 0);

  buff->size = size;
}

void vgfx_gl_buffer_sub_data(VGFX_GL_Buffer *buff, usize offset, usize size,
                             void *data) {

  glBindBuffer(buff->type, buff->handle);
  glBufferSubData(buff->type, offset, size, data);
  glBindBuffer(buff->type, 0);
}

VGFX_GL_VertexArray vgfx_gl_vertex_array_create(VGFX_GL_VertexArrayDesc *desc) {

  VGFX_ASSERT_NON_NULL(desc);

  VGFX_GL_VertexArray va;
  glGenVertexArrays(1, &va.handle);

  // Set layouts
  for (usize i = 0; i < VGFX_GL_MAX_BUFFER; ++i) {
    VGFX_GL_VertexLayout *layout = &desc->layouts[i];

    if (!layout->buff.handle) {
      continue;
    }

    vgfx_gl_vertex_array_layout(&va, layout);
  }

  // Set index buffer
  if (desc->index_buffer.handle) {
    VGFX_ASSERT(desc->index_buffer.type == GL_ELEMENT_ARRAY_BUFFER,
                "Index buffer's type is wrong.");

    glBindVertexArray(va.handle);
    glBindBuffer(desc->index_buffer.type, desc->index_buffer.handle);

    glBindVertexArray(0);
    glBindBuffer(desc->index_buffer.type, 0);
  }

  return va;
}

void vgfx_gl_vertex_array_delete(VGFX_GL_VertexArray *va) {

  glDeleteVertexArrays(1, &va->handle);

  va->handle = VGFX_GL_INVALID_HANDLE;
}

void vgfx_gl_vertex_array_layout(VGFX_GL_VertexArray *va,
                                 VGFX_GL_VertexLayout *layout) {

  VGFX_ASSERT_NON_NULL(va);
  VGFX_ASSERT_NON_NULL(layout);
  VGFX_ASSERT(layout->buff.handle, "VertexLayout doesn't have a buffer.");

  // Calculate stride
  u32 format = 0;
  usize tsize = 0;
  usize stride = 0;

  for (usize i = 0; i < VGFX_GL_MAX_ATTRIB; ++i) {
    VGFX_GL_VertexAttrib *attrib = &layout->attribs[i];

    if (!attrib->format) {
      continue;
    }

    if (!tsize) {
      tsize = _vgfx_gl_get_format_size(attrib->format);
      format = attrib->format;
    }

    VGFX_ASSERT(format == attrib->format,
                "Attribute format doesn't match with the layout format, "
                "expected `%u` but found `%u`.",
                format, attrib->format);

    stride += tsize * attrib->size;
  }

  // Set vertex attributes
  glBindVertexArray(va->handle);
  glBindBuffer(layout->buff.type, layout->buff.handle);

  usize offset = 0;

  for (usize i = 0; i < VGFX_GL_MAX_ATTRIB; ++i) {
    VGFX_GL_VertexAttrib *attrib = &layout->attribs[i];

    // Skip empty attributes
    if (!attrib->format) {
      continue;
    }

    // Set the attribute
    glVertexAttribPointer(i, attrib->size, attrib->format, attrib->normalize,
                          stride, (void *)offset);
    glEnableVertexAttribArray(i);
    glVertexAttribDivisor(i, layout->update);

    // Calculate the new offset
    offset += tsize * attrib->size;
  }

  glBindVertexArray(0);
  glBindBuffer(layout->buff.type, layout->buff.handle);
}

usize _vgfx_gl_get_format_size(u32 format) {
  switch (format) {
  case GL_BYTE:
    return sizeof(i8);
  case GL_UNSIGNED_BYTE:
    return sizeof(u8);
  case GL_SHORT:
    return sizeof(i16);
  case GL_UNSIGNED_SHORT:
    return sizeof(u16);
  case GL_INT:
    return sizeof(i32);
  case GL_UNSIGNED_INT:
    return sizeof(u32);
  case GL_HALF_FLOAT:
    return sizeof(GLhalf);
  case GL_FLOAT:
    return sizeof(f32);
  case GL_DOUBLE:
    return sizeof(f64);
  }

  VGFX_ABORT("Unsupported or unknown GL format, `%u`.", format);
}

// =============================================
//
//
// Asset Handles
//
//
// =============================================
