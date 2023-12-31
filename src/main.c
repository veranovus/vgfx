#include "vgfx/asset.h"
#include "vgfx/camera.h"
#include "vgfx/input.h"
#include "vgfx/core.h"
#include "vgfx/gl.h"
#include "vgfx/os.h"
#include "vgfx/render.h"
#include <time.h>

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "vgfx";

const char *BASE_FRAG_SHADER_PATH = "res/shader/base.frag";
const char *BASE_VERT_SHADER_PATH = "res/shader/base.vert";
const char *TEXT_FRAG_SHADER_PATH = "res/shader/text.frag";
const char *TEXT_VERT_SHADER_PATH = "res/shader/text.vert";

const char *TEST_FONT_PATH = "res/font/JetBrainsMono-Regular.ttf";
const char *TEST_TEXTURE_PATH = "res/bunny.png";

static VGFX_RD_Camera *s_camera = NULL;

typedef struct Object {
  vec3 pos;
  vec3 scl;
  vec4 col;
  vec2 dir;
} Object;

void spawn_bunny(VSTD_Vector(Object) *objs, f32 x, f32 y, usize num) {

  for (usize i = 0; i < num; ++i) {
    f32 smodx = (rand() % 100 < 50) ? (rand() % 1000) / -1000.0f
                                    : (rand() % 1000) / 1000.0f;
    f32 smody = (rand() % 100 < 50) ? (rand() % 1000) / -1000.0f
                                    : (rand() % 1000) / 1000.0f;
    
    f32 dirx = (rand() % 100 < 50) ? (rand() % 100) / -100.0f
                                   : (rand() % 100) / 100.0f;
    f32 diry = (rand() % 100 < 50) ? (rand() % 100) / -100.0f
                                   : (rand() % 100) / 100.0f;

    f32 r = (f32)(rand() % 255 + 1) / 255.0f;
    f32 g = (f32)(rand() % 255 + 1) / 255.0f;
    f32 b = (f32)(rand() % 255 + 1) / 255.0f;

    vstd_vector_push(
      Object,
      objs,
      ((Object){
        .pos = {x, y, 0.0f},
        .scl = {32.0f, 32.0f, 1.0f},
        .col = {r, g, b, 1.0f},
        .dir = {dirx + smodx, diry + smody},
    }));
  }
}

int main(i32 argc, char *argv[]) {

  VGFX_UNUSED(argc);
  VGFX_UNUSED(argv);

  srand(time(NULL));

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

  // Asset Server
  VGFX_AS_AssetServer *asset_server = vgfx_as_asset_server_new();

  // Load texture
  VGFX_AS_Asset *texture = vgfx_as_asset_server_load(asset_server, &(VGFX_AS_AssetDesc){
    .type = VGFX_ASSET_TYPE_TEXTURE,
    .texture_path = TEST_TEXTURE_PATH,
    .texture_filter = GL_NEAREST,
    .texture_wrap = GL_REPEAT,
  });

  // Load font
  VGFX_AS_Asset *font = vgfx_as_asset_server_load(asset_server, &(VGFX_AS_AssetDesc){
    .type = VGFX_ASSET_TYPE_FONT,
    .font_path = TEST_FONT_PATH,
    .font_filter = GL_LINEAR,
    // .font_filter = GL_NEAREST,
    .font_range = { 32, 128 },
    .font_size = 32,
  });

  // Load shader programs
  VGFX_AS_Asset *base_shader = vgfx_as_asset_server_load(asset_server, &(VGFX_AS_AssetDesc) {
    .type = VGFX_ASSET_TYPE_SHADER,
    .shader_vert_path = BASE_VERT_SHADER_PATH,
    .shader_frag_path = BASE_FRAG_SHADER_PATH,
  });

  VGFX_AS_Asset *text_shader = vgfx_as_asset_server_load(asset_server, &(VGFX_AS_AssetDesc) {
    .type = VGFX_ASSET_TYPE_SHADER,
    .shader_vert_path = TEXT_VERT_SHADER_PATH,
    .shader_frag_path = TEXT_FRAG_SHADER_PATH,
  });

  // Create pipeline
  VGFX_RD_Pipeline *pipeline = vgfx_rd_pipeline_new(asset_server);

  // Setup camera
  s_camera = vgfx_rd_camera_new(&(VGFX_RD_CameraDesc) {
    .mode = VGFX_RD_CAMERA_MODE_ORTHO,
    .projection_settings = {
      .near = 0.0f,
      .far = 1000.0f,
      .ortho_width = WINDOW_WIDTH,
      .ortho_height = WINDOW_HEIGHT,
    },
    .position = { 0.0f, 0.0f, 999.9f },
  });

  // Setup render pipeline
  const usize MAX_BUNNY = 100000;

  VSTD_Vector(Object) objs = vstd_vector_with_capacity(Object, MAX_BUNNY);

  spawn_bunny(&objs, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 10000);

  bool run = true;
  bool spawn = false;

  f32 ft[10] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  usize ft_counter = 0;

  VSTD_String frm_str = vstd_string_from("FRM: 0.0 ms");
  VSTD_String cnt_str = vstd_string_from("CNT: 0");

  volatile u32 fps_counter, fps;
  volatile f64 fps_timer = 0.0f;
  volatile f64 frm_timer = 0.0f;
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

      vstd_string_free(&cnt_str);
      cnt_str = vstd_string_format("CNT: %u", objs.len);
    }

    ft[ft_counter] = dt;
    ft_counter += 1;
    if (ft_counter >= 10) {
      ft_counter = 0;
    }

    frm_timer += dt;
    if (frm_timer > 0.1f) {
      frm_timer = 0.0f;

      f32 aft = 0.0f;
      for (usize i = 0; i < 10; ++i) {
        aft += ft[i];
      }
      aft /= 10.0f;

      vstd_string_free(&frm_str);
      frm_str = vstd_string_format("FRM: %.2f ms", aft * 1000);
    }

    VSTD_Vector(VGFX_OS_Event) events = vgfx_os_events(win);
    vstd_vector_iter(VGFX_OS_Event, events, {
      if (_$iter->type == VGFX_OS_EVENT_TYPE_WINDOW_CLOSE) {
        run = false;
      }

      if (_$iter->type == VGFX_OS_EVENT_TYPE_KEY_PRESS) {
        if (_$iter->key_code == VGFX_IN_KEY_ENTER) {
          spawn = true;
        }
      }
    });

    if (spawn) {
      f64 x, y;
      glfwGetCursorPos((void*)win, &x, &y);

      y = WINDOW_HEIGHT - y;
  
      spawn_bunny(&objs, x, y, 10000);

      spawn = false;
    }

    f32 ww = WINDOW_WIDTH;
    f32 wh = WINDOW_HEIGHT;
    vec2 tmp;
    const f32 speed = 100.0f * dt;

    for (usize i = 0; i < objs.len; ++i) {
      Object *obj = &vstd_vector_get(Object, objs, i);

      glm_vec2_scale(obj->dir, speed, tmp);

      if (obj->pos[0] + tmp[0] < 0 || obj->pos[0] + obj->scl[0] + tmp[0] > ww) {
        obj->dir[0] = -obj->dir[0];
      } else {
        obj->pos[0] += tmp[0];
      }

      if (obj->pos[1] + tmp[1] < 0 || obj->pos[1] + obj->scl[1] + tmp[1] > wh) {
        obj->dir[1] = -obj->dir[1];
      } else {
        obj->pos[1] += tmp[1];
      }      
    }

    // Update camera view
    vgfx_rd_camera_update_view(s_camera);

    // Get view & projection matrix
    mat4 vpm;
    vgfx_rd_camera_combined_matrix(s_camera, vpm);

    // Set instance data
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the scene
    vgfx_rd_pipeline_begin(pipeline, base_shader);

    vgfx_gl_uniform_fv("u_time", 1, (f32[1]){(f32)time});
    vgfx_gl_uniform_mat4fv("u_vpm", 1, false, &vpm[0][0]);

    VGFX_AS_Texture *th;
    VGFX_ASSET_DEBUG_CAST(texture, VGFX_ASSET_TYPE_TEXTURE, th);

    vstd_vector_iter(Object, objs, {
      vgfx_rd_send_texture(th, _$iter->pos, _$iter->scl, NULL, _$iter->col);
    });

    vgfx_rd_pipeline_flush();

    vgfx_rd_pipeline_begin(pipeline, text_shader);

    vgfx_gl_uniform_fv("u_time", 1, (f32[1]){(f32)time});
    vgfx_gl_uniform_mat4fv("u_vpm", 1, false, &vpm[0][0]);

    VGFX_AS_Font *fh;
    VGFX_ASSET_DEBUG_CAST(font, VGFX_ASSET_TYPE_FONT, fh);

    vec2s tsize0 = vgfx_rd_font_render_size(fh, frm_str.ptr, true);
    vec2s tsize1 = vgfx_rd_font_render_size(fh, cnt_str.ptr, true);

    vgfx_rd_send_text(
      fh, frm_str.ptr, 
      (vec3){0.0f, (f32)WINDOW_HEIGHT - tsize0.y, 100.0f}, 
      (vec4){1.0f, 1.0f, 1.0f, 1.0f}
    );

    vgfx_rd_send_text(
      fh, cnt_str.ptr, 
      (vec3){0.0f, (f32)WINDOW_HEIGHT - (tsize0.y + tsize1.y), 100.0f}, 
      (vec4){1.0f, 1.0f, 1.0f, 1.0f}
    );

    vgfx_rd_pipeline_flush();

    vgfx_os_window_swap_buffers(win);
    vgfx_os_poll_events();
  }

  // Delete vectors
  vstd_vector_free(Object, (&objs));

  // Delete render pipeline
  vgfx_rd_piepline_free(pipeline);

  // Free resources
  vgfx_rd_camera_free(s_camera);

  // Free the vgfx
  vgfx_as_asset_server_free(asset_server);
  vgfx_os_window_free(win);

  return 0;
}
