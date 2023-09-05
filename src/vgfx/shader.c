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

    return VGFX_INVALID_SHADER_ID;
  }

  return shader;
}

VGFX_ShaderProgram vgfx_shader_program_new(VGFX_Shader *vec, u32 len) {
  VGFX_ShaderProgram program = glCreateProgram();

  for (usize i = 0; i < len; ++i) {
    glAttachShader(program, vec[i]);
  }

  glLinkProgram(program);

  int success;
  char info_log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    fprintf(stderr, "ERROR: Failed to link shader program:\n%s", info_log);

    return VGFX_INVALID_SHADER_PROGRAM_ID;
  }

  return program;
}
