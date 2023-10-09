#include "render.h"

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

  // Properties
  pipeline->max_vertex_count = VGFX_RD_MAX_VERTEX_COUNT;
  pipeline->crn_vertex_count = 0;
  pipeline->crn_index_count  = 0;
  pipeline->crn_texture      = 0;

  // Pipeline cache
  pipeline->_cache.texture        = VGFX_GL_INVALID_HANDLE;
  pipeline->_cache.internal_flush = false;

  // OpenGL buffers
  pipeline->vb = vgfx_gl_buffer_create(GL_ARRAY_BUFFER);
  vgfx_gl_buffer_data(
    &pipeline->vb, 
    GL_DYNAMIC_DRAW, 
    VGFX_RD_MAX_VERTEX_COUNT * sizeof(VGFX_RD_Vertex), 
    NULL
  );

  pipeline->ib = vgfx_gl_buffer_create(GL_ELEMENT_ARRAY_BUFFER);
  
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

  vgfx_gl_vertex_array_index_buffer(&pipeline->va, &pipeline->ib);

  // CPU buffer
  pipeline->cpu_vb = vstd_vector_with_capacity(
        VGFX_RD_Vertex, VGFX_RD_MAX_VERTEX_COUNT);

  // Send indices
  VSTD_Vector(u32) tmp = vstd_vector_with_capacity(
        u32, VGFX_RD_MAX_INDEX_COUNT);

  usize offset = 0;
  for (u32* ptr = (u32*)tmp.ptr; ptr < (u32*)tmp.ptr + VGFX_RD_MAX_INDEX_COUNT; ptr += 6) {

    ptr[0] = offset + 1;
    ptr[1] = offset + 3;
    ptr[2] = offset + 2;
    ptr[3] = offset + 1;
    ptr[4] = offset + 0;
    ptr[5] = offset + 2;

    offset += 4;
  }

  vgfx_gl_buffer_data(
    &pipeline->ib,
    GL_STATIC_DRAW,
    VGFX_RD_MAX_INDEX_COUNT * sizeof(u32),
    tmp.ptr
  );

  vstd_vector_free(u32, (&tmp));
  
  return pipeline;
}

void vgfx_rd_piepline_free(VGFX_RD_Pipeline *pipeline) {

  VGFX_ASSERT_NON_NULL(pipeline);

  vgfx_gl_buffer_delete(&pipeline->vb);
  vgfx_gl_buffer_delete(&pipeline->ib);

  vgfx_gl_vertex_array_delete(&pipeline->va);

  vstd_vector_free(VGFX_RD_Vertex, (&pipeline->cpu_vb));

  free(pipeline);
}

void vgfx_rd_pipeline_begin(VGFX_RD_Pipeline *pipeline, VGFX_AS_Asset *shader) {

  VGFX_ASSERT_NON_NULL(pipeline);

  if (!pipeline->_cache.internal_flush) {
    VGFX_DEBUG_ASSERT(shader, "Handle is NULL.");
    
    VGFX_AS_Shader *handle;
    VGFX_ASSET_CAST(shader, VGFX_ASSET_TYPE_SHADER, handle);

    vgfx_gl_bind_shader_program(handle->handle);
  }

  // Reset pipeline
  pipeline->crn_vertex_count = 0;
  pipeline->crn_index_count  = 0;
  pipeline->crn_texture      = 0;

  pipeline->_cache.texture        = VGFX_GL_INVALID_HANDLE;
  pipeline->_cache.internal_flush = false;

  s_rd_bound_pipeline = pipeline;
}

void vgfx_rd_pipeline_flush() {

  if (!s_rd_bound_pipeline->crn_vertex_count) {
    return;
  }

  // Send vertex data
  vgfx_gl_buffer_sub_data(
    &s_rd_bound_pipeline->vb, 
    0, 
    s_rd_bound_pipeline->crn_vertex_count * sizeof(VGFX_RD_Vertex), 
    s_rd_bound_pipeline->cpu_vb.ptr
  );

  // Set textures
  for (usize i = 0; i < s_rd_bound_pipeline->crn_texture; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, s_rd_bound_pipeline->textures[i]);

    if (s_rd_bound_pipeline->_cache.internal_flush) {
      continue;
    }

    VSTD_String tmp = vstd_string_format("u_texture[%lu]", i);
    vgfx_gl_uniform_fv(tmp.ptr, 1, (f32[1]){i});
    vstd_string_free(&tmp);
  }

  // Draw the vertices
  glBindVertexArray(s_rd_bound_pipeline->va.handle);

  glDrawElements(
    GL_TRIANGLES, s_rd_bound_pipeline->crn_index_count, GL_UNSIGNED_INT, NULL);

  if (!s_rd_bound_pipeline->_cache.internal_flush) {
    vgfx_gl_unbind_shader_program();
  }

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

    s_rd_bound_pipeline->_cache.internal_flush = true;

    vgfx_rd_pipeline_flush();
    vgfx_rd_pipeline_begin(tmp, NULL);
  }
  
  VGFX_RD_Vertex *v = &vstd_vector_get(
    VGFX_RD_Vertex, 
    s_rd_bound_pipeline->cpu_vb, 
    s_rd_bound_pipeline->crn_vertex_count
  );

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

  // Send vertices
  vec3 tpos = {pos[0], pos[1], pos[2]};
  vec2 ttex = {tex[0], tex[1] + tex[3]};
  
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] += scl[0];

  ttex[0] += tex[2];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] -= scl[0];
  tpos[1] += scl[1];

  ttex[0] -= tex[2];
  ttex[1] -= tex[3];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  tpos[0] += scl[0];

  ttex[0] += tex[2];
  vgfx_rd_send_vert(texture, tpos, ttex, col);

  // Increase index count
  s_rd_bound_pipeline->crn_index_count += 6;
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

      s_rd_bound_pipeline->_cache.internal_flush = true;

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
    vgfx_rd_send_quad(slot, pos, scl, VGFX_RD_NO_SUB_TEXTURE, col);
  } else {
    vgfx_rd_send_quad(slot, pos, scl, tex, col);
  }
}

void vgfx_rd_send_text(VGFX_AS_Font *handle, const char* str, vec3 pos, vec4 col) {

  VGFX_DEBUG_ASSERT(handle, "Handle is NULL.");

  VGFX_AS_Texture tmp;
  tmp.handle = handle->handle;

  usize len    = strlen(str);
  f32   offset = 0;

  for (usize i = 0; i < len; ++i) {
    _VGFX_AS_Glyph *glyph = &vstd_vector_get(
      _VGFX_AS_Glyph, handle->glyphs, ((usize)str[i] - handle->range[0])
    );

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

vec2s vgfx_rd_font_render_size(VGFX_AS_Font *handle, const char *str, bool fh) {
  
  VGFX_DEBUG_ASSERT(handle, "Handle is NULL.");

  VGFX_AS_Texture tmp;
  tmp.handle = handle->handle;

  usize len = strlen(str);

  f32 w = 0;
  f32 h = 0;

  for (usize i = 0; i < len; ++i) {
    _VGFX_AS_Glyph *glyph = &vstd_vector_get(
      _VGFX_AS_Glyph, handle->glyphs, ((usize)str[i] - handle->range[0])
    );

    if (glyph->size[0] > h) {
      h = glyph->size[0];
    }

    w += glyph->advn[0];
  }

  if (fh) {
    h = handle->_average_glyph_height;
  }

  return (vec2s) {.x = w, .y = h};
}
