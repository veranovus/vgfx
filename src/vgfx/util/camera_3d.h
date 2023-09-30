#pragma once

#include "../camera.h"
#include "../core.h"
#include "../window.h"

/*****************************************************************************
 * - Types
 * */

// VGFX Camera3D

static const f32 VGFX_CAMERA3D_DEFAULT_SPEED = 5.0f;

static const f32 VGFX_CAMERA3D_DEFAULT_SENSIVITY = 0.1f;

typedef struct VGFX_Camera3D {
  VGFX_Camera *camera;
  f32 yaw;
  f32 pitch;
  f32 speed;
  f32 sensivity;
} VGFX_Camera3D;

/*****************************************************************************
 * - VGFX Camera 3D
 * */

VGFX_Camera3D *vgfx_camera3d_new(f32 fov, f32 width, f32 height, f32 near,
                                 f32 far);

void vgfx_camera3d_free(VGFX_Camera3D *camera);

void vgfx_camera3d_handle_input(VGFX_Camera3D *camera,
                                const VGFX_Window *window, f64 dt,
                                bool editor_mode);

/*****************************************************************************
 * - Helper Functions
 * */

void vgfx_camera3d_update_vectors(VGFX_Camera3D *camera);

/*****************************************************************************
 * - Camera Input Functions
 * */

void _vgfx_camera3d_key_input(VGFX_Camera3D *camera, const VGFX_Window *window,
                              f64 dt, bool editor_mode);

void _vgfx_camera3d_cursor_input(VGFX_Camera3D *camera,
                                 const VGFX_Window *window, bool editor_mode);
