#include "cglm/sphere.h"
#include "vgfx/render.h"
#include "vgfx/util/camera_3d.h"
#include "vgfx/vgfx.h"
#include <time.h>

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

const char *FRAG_SHADER_PATH = "res/shader/base.frag";
const char *VERT_SHADER_PATH = "res/shader/base.vert";
const char *TEST_FONT_PATH = "res/font/FiraCode-Medium.ttf";
const char *TEST_TEXTURE_PATH = "res/dummy.png";

static VGFX_Camera *s_camera = NULL;
static bool s_editor_mode = false;

int main(i32 argc, char *argv[]) {

  // VGFX setup
  vgfx_initialize();

  VSTD_String title = vstd_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  VGFX_Window *window = vgfx_window_new(&(VGFX_WindowDescriptor){
      .title = title.ptr,
      .size = {(i32)WINDOW_WIDTH, (i32)WINDOW_HEIGHT},
      .vsync = false,
      .resizable = true,
      .decorated = true,
  });

  vstd_string_free(&title);

  // Base Shader program
  VGFX_Shader base_shaders[2];

  {
    // Vertex shader
    VSTD_String vertex_shader_source = vstd_fs_read_file(VERT_SHADER_PATH);

    VGFX_Shader vertex_shader = vgfx_shader_new(
        GL_VERTEX_SHADER, (const char **)&vertex_shader_source.ptr);

    vstd_string_free(&vertex_shader_source);

    base_shaders[0] = vertex_shader;

    // Fragment shader
    VSTD_String fragment_shader_source = vstd_fs_read_file(FRAG_SHADER_PATH);

    VGFX_Shader fragment_shader = vgfx_shader_new(
        GL_FRAGMENT_SHADER, (const char **)&fragment_shader_source.ptr);

    vstd_string_free(&fragment_shader_source);

    base_shaders[1] = fragment_shader;
  }

  VGFX_ShaderProgram base_program = vgfx_shader_program_new(base_shaders, 2);

  // Base Shader program
  VGFX_Shader text_shaders[2];

  {
    // Vertex shader
    VSTD_String vertex_shader_source =
        vstd_fs_read_file("res/shader/text.vert");

    VGFX_Shader vertex_shader = vgfx_shader_new(
        GL_VERTEX_SHADER, (const char **)&vertex_shader_source.ptr);

    vstd_string_free(&vertex_shader_source);

    text_shaders[0] = vertex_shader;

    // Fragment shader
    VSTD_String fragment_shader_source =
        vstd_fs_read_file("res/shader/text.frag");

    VGFX_Shader fragment_shader = vgfx_shader_new(
        GL_FRAGMENT_SHADER, (const char **)&fragment_shader_source.ptr);

    vstd_string_free(&fragment_shader_source);

    text_shaders[1] = fragment_shader;
  }

  VGFX_ShaderProgram text_program = vgfx_shader_program_new(text_shaders, 2);

  // Load font
  VGFX_Font *font = vgfx_font_new(TEST_FONT_PATH, VGFX_FONT_DEFAULT_SIZE);

  // Load texture
  VGFX_Texture2D *texture =
      vgfx_texture_new(TEST_TEXTURE_PATH, GL_REPEAT, GL_LINEAR);

  // Setup camera
  s_camera = vgfx_camera_new(glm_rad(45.0f), WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f,
                             1000.0f, VGFX_CameraModeOrthographic);

  // Setup render pipeline
  const usize MAX_INSTANCE = 10000;

  f32 vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 0.0f,
  };

  u32 indices[] = {0, 1, 3, 1, 2, 3};

  VGFX_Buffer vbuff = vgfx_buffer_new(&(VGFX_BufferDesc){
      .type = GL_ARRAY_BUFFER,
      .usage = GL_STATIC_DRAW,
      .size = sizeof(vertices),
      .data = vertices,
  });

  VGFX_Buffer ibuff = vgfx_buffer_new(&(VGFX_BufferDesc){
      .type = GL_ELEMENT_ARRAY_BUFFER,
      .usage = GL_STATIC_DRAW,
      .size = sizeof(indices),
      .data = indices,
  });

  VGFX_Buffer dbuff = vgfx_buffer_new(&(VGFX_BufferDesc){
      .type = GL_ARRAY_BUFFER,
      .usage = GL_DYNAMIC_DRAW,
      .size = sizeof(mat4) * MAX_INSTANCE,
      .data = NULL,
  });

  VGFX_VertexArray va = vgfx_vertex_array_new(&(VGFX_VertexArrayDesc){
      .index_buffer = ibuff,
      .layouts = {{
                      .buffer = vbuff,
                      .update = 0,
                      .attrib =
                          {
                              [0] = {3, GL_FLOAT, GL_FALSE},
                              [1] = {2, GL_FLOAT, GL_FALSE},
                          },
                  },
                  {
                      .buffer = dbuff,
                      .update = 1,
                      .attrib =
                          {
                              [2] = {4, GL_FLOAT, GL_FALSE},
                              [3] = {4, GL_FLOAT, GL_FALSE},
                              [4] = {4, GL_FLOAT, GL_FALSE},
                              [5] = {4, GL_FLOAT, GL_FALSE},
                          },
                  }},
  });

  const usize INITIAL_OBJ_COUNT = 10000;

  typedef struct Object {
    vec3 position;
    vec3 size;
  } Object;

  VSTD_Vector(Object) objs = vstd_vector_with_capacity(Object, MAX_INSTANCE);

  mat4 *buff = (mat4 *)calloc(MAX_INSTANCE, sizeof(mat4));

  srand(time(NULL));

  for (usize i = 0; i < INITIAL_OBJ_COUNT; ++i) {
    f32 x = (WINDOW_WIDTH / 2.0f) - rand() % WINDOW_WIDTH;
    f32 y = (WINDOW_HEIGHT / 2.0f) - rand() % WINDOW_HEIGHT;

    vstd_vector_push(Object, objs,
                     ((Object){
                         .position = {x, y, 0.0f},
                         .size = {30.0f, 30.0f, 1.0f},
                     }));
  }

  f64 dt, last_frame;

  while (!vgfx_window_get_window_close(window)) {
    // Time and delta time
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    usize instance_count = 0;

    // Set instance data

    // Update camera view
    vgfx_camera_update_view(s_camera);

    // Calculate mvp matrix
    mat4 vpm;
    vgfx_camera_get_matrix(s_camera, vpm);

    for (usize i = 0; i < objs.len; ++i) {
      Object *obj = &vstd_vector_get(Object, objs, i);

      glm_mat4_identity(buff[i]);
      glm_translate(buff[i], obj->position);
      glm_scale(buff[i], obj->size);
      glm_mat4_mul(vpm, buff[i], buff[i]);

      instance_count += 1;
    }

    vgfx_buffer_sub_data(dbuff, GL_ARRAY_BUFFER, 0,
                         sizeof(mat4) * instance_count, &buff[0]);

    // Render the scene
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(base_program);

    vgfx_shader_program_uniform_f1(base_program, "u_time", (f32)time);
    vgfx_shader_program_uniform_i1(base_program, "u_texture", 0);
    // vgfx_shader_program_uniform_mat4fv(base_program, "u_vpm", false,
    //                                    &vpm[0][0]);

    vgfx_texture_bind(texture, 0);
    // vgfx_texture_handle_bind(font->handle, 0);

    // glBindVertexArray(vao);
    glBindVertexArray(va);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                            instance_count);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    vgfx_texture_unbind(texture);
    // vgfx_texture_handle_unbind(0);
    glUseProgram(0);

    vgfx_window_swap_buffers(window);
    vgfx_window_poll_events(window);
  }

  // Delete vectors
  vstd_vector_free(Object, objs);
  free(buff);

  // Delete render pipeline
  vgfx_vertex_array_free(va);
  vgfx_buffer_free(vbuff);
  vgfx_buffer_free(ibuff);
  vgfx_buffer_free(dbuff);

  // Free resources
  vgfx_texture_free(texture);
  vgfx_font_free(font);
  vgfx_shader_program_free(base_program);
  vgfx_shader_program_free(text_program);
  vgfx_camera_free(s_camera);

  // Free the vgfx
  vgfx_window_free(window);
  vgfx_terminate();

  return 0;
}
