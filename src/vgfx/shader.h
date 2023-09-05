#pragma once

#include "common.h"

typedef u32 VGFX_Shader;
typedef u32 VGFX_ShaderProgram;

#define VGFX_INVALID_SHADER_ID 0
#define VGFX_INVALID_SHADER_PROGRAM_ID 0

VGFX_Shader vgfx_shader_new(u32 type, const char **source);

VGFX_ShaderProgram vgfx_shader_program_new(VGFX_Shader *vec, u32 len);
