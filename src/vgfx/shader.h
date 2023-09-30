#pragma once

#include "core.h"

/*****************************************************************************
 * - Types
 * */

// VGFX ShaderProgram

#define VGFX_INVALID_SHADER_PROGRAM_ID 0

typedef u32 VGFX_ShaderProgram;

// VGFX Shader

#define VGFX_INVALID_SHADER_ID 0

typedef u32 VGFX_Shader;

/*****************************************************************************
 * - VGFX Shader
 * */

// Compiles a shader from given string.
VGFX_Shader vgfx_shader_new(u32 type, const char **source);

void vgfx_shader_free(VGFX_Shader shader);

/*****************************************************************************
 * - VGFX ShaderProgram
 * */

// Creates a shader program from given shaders, this function consumes the
// shaders making them unavailable for later use.
VGFX_ShaderProgram vgfx_shader_program_new(VGFX_Shader *vec, u32 len);

void vgfx_shader_program_free(VGFX_ShaderProgram program);

/*****************************************************************************
 * - Float Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_f1(VGFX_ShaderProgram program,
                                    const char *name, f32 v0);

void vgfx_shader_program_uniform_f2(VGFX_ShaderProgram program,
                                    const char *name, f32 v0, f32 v1);

void vgfx_shader_program_uniform_f3(VGFX_ShaderProgram program,
                                    const char *name, f32 v0, f32 v1, f32 v2);

void vgfx_shader_program_uniform_f4(VGFX_ShaderProgram program,
                                    const char *name, f32 v0, f32 v1, f32 v2,
                                    f32 v3);

/*****************************************************************************
 * - Integer Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_i1(VGFX_ShaderProgram program,
                                    const char *name, i32 v0);

void vgfx_shader_program_uniform_i2(VGFX_ShaderProgram program,
                                    const char *name, i32 v0, i32 v1);

void vgfx_shader_program_uniform_i3(VGFX_ShaderProgram program,
                                    const char *name, i32 v0, i32 v1, i32 v2);

void vgfx_shader_program_uniform_i4(VGFX_ShaderProgram program,
                                    const char *name, i32 v0, i32 v1, i32 v2,
                                    i32 v3);

/*****************************************************************************
 * - Unsigned Integer Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_ui1(VGFX_ShaderProgram program,
                                     const char *name, u32 v0);

void vgfx_shader_program_uniform_ui2(VGFX_ShaderProgram program,
                                     const char *name, u32 v0, u32 v1);

void vgfx_shader_program_uniform_ui3(VGFX_ShaderProgram program,
                                     const char *name, u32 v0, u32 v1, u32 v2);

void vgfx_shader_program_uniform_ui4(VGFX_ShaderProgram program,
                                     const char *name, u32 v0, u32 v1, u32 v2,
                                     u32 v3);

/*****************************************************************************
 * - Matrix Uniform Helper Functions
 * */

void vgfx_shader_program_uniform_mat2fv(VGFX_ShaderProgram program,
                                        const char *name, bool transpose,
                                        const f32 *val);

void vgfx_shader_program_uniform_mat3fv(VGFX_ShaderProgram program,
                                        const char *name, bool transpose,
                                        const f32 *val);

void vgfx_shader_program_uniform_mat4fv(VGFX_ShaderProgram program,
                                        const char *name, bool transpose,
                                        const f32 *val);

void vgfx_shader_program_uniform_mat2x3fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val);

void vgfx_shader_program_uniform_mat3x2fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val);

void vgfx_shader_program_uniform_mat2x4fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val);

void vgfx_shader_program_uniform_mat4x2fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val);

void vgfx_shader_program_uniform_mat3x4fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val);

void vgfx_shader_program_uniform_mat4x3fv(VGFX_ShaderProgram program,
                                          const char *name, bool transpose,
                                          const f32 *val);
