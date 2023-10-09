#include "gl.h"
#include "asset.h"

static VGFX_AS_ShaderProgramHandle s_gl_bound_shader;

// =============================================
//
//
// Buffers & Rendering Pipeline
//
//
// =============================================

VGFX_GL_Buffer 
vgfx_gl_buffer_create(u32 type) {

  VGFX_GL_Buffer buff;
  glGenBuffers(1, &buff.handle);

  buff.size = 0;
  buff.type = type;

  return buff;
}

void 
vgfx_gl_buffer_delete(VGFX_GL_Buffer *buff) {

  VGFX_ASSERT_NON_NULL(buff);

  glDeleteBuffers(1, &buff->handle);

  buff->handle = VGFX_GL_INVALID_HANDLE;
  buff->size   = 0;
}

void 
vgfx_gl_buffer_data(VGFX_GL_Buffer *buff, u32 usage, usize size, void *data) {

  VGFX_ASSERT_NON_NULL(buff);

  buff->size = size;

  glBindBuffer(buff->type, buff->handle);
  glBufferData(buff->type, size, data, usage);
  glBindBuffer(buff->type, VGFX_GL_INVALID_HANDLE);
}

void 
vgfx_gl_buffer_sub_data(VGFX_GL_Buffer *buff, usize offset, usize size, void *data) {
  
  VGFX_ASSERT_NON_NULL(buff);

  glBindBuffer(buff->type, buff->handle);
  glBufferSubData(buff->type, offset, size, data);
  glBindBuffer(buff->type, VGFX_GL_INVALID_HANDLE);
}

VGFX_GL_VertexArray 
vgfx_gl_vertex_array_create() {
  
  VGFX_GL_VertexArray va;
  glGenVertexArrays(1, &va.handle);

  va._cached_id = 0;

  return va;
}

void 
vgfx_gl_vertex_array_delete(VGFX_GL_VertexArray *va) {
  
  VGFX_ASSERT_NON_NULL(va);

  glDeleteVertexArrays(1, &va->handle);

  va->handle = VGFX_GL_INVALID_HANDLE;
}

void 
vgfx_gl_vertex_array_layout(VGFX_GL_VertexArray *va, VGFX_GL_VertexAttribLayout* layout) {
  
  VGFX_ASSERT_NON_NULL(va);
  VGFX_ASSERT_NON_NULL(layout);
  VGFX_ASSERT(layout->buffer.handle, "Buffer handle is invalid.");

  // Calculate stride
  usize stride = 0;
  usize format = 0;
  usize tsize  = 0;

  for (usize i = 0; i < VGFX_GL_MAX_ATTRIBUTES; ++i) {
    VGFX_GL_VertexAttrib *attrib = &layout->attribs[i];

    if (!attrib->size) {
      continue;
    }

    if (!tsize) {
      tsize = _vgfx_gl_get_format_size(attrib->format);
      format = attrib->format;
    }

    VGFX_ASSERT(format == attrib->format, 
                "Format missmatch in VertexLayout, "
                "expected `%lu`, but found `%u`.", 
                format, attrib->format);

    stride += tsize * attrib->size;
  }

  // Set vertex attributes
  glBindVertexArray(va->handle);
  glBindBuffer(layout->buffer.type, layout->buffer.handle);

  usize offset = 0;

  for (usize i = 0; i < VGFX_GL_MAX_ATTRIBUTES; ++i) {
    VGFX_GL_VertexAttrib *attrib = &layout->attribs[i];

    if (!attrib->size) {
      continue;
    }

    VGFX_ASSERT(
        va->_cached_id < VGFX_GL_MAX_ATTRIBUTES, 
        "Vertex Buffer has already bound maximum number of attributes.");

    // Set GL vertex attrib pointer
    glVertexAttribPointer(va->_cached_id, attrib->size, attrib->format, 
                          attrib->norm, stride, (void *)offset);
    glEnableVertexAttribArray(va->_cached_id);
    glVertexAttribDivisor(va->_cached_id, layout->update_freq);

    va->_cached_id += 1;

    offset += tsize * attrib->size;
  }

  glBindVertexArray(VGFX_GL_INVALID_HANDLE);
  glBindBuffer(layout->buffer.type, VGFX_GL_INVALID_HANDLE);
}

void 
vgfx_gl_vertex_array_index_buffer(VGFX_GL_VertexArray *va, VGFX_GL_Buffer *buff) {
  
  VGFX_ASSERT_NON_NULL(va);
  VGFX_ASSERT_NON_NULL(buff);
  VGFX_ASSERT(buff->handle, "Buffer handle is invalid.");
  VGFX_ASSERT(buff->type, 
              "Buffer type is wrong, expected `%u`, "
              "but found `%u`.", 
              GL_ELEMENT_ARRAY_BUFFER, buff->type);

  glBindVertexArray(va->handle);
  glBindBuffer(buff->type, buff->handle);

  glBindVertexArray(VGFX_GL_INVALID_HANDLE);
  glBindBuffer(buff->type, VGFX_GL_INVALID_HANDLE);
}

void 
vgfx_gl_vertex_array_reset(VGFX_GL_VertexArray *va) {

  VGFX_ASSERT_NON_NULL(va);

  va->_cached_id = 0;
}

usize 
_vgfx_gl_get_format_size(u32 format) {
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

void 
vgfx_gl_bind_texture_handle(VGFX_AS_TextureHandle handle, u32 slot) {
  
  VGFX_ASSERT(handle, "Invalid handle.");

  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, handle);
}

void 
vgfx_gl_unbind_texture_handle(u32 slot) {

  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, VGFX_GL_INVALID_HANDLE);
}

void 
vgfx_gl_bind_shader_program(VGFX_AS_ShaderProgramHandle handle) {
  
  VGFX_ASSERT(handle, "Invalid handle.");

  s_gl_bound_shader = handle;

  glUseProgram(handle);
}

void 
vgfx_gl_unbind_shader_program() {

  s_gl_bound_shader = VGFX_GL_INVALID_HANDLE;

  glUseProgram(VGFX_GL_INVALID_HANDLE);
}

// =============================================
//
//
// Uniforms
//
//
// =============================================

void 
vgfx_gl_uniform_fv(const char *name, usize count, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  switch (count) {
  case 1:
    glUniform1fv(location, count, v);
    break;
  case 2:
    glUniform2fv(location, count, v);
    break;
  case 3:
    glUniform3fv(location, count, v);
    break;
  case 4:
    glUniform4fv(location, count, v);
    break;
  default:
    VGFX_ABORT("Unsupported uniform count, `%lu`.", count);
    break;
  }
}

void 
vgfx_gl_uniform_iv(const char* name, usize count, const i32 *v) {
  
  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  switch (count) {
  case 1:
    glUniform1iv(location, count, v);
    break;
  case 2:
    glUniform2iv(location, count, v);
    break;
  case 3:
    glUniform3iv(location, count, v);
    break;
  case 4:
    glUniform4iv(location, count, v);
    break;
  default:
    VGFX_ABORT("Unsupported uniform count, `%lu`.", count);
    break;
  }
}

void 
vgfx_gl_uniform_uv(const char* name, usize count, const u32 *v) {
  
  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  switch (count) {
  case 1:
    glUniform1uiv(location, count, v);
    break;
  case 2:
    glUniform2uiv(location, count, v);
    break;
  case 3:
    glUniform3uiv(location, count, v);
    break;
  case 4:
    glUniform4uiv(location, count, v);
    break;
  default:
    VGFX_ABORT("Unsupported uniform count, `%lu`.", count);
    break;
  }
}

void 
vgfx_gl_uniform_mat2fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix2fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat3fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix3fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat4fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix4fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat2x3fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix2x3fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat3x2fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix3x2fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat2x4fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix2x4fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat4x2fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix4x2fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat3x4fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix3x4fv(location, count, trans, v);
}

void 
vgfx_gl_uniform_mat4x3fv(const char *name, usize count, bool trans, const f32 *v) {

  i32 location = glGetUniformLocation(s_gl_bound_shader, name);

  VGFX_GL_DEBUG_UNIFORM_WARNING(location, name);

  glUniformMatrix4x3fv(location, count, trans, v);
}

