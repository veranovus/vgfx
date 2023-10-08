#include "render.h"

const usize VGFX_RD_MAX_QUAD_COUNT = 15000;

const usize VGFX_RD_MAX_VERTEX_COUNT = VGFX_RD_MAX_QUAD_COUNT * 6;

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

  pipeline->max_vertex_count = VGFX_RD_MAX_VERTEX_COUNT;
  pipeline->crn_vertex_count = 0;
  pipeline->crn_texture = 0;

  pipeline->_cache.shader = NULL;
  pipeline->_cache.texture = VGFX_GL_INVALID_HANDLE;

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

void vgfx_rd_pipeline_begin(VGFX_RD_Pipeline *pipeline, VGFX_AS_Asset *shader) {

  VGFX_ASSERT_NON_NULL(pipeline);

  pipeline->crn_vertex_count = 0;
  pipeline->crn_texture = 0;

  if (shader) {
    pipeline->_cache.shader = shader;
  }
  pipeline->_cache.texture = VGFX_GL_INVALID_HANDLE;

  VGFX_AS_Shader *handle;
  VGFX_ASSET_CAST(pipeline->_cache.shader, VGFX_ASSET_TYPE_SHADER, handle);

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

void vgfx_rd_send_vert(f32 texture, vec3 pos, vec2 tex, vec4 col) {

  if (s_rd_bound_pipeline->crn_vertex_count == s_rd_bound_pipeline->max_vertex_count) {
    VGFX_RD_Pipeline *tmp = s_rd_bound_pipeline;

    vgfx_rd_pipeline_flush();
    vgfx_rd_pipeline_begin(tmp, NULL);
  }
  
  VGFX_RD_Vertex *v = &vstd_vector_get(VGFX_RD_Vertex, 
        s_rd_bound_pipeline->cpu_vb, s_rd_bound_pipeline->crn_vertex_count);

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

void vgfx_rd_send_quad(f32 texture, vec3 pos, vec2 scl, vec4 tex, vec4 col) {

  vec3 tpos = {pos[0], pos[1], pos[2]};
  vec2 ttex = {tex[0], tex[1] + tex[3]};
  
  // First triangle
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] += scl[0];

  ttex[0] += tex[2];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] -= scl[0];
  tpos[1] += scl[1];

  ttex[0] -= tex[2];
  ttex[1] -= tex[3];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  // Second triangle
  tpos[0] += scl[0];
  tpos[1] -= scl[1];

  ttex[0] += tex[2];
  ttex[1] += tex[3];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] -= scl[0];
  tpos[1] += scl[1];

  ttex[0] -= tex[2];
  ttex[1] -= tex[3];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] += scl[0];

  ttex[0] += tex[2];
  vgfx_rd_send_vert(texture, tpos, ttex, col);
}

void vgfx_rd_send_texture(VGFX_AS_Texture *handle, vec3 pos, vec2 scl, vec4 tex, vec4 col) {

  VGFX_DEBUG_ASSERT(handle, "Handle is NULL.");

  isize slot = -1;

  if (handle->handle == s_rd_bound_pipeline->_cache.texture) {
    slot = s_rd_bound_pipeline->crn_texture;    
    goto render_quad;
  }

  for (usize i = 0; i < VGFX_RD_MAX_BOUND_TEXTURE; ++i) {
    if (handle->handle != s_rd_bound_pipeline->textures[i]) {
      continue;
    }

    slot = i;
    break;
  }

  if (slot < 0) {
    if (s_rd_bound_pipeline->crn_texture == VGFX_RD_MAX_BOUND_TEXTURE) {
      VGFX_RD_Pipeline *tmp = s_rd_bound_pipeline;

      vgfx_rd_pipeline_flush();
      vgfx_rd_pipeline_begin(tmp, NULL);  
    }
    
    slot = s_rd_bound_pipeline->crn_texture;

    s_rd_bound_pipeline->_cache.texture = handle->handle;
    s_rd_bound_pipeline->textures[slot] = handle->handle;

    s_rd_bound_pipeline->crn_texture += 1;
  }

render_quad:
  if (!tex) {
    vgfx_rd_send_quad(slot, pos, scl, (vec4) {0.0f, 0.0f, 1.0f, 1.0f}, col);
  } else {
    vgfx_rd_send_quad(slot, pos, scl, tex, col);
  }
}

void vgfx_rd_send_text(VGFX_AS_Font *handle, const char* str, vec3 pos, vec4 col) {

  VGFX_DEBUG_ASSERT(handle, "Handle is NULL.");

  VGFX_AS_Texture tmp;
  tmp.handle = handle->handle;

  usize len = strlen(str);
  f32 offset = 0;

  for (usize i = 0; i < len; ++i) {
    _VGFX_AS_Glyph *glyph = &vstd_vector_get(_VGFX_AS_Glyph, handle->glyphs, (usize)str[i]);

    vec3 tpos = {
      pos[0] + offset + glyph->brng[0], 
      pos[1] - (glyph->size[1] - glyph->brng[1]), 
      pos[2],
    };

    vec4 tex = {
      glyph->offset,
      0.0f,
      glyph->size[0] / handle->size[0],
      glyph->size[1] / handle->size[1],
    };

    vgfx_rd_send_texture(&tmp, tpos, glyph->size, tex, col);

    offset += glyph->advn[0];
  }
}
