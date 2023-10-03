#include "vgfx/camera.h"
#include "vgfx/core.h"
#include "vgfx/font.h"
#include "vgfx/os.h"
#include "vgfx/render.h"
#include "vgfx/shader.h"
#include "vgfx/texture.h"
#include <time.h>

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

const char *FRAG_SHADER_PATH = "res/shader/base.frag";
const char *VERT_SHADER_PATH = "res/shader/base.vert";
const char *TEST_FONT_PATH = "res/font/FiraCode-Medium.ttf";
const char *TEST_TEXTURE_PATH = "res/bunny.png";

static VGFX_Camera *s_camera = NULL;

int main(i32 argc, char *argv[]) {

  VGFX_UNUSED(argc);
  VGFX_UNUSED(argv);

  // VGFX setup
  VSTD_String title = vstd_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  VGFX_OS_WindowHandle win = vgfx_os_window_open(&(VGFX_OS_WindowDesc){
      .title = title.ptr,
      .width = WINDOW_WIDTH,
      .height = WINDOW_HEIGHT,
      .vsync = false,
      .resizable = true,
      .decorated = true,
      .visible = true,
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

  // Load texture
  VGFX_Texture2D *texture =
      vgfx_texture_new(TEST_TEXTURE_PATH, GL_REPEAT, GL_NEAREST);

  // Setup camera
  s_camera = vgfx_camera_new(glm_rad(45.0f), WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f,
                             1000.0f, VGFX_CameraModeOrthographic);

  // Setup render pipeline
  const usize MAX_INSTANCE = 100000;

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

  mat4 *buff = (mat4 *)calloc(MAX_INSTANCE, sizeof(mat4));

  const usize INITIAL_OBJ_COUNT = 10000;

  typedef struct Object {
    vec3 pos;
    vec3 scl;
    vec2 dir;
  } Object;

  VSTD_Vector(Object) objs = vstd_vector_with_capacity(Object, MAX_INSTANCE);

  srand(time(NULL));

  for (usize i = 0; i < INITIAL_OBJ_COUNT; ++i) {
    f32 x = (WINDOW_WIDTH / 2.0f) - rand() % WINDOW_WIDTH;
    f32 y = (WINDOW_HEIGHT / 2.0f) - rand() % WINDOW_HEIGHT;

    f32 dirx = (rand() % 100 < 50) ? (rand() % 100) / -100.0f
                                   : (rand() % 100) / 100.0f;
    f32 diry = (rand() % 100 < 50) ? (rand() % 100) / -100.0f
                                   : (rand() % 100) / 100.0f;

    vstd_vector_push(Object, (&objs),
                     ((Object){.pos = {x, y, 0.0f},
                               .scl = {25.0f, 25.0f, 1.0f},
                               .dir = {dirx, diry}}));
  }

  bool run = true;

  volatile u32 fps_counter, fps;
  volatile f64 fps_timer = 0.0f;
  volatile f64 dt, last_frame;
  while (run) {
    // Time and delta time
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    fps_timer += dt;
    fps_counter += 1;
    if (fps_timer > 1.0f) {
      fps_timer = 0;
      fps = fps_counter;
      fps_counter = 0;
      printf("FPS: %u\n", fps);
    }

    VSTD_Vector(VGFX_OS_Event) events = vgfx_os_events(win);
    vstd_vector_iter(VGFX_OS_Event, events, {
      if (_$iter->type == VGFX_OS_EVENT_TYPE_WINDOW_CLOSE) {
        run = false;
      }

      if (_$iter->type == VGFX_OS_EVENT_TYPE_CHAR) {
        char chr[5];
        vgfx_utf8_to_cstr(vgfx_utf8_encode(_$iter->char_codepoint), chr);

        VGFX_DEBUG_PRINT("(Codepoint, UTF-8 Char) : (%d, %s)\n",
                         _$iter->char_codepoint, chr);
      }
    });

    // Update camera view
    vgfx_camera_update_view(s_camera);

    // Calculate mvp matrix
    mat4 vpm;
    vgfx_camera_get_matrix(s_camera, vpm);

    // Set instance data
    f32 ww = WINDOW_WIDTH / 2.0f;
    f32 wh = WINDOW_HEIGHT / 2.0f;
    usize instance_count = 0;
    vec2 tmp;
    const f32 speed = 100.0f * dt;

    for (usize i = 0; i < objs.len; ++i) {
      Object *obj = &vstd_vector_get(Object, objs, i);

      glm_vec2_scale(obj->dir, speed, tmp);

      if (obj->pos[0] + tmp[0] < -ww || obj->pos[0] + tmp[0] > ww) {
        obj->dir[0] = -obj->dir[0];
      } else {
        obj->pos[0] += tmp[0];
      }

      if (obj->pos[1] + tmp[1] < -wh || obj->pos[1] + tmp[1] > wh) {
        obj->dir[1] = -obj->dir[1];
      } else {
        obj->pos[1] += tmp[1];
      }

      glm_mat4_identity(buff[i]);
      glm_translate(buff[i], obj->pos);
      glm_scale(buff[i], obj->scl);
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

    vgfx_texture_bind(texture, 0);
    // vgfx_texture_handle_bind(font->handle, 0);

    glBindVertexArray(va);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                            instance_count);

    glBindVertexArray(0);
    vgfx_texture_unbind(texture);
    // vgfx_texture_handle_unbind(0);
    glUseProgram(0);

    // vgfx_window_swap_buffers(window);
    // vgfx_window_poll_events(window);
    vgfx_os_window_swap_buffers(win);
    vgfx_os_poll_events();
  }

  // Delete vectors
  vstd_vector_free(Object, (&objs));
  free(buff);

  // Delete render pipeline
  vgfx_vertex_array_free(va);
  vgfx_buffer_free(vbuff);
  vgfx_buffer_free(ibuff);
  vgfx_buffer_free(dbuff);

  // Free resources
  vgfx_texture_free(texture);
  vgfx_shader_program_free(base_program);
  vgfx_camera_free(s_camera);

  // Free the vgfx
  vgfx_os_window_free(win);
  // vgfx_window_free(window);
  // vgfx_terminate();

  return 0;
}
