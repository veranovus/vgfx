#include "shader.h"

/*****************************************************************************
 * - VGFX Shader
 * */

VGFX_Shader vgfx_shader_new(u32 type, const char **source) {
  VGFX_Shader shader = glCreateShader(type);

  glShaderSource(shader, 1, source, NULL);
  glCompileShader(shader);

  int success;
  char info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    VSTD_String tmp;
    switch (type) {
    case GL_VERTEX_SHADER:
      tmp = vstd_string_from(VSTD_STRINGIFY(GL_VERTEX_SHADER));
      break;
    // case GL_TESS_CONTROL_SHADER:
    //   tmp = vstd_string_from(VSTD_STRINGIFY(GL_TESS_CONTROL_SHADER));
    //   break;
    // case GL_TESS_EVALUATION_SHADER:
    //   tmp = vstd_string_from(VSTD_STRINGIFY(GL_TESS_EVALUATION_SHADER));
    //   break;
    case GL_GEOMETRY_SHADER:
      tmp = vstd_string_from(VSTD_STRINGIFY(GL_GEOMETRY_SHADER));
      break;
    case GL_FRAGMENT_SHADER:
      tmp = vstd_string_from(VSTD_STRINGIFY(GL_FRAGMENT_SHADER));
      break;
    // case GL_COMPUTE_SHADER:
    //   tmp = vstd_string_from(VSTD_STRINGIFY(GL_COMPUTE_SHADER));
    //   break;
    default:
      fprintf(stderr,
              "ERROR: Failed to compile shader unknown shader type `%d`.\n",
              type);
      abort();
      break;
    }

    glGetShaderInfoLog(shader, 512, NULL, info_log);
    fprintf(stderr, "ERROR: Failed to compile `%s`:\n%s", tmp.ptr, info_log);

    vstd_string_free(&tmp);
    glDeleteShader(shader);

    return VGFX_INVALID_SHADER_ID;
  }

  return shader;
}

void vgfx_shader_free(VGFX_Shader shader) { glDeleteShader(shader); }

/*****************************************************************************
 * - VGFX ShaderProgram
 * */

VGFX_ShaderProgram vgfx_shader_program_new(VGFX_Shader *vec, u32 len) {
  VGFX_ShaderProgram program = glCreateProgram();

  for (usize i = 0; i < len; ++i) {
    glAttachShader(program, vec[i]);
  }

  glLinkProgram(program);

  // Delete the shaders
  for (usize i = 0; i < len; ++i) {
    vgfx_shader_free(vec[i]);
  }

  int success;
  char info_log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    fprintf(stderr, "ERROR: Failed to link shader program:\n%s", info_log);

    glDeleteProgram(program);

    return VGFX_INVALID_SHADER_PROGRAM_ID;
  }

  return program;
}

void vgfx_shader_program_free(VGFX_ShaderProgram program) {
  glDeleteProgram(program);
}

/*****************************************************************************
 * - Private Helper Functions
 * */

VSTD_INLINE void _vgfx_shader_program_uniform_location_check(i32 location,
                                                             const char *name) {
  if (location == -1) {
    fprintf(stderr, "WARNING: Uniform at location `%s` doesn't exists.\n",
            name);
  }
}

/*****************************************************************************
 * - Float Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_f1(VGFX_ShaderProgram program,
                                    const char *name, f32 v0) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform1f(location, v0);
}

void vgfx_shader_program_uniform_f2(VGFX_ShaderProgram program,
                                    const char *name, f32 v0, f32 v1) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform2f(location, v0, v1);
}

void vgfx_shader_program_uniform_f3(VGFX_ShaderProgram program,
                                    const char *name, f32 v0, f32 v1, f32 v2) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform3f(location, v0, v1, v2);
}

void vgfx_shader_program_uniform_f4(VGFX_ShaderProgram program,
                                    const char *name, f32 v0, f32 v1, f32 v2,
                                    f32 v3) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform4f(location, v0, v1, v2, v3);
}

/*****************************************************************************
 * - Integer Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_i1(VGFX_ShaderProgram program,
                                    const char *name, i32 v0) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform1i(location, v0);
}

void vgfx_shader_program_uniform_i2(VGFX_ShaderProgram program,
                                    const char *name, i32 v0, i32 v1) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform2i(location, v0, v1);
}

void vgfx_shader_program_uniform_i3(VGFX_ShaderProgram program,
                                    const char *name, i32 v0, i32 v1, i32 v2) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform3i(location, v0, v1, v2);
}

void vgfx_shader_program_uniform_i4(VGFX_ShaderProgram program,
                                    const char *name, i32 v0, i32 v1, i32 v2,
                                    i32 v3) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform4i(location, v0, v1, v2, v3);
}

/*****************************************************************************
 * - Unsigned Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_ui1(VGFX_ShaderProgram program,
                                     const char *name, u32 v0) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform1ui(location, v0);
}

void vgfx_shader_program_uniform_ui2(VGFX_ShaderProgram program,
                                     const char *name, u32 v0, u32 v1) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform2ui(location, v0, v1);
}

void vgfx_shader_program_uniform_ui3(VGFX_ShaderProgram program,
                                     const char *name, u32 v0, u32 v1, u32 v2) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform3ui(location, v0, v1, v2);
}

void vgfx_shader_program_uniform_ui4(VGFX_ShaderProgram program,
                                     const char *name, u32 v0, u32 v1, u32 v2,
                                     u32 v3) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniform4ui(location, v0, v1, v2, v3);
}

/*****************************************************************************
 * - Matrix Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_mat2fv(VGFX_ShaderProgram program,
                                        const char *name, bool transpose,
                                        const f32 *val) {
  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix2fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat3fv(VGFX_ShaderProgram program,
                                        const char *name, bool transpose,
                                        const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix3fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat4fv(VGFX_ShaderProgram program,
                                        const char *name, bool transpose,
                                        const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix4fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat2x3fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix2x3fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat3x2fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix3x2fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat2x4fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix2x4fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat4x2fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix4x2fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat3x4fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix3x4fv(location, 1, transpose, val);
}

void vgfx_shader_program_uniform_mat4x3fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val) {

  i32 location = glGetUniformLocation(program, name);

#ifdef DEBUG
  _vgfx_shader_program_uniform_location_check(location, name);
#endif

  glUniformMatrix4x3fv(location, 1, transpose, val);
}
