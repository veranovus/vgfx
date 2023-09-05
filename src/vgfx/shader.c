#include "shader.h"

VGFX_Shader vgfx_shader_new(u32 type, const char **source) {
  VGFX_Shader shader = glCreateShader(type);

  glShaderSource(shader, 1, source, NULL);
  glCompileShader(shader);

  int success;
  char info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    String tmp;
    switch (type) {
    case GL_VERTEX_SHADER:
      tmp = std_string_from("vertex");
      break;
    case GL_FRAGMENT_SHADER:
      tmp = std_string_from("fragment");
      break;
    case GL_GEOMETRY_SHADER:
      tmp = std_string_from("geometry");
      break;
    default:
      break;
    }

    glGetShaderInfoLog(shader, 512, NULL, info_log);
    fprintf(stderr, "ERROR: Failed to compile %s shader:\n%s", tmp.ptr,
            info_log);

    std_string_free(&tmp);
    glDeleteShader(shader);

    return VGFX_INVALID_SHADER_ID;
  }

  return shader;
}

void vgfx_shader_free(VGFX_Shader shader) { glDeleteShader(shader); }

VGFX_ShaderProgram vgfx_shader_program_new(VGFX_Shader *vec, u32 len) {
  VGFX_ShaderProgram program = glCreateProgram();

  for (usize i = 0; i < len; ++i) {
    glAttachShader(program, vec[i]);
  }

  glLinkProgram(program);

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

static inline void
_vgfx_shader_program_uniform_location_check(i32 location, const char *name) {
  if (location == -1) {
    fprintf(stderr, "WARNING: Uniform at location `%s` doesn't exists.\n",
            name);
  }
}

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
