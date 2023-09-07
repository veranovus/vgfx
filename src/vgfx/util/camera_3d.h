#pragma once

#include "../camera.h"
#include "../common.h"

typedef struct VGFX_Camera3D {
  VGFX_Camera *camera;
  f32 yaw;
  f32 pitch;
  f32 speed;
  f32 sensivity;
  bool editor_mode;
  vec2 _cursor_last;
} VGFX_Camera3D;

static const f32 VGFX_CAMERA3D_DEFAULT_SPEED = 5.0f;
static const f32 VGFX_CAMERA3D_DEFAULT_SENSIVITY = 0.1f;

// VGFX_Camera3D
// =============

VGFX_Camera3D *vgfx_camera3d_new(f32 fov, f32 width, f32 height, f32 near,
                                 f32 far);

void vgfx_camera3d_free(VGFX_Camera3D *camera);

// Helper functions
// ----------------

void vgfx_camera3d_update_vectors(VGFX_Camera3D *camera);

// Camera input functions
// ----------------------

void vgfx_camera3d_respond_key(VGFX_Camera3D *camera, i32 key, f64 dt);

void vgfx_camera3d_respond_cursor(VGFX_Camera3D *camera, f32 x, f32 y);
