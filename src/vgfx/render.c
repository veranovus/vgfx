#include "render.h"
#include "asset.h"

const char *VGFX_RD_BASE_VERT_PATH = "res/shader/default.vert";

const char *VGFX_RD_BASE_FRAG_PATH = "res/shader/default.frag";

const usize VGFX_RD_MAX_VERTEX_COUNT = 15000 * 6;

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
    .type = VGFX_ASSET_TYPE_SHADER,
    .shader_vert_path = VGFX_RD_BASE_VERT_PATH,
    .shader_frag_path = VGFX_RD_BASE_FRAG_PATH,
  });

  pipeline->max_vertex_count = VGFX_RD_MAX_VERTEX_COUNT;
  pipeline->crn_vertex_count = 0;
  pipeline->crn_texture = 0;

  // OpenGL buffers
  pipeline->vb = vgfx_gl_buffer_create(GL_ARRAY_BUFFER);
  vgfx_gl_buffer_data(
    &pipeline->vb, 
    GL_DYNAMIC_DRAW, 
    sizeof(VGFX_RD_Vertex) * pipeline->max_vertex_count, 
    NULL
  );

  pipeline->va = vgfx_gl_vertex_array_create();

  VGFX_GL_VertexAttribLayout layout = {
    .buffer = pipeline->vb,
    .update_freq = 0,
    .attribs = {
      {1, GL_FLOAT, GL_FALSE},
      {1, GL_FLOAT, GL_FALSE},
      {3, GL_FLOAT, GL_FALSE},
      {2, GL_FLOAT, GL_FALSE},
      {4, GL_FLOAT, GL_FALSE},
    },
  };
  vgfx_gl_vertex_array_layout(&pipeline->va, &layout);

  // CPU buffer
  pipeline->cpu_vb = vstd_vector_with_capacity(
        VGFX_RD_Vertex, pipeline->max_vertex_count);
  
  return pipeline;
}

void vgfx_rd_piepline_free(VGFX_RD_Pipeline *pipeline) {

  VGFX_ASSERT_NON_NULL(pipeline);

  vgfx_gl_buffer_delete(&pipeline->vb);

  vgfx_gl_vertex_array_delete(&pipeline->va);

  vstd_vector_free(VGFX_RD_Vertex, (&pipeline->cpu_vb));

  free(pipeline);
}

void vgfx_rd_pipeline_begin(VGFX_RD_Pipeline *pipeline) {

  VGFX_ASSERT_NON_NULL(pipeline);

  pipeline->crn_vertex_count = 0;

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
    s_rd_bound_pipeline->crn_vertex_count * sizeof(VGFX_RD_Vertex), 
    s_rd_bound_pipeline->cpu_vb.ptr
  );

  for (usize i = 0; i < VGFX_RD_MAX_BOUND_TEXTURE; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, s_rd_bound_pipeline->textures[i]);

    VSTD_String tmp = vstd_string_format("u_texture[%lu]", i);
    vgfx_gl_uniform_fv(tmp.ptr, 1, (f32[1]){i});
    vstd_string_free(&tmp);
  }

  glBindVertexArray(s_rd_bound_pipeline->va.handle);

  glDrawArrays(GL_TRIANGLES, 0, s_rd_bound_pipeline->crn_vertex_count);

  glBindVertexArray(0);

  for (usize i = 0; i < VGFX_RD_MAX_BOUND_TEXTURE; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

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

void vgfx_rd_send_vert(f32 shader, f32 texture, vec3 pos, vec2 tex, vec4 col) {

  if (s_rd_bound_pipeline->crn_vertex_count == s_rd_bound_pipeline->max_vertex_count) {
    VGFX_RD_Pipeline *tmp = s_rd_bound_pipeline;

    vgfx_rd_pipeline_flush();
    vgfx_rd_pipeline_begin(tmp);
  }
  
  VGFX_RD_Vertex *v = &vstd_vector_get(VGFX_RD_Vertex, 
        s_rd_bound_pipeline->cpu_vb, s_rd_bound_pipeline->crn_vertex_count);

  v->shader = shader;
  v->texture = texture;

  v->pos[0] = pos[0];
  v->pos[1] = pos[1];
  v->pos[2] = pos[2];

  v->tex[0] = tex[0];
  v->tex[1] = tex[1];

  v->col[0] = col[0];
  v->col[1] = col[1];
  v->col[2] = col[2];
  v->col[3] = col[3];

  s_rd_bound_pipeline->crn_vertex_count += 1;
}

void vgfx_rd_send_quad(f32 shader, f32 texture, vec3 pos, vec2 scl, vec4 col) {
  
  // First triangle
  vgfx_rd_send_vert(shader, texture, (vec3){pos[0], pos[1], pos[2]}, (vec2){0.0f, 1.0f}, col);

  vgfx_rd_send_vert(shader, texture, (vec3){pos[0] + scl[0], pos[1], pos[2]}, (vec2){1.0f, 1.0f}, col);

  vgfx_rd_send_vert(shader, texture, (vec3){pos[0], pos[1] + scl[1], pos[2]}, (vec2){0.0f, 0.0f}, col);

  // Second triangle
  vgfx_rd_send_vert(shader, texture, (vec3){pos[0] + scl[0], pos[1], pos[2]}, (vec2){1.0f, 1.0f}, col);

  vgfx_rd_send_vert(shader, texture, (vec3){pos[0], pos[1] + scl[1], pos[2]}, (vec2){0.0f, 0.0f}, col);

  vgfx_rd_send_vert(shader, texture, (vec3){pos[0] + scl[0], pos[1] + scl[1], pos[2]}, (vec2){1.0f, 0.0f}, col);
}

void vgfx_rd_send_text(VGFX_AS_Asset *text, vec3 pos, vec2 scl, vec4 col) {

  VGFX_DEBUG_ASSERT(text, "Handle is NULL.");

  VGFX_AS_Texture* handle;
  VGFX_ASSET_DEBUG_CAST(text, VGFX_ASSET_TYPE_TEXTURE, handle);

  isize slot = -1;
  for (usize i = 0; i < VGFX_RD_MAX_BOUND_TEXTURE; ++i) {
    if (handle->handle != s_rd_bound_pipeline->textures[i]) {
      continue;
    }

    slot = i;
    break;
  }

  if (slot < 0) {
    if (s_rd_bound_pipeline->crn_texture == VGFX_RD_MAX_BOUND_TEXTURE) {
      VGFX_RD_Pipeline *pipeline = s_rd_bound_pipeline;

      vgfx_rd_pipeline_flush();
      vgfx_rd_pipeline_begin(pipeline);  
    }
    
    slot = s_rd_bound_pipeline->crn_texture;

    s_rd_bound_pipeline->textures[slot] = handle->handle;
    s_rd_bound_pipeline->crn_texture += 1;
  }

  vgfx_rd_send_quad(0, slot, pos, scl, col);
}
