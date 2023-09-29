#include "render.h"

// Pipeline
// - Program       M
// - VAO           M
// - Index Buffer  ?
// - Stencil       ?
// - Depth         ?
// - Blend         ?
// - Cull Face     ?

/*****************************************************************************
 * - VGFX VertexArray
 * */

VGFX_VertexArray vgfx_vertex_array_new(VGFX_VertexArrayDesc *desc) {

  VGFX_VertexArray handle;
  glGenVertexArrays(1, &handle);

  if (!desc) {
    return handle;
  }

  // Set vertex layouts
  for (i32 i = 0; i < VGFX_MAX_BUFFER; ++i) {
    VGFX_VertexLayout *layout = &desc->layouts[i];

    vgfx_vertex_array_layout(handle, layout);
  }

  // Set index buffer
  if (desc->index_buffer) {
    vgfx_vertex_array_index_buffer(handle, desc->index_buffer);
  }

  return handle;
}

void vgfx_vertex_array_free(VGFX_VertexArray va) {

  glDeleteVertexArrays(1, &va);
}

void vgfx_vertex_array_layout(VGFX_VertexArray va, VGFX_VertexLayout *layout) {

  // Skip invalid layout
  if (!layout->buffer) {
    return;
  }

  // Calculate stride and type_size
  u32 format = 0;
  usize tsize = 0;
  usize stride = 0;

  for (i32 i = 0; i < VGFX_MAX_VERTEX_ATTRIB; ++i) {
    VGFX_VertexAttrib *attrib = &layout->attrib[i];

    // Skip the attribute if its empty
    if (!attrib->format) {
      continue;
    }

    if (!format) {
      tsize = vgfx_get_gl_format_size(attrib->format);
      format = attrib->format;
    }

    // Validate vertex attribute format
    if (format != attrib->format) {
      fprintf(
          stderr,
          "Format missmatch detected when setting the VGFX_VertexLayout.\n");
      fprintf(stderr, "Expected `%u`, but found `%u`.\n", format,
              attrib->format);
      abort();
    }

    // Calculate the stride
    stride += attrib->size * tsize;
  }

  // Set the vertex attributes
  glBindVertexArray(va);
  glBindBuffer(GL_ARRAY_BUFFER, layout->buffer);

  usize offset = 0;

  for (i32 i = 0; i < VGFX_MAX_VERTEX_ATTRIB; ++i) {
    VGFX_VertexAttrib *attrib = &layout->attrib[i];

    // Skip the attribute if its empty
    if (!attrib->format) {
      continue;
    }

    // Set the attribute and enable it
    glVertexAttribPointer(i, attrib->size, attrib->format, attrib->normalize,
                          stride, (void *)offset);
    glEnableVertexAttribArray(i);
    glVertexAttribDivisor(i, layout->update);

    // Calculate the new offset for next attribute
    offset += tsize * attrib->size;
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vgfx_vertex_array_index_buffer(VGFX_VertexArray va, VGFX_Buffer buff) {

  glBindVertexArray(va);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*****************************************************************************
 * - VGFX Buffer
 * */

VGFX_Buffer vgfx_buffer_new(VGFX_BufferDesc *desc) {

  VGFX_Buffer handle;
  glGenBuffers(1, &handle);

  vgfx_buffer_data(handle, desc->type, desc->size, desc->data, desc->usage);

  return handle;
}

void vgfx_buffer_free(VGFX_Buffer buff) { glDeleteBuffers(1, &buff); }

void vgfx_buffer_data(VGFX_Buffer buff, u32 type, usize size, const void *data,
                      u32 usage) {

  glBindBuffer(type, buff);
  glBufferData(type, size, data, usage);
  glBindBuffer(type, 0);
}

void vgfx_buffer_sub_data(VGFX_Buffer buff, u32 type, usize offset, usize size,
                          const void *data) {

  glBindBuffer(type, buff);
  glBufferSubData(type, offset, size, data);
  glBindBuffer(type, 0);
}

/*****************************************************************************
 * - Helper Functions
 * */

usize vgfx_get_gl_format_size(u32 type) {
  switch (type) {
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

  fprintf(stderr,
          "Encountered unsupported enum in vgfx_get_gl_type_size, `%u`\n",
          type);
  abort();
}
