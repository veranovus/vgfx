#include "render.h"

const char *VGFX_RD_BASE_VERT_PATH = "res/shader/default.vert";

const char *VGFX_RD_BASE_FRAG_PATH = "res/shader/default.frag";

const usize VGFX_RD_MAX_RENDER_COUNT = 10000;

static VGFX_RD_Pipeline *s_rd_bound_pipeline;

// =============================================
//
//
// Pipeline
//
//
// =============================================

VGFX_RD_Pipeline *vgfx_rd_pipeline_new(VGFX_AS_AssetServer *as) {

  VGFX_ASSERT_NON_NULL(as);

  VGFX_RD_Pipeline *pipeline = (VGFX_RD_Pipeline*) malloc(sizeof(VGFX_RD_Pipeline));

  pipeline->shader = vgfx_as_asset_server_load(as, &(VGFX_AS_AssetDesc) {
    .shader_vert_path = VGFX_RD_BASE_VERT_PATH,
    .shader_frag_path = VGFX_RD_BASE_FRAG_PATH,
  });

  pipeline->max_render_count = VGFX_RD_MAX_RENDER_COUNT * 6;
  pipeline->crn_render_count = 0;

  // OpenGL buffers
  pipeline->vb = vgfx_gl_buffer_create(GL_ARRAY_BUFFER);
  vgfx_gl_buffer_data(&pipeline->vb, GL_DYNAMIC_DRAW, 
                      sizeof(VGFX_RD_Vertex) * pipeline->max_render_count, NULL);

  pipeline->va = vgfx_gl_vertex_array_create();

  VGFX_GL_VertexAttribLayout layout = {
    .buffer = pipeline->vb,
    .update_freq = 0,
    .attribs = {
      {3, GL_FLOAT, GL_FALSE},
      {4, GL_FLOAT, GL_FALSE},
      {1, GL_FLOAT, GL_FALSE},
    },
  };
  vgfx_gl_vertex_array_layout(&pipeline->va, &layout);

  // CPU buffer
  pipeline->cpu_vb = vstd_vector_with_capacity(
        VGFX_RD_Vertex, pipeline->max_render_count);
  
  // Textures
  pipeline->crn_texture = 0;

  return pipeline;
}

void vgfx_rd_piepline_delete(VGFX_RD_Pipeline *pipeline) {

  VGFX_ASSERT_NON_NULL(pipeline);

  vgfx_gl_buffer_delete(&pipeline->vb);

  vgfx_gl_vertex_array_delete(&pipeline->va);

  vstd_vector_free(VGFX_RD_Vertex, (&pipeline->cpu_vb));

  free(pipeline);
}

void vgfx_rd_pipeline_begin(VGFX_RD_Pipeline *pipeline) {

  VGFX_ASSERT_NON_NULL(pipeline);

  pipeline->crn_render_count = 0;

  pipeline->crn_texture = 0;

  VGFX_AS_Shader *handle;
  VGFX_ASSET_CAST(pipeline->shader, VGFX_ASSET_TYPE_SHADER, handle);

  vgfx_gl_bind_shader_program(handle->handle);

  s_rd_bound_pipeline = pipeline;
}

void vgfx_rd_pipeline_flush() {

  vgfx_gl_buffer_sub_data(
    &s_rd_bound_pipeline->vb, 
    0, 
    s_rd_bound_pipeline->crn_render_count * sizeof(VGFX_RD_Vertex), 
    s_rd_bound_pipeline->cpu_vb.ptr
  );

  glBindVertexArray(s_rd_bound_pipeline->va.handle);

  glDrawArrays(GL_TRIANGLES, 0, s_rd_bound_pipeline->crn_render_count);

  glBindVertexArray(0);

  vgfx_gl_unbind_shader_program();

  s_rd_bound_pipeline = NULL;
}

// =============================================
//
//
// Render
//
//
// =============================================

void vgfx_rd_send_vert(f32 shader, f32 pos[3], f32 col[4]) {
  VGFX_RD_Vertex *v = &vstd_vector_get(VGFX_RD_Vertex, 
        s_rd_bound_pipeline->cpu_vb, s_rd_bound_pipeline->crn_render_count);

  v->shader = shader;

  v->pos[0] = pos[0];
  v->pos[1] = pos[1];
  v->pos[2] = pos[2];

  v->col[0] = col[0];
  v->col[1] = col[1];
  v->col[2] = col[2];
  v->col[3] = col[3];

  s_rd_bound_pipeline->crn_render_count += 1;
}

void vgfx_rd_send_quad(f32 shader, f32 pos[3], f32 scl[2], f32 col[4]) {
  
  // First triangle
  vgfx_rd_send_vert(shader, (vec3){pos[0], pos[1], pos[2]},  col);

  vgfx_rd_send_vert(shader, (vec3){pos[0] + scl[0], pos[1], pos[2]},  col);

  vgfx_rd_send_vert(shader, (vec3){pos[0], pos[1] + scl[1], pos[2]},  col);

  // Second triangle
  vgfx_rd_send_vert(shader, (vec3){pos[0] + scl[0], pos[1], pos[2]},  col);

  vgfx_rd_send_vert(shader, (vec3){pos[0], pos[1] + scl[1], pos[2]},  col);

  vgfx_rd_send_vert(shader, (vec3){pos[0] + scl[0], pos[1] + scl[1], pos[2]},  col);
}
