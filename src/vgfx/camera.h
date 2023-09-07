#pragma once

#include "common.h"

typedef enum VGFX_CameraMode {
  VGFX_CameraModePerspective,
  VGFX_CameraModeOrthographic,
} VGFX_CameraMode;

typedef struct VGFX_Camera {
  // Vectors
  vec3 position;
  vec3 front;
  vec3 right;
  vec3 up;
  vec3 world_up;
  // Projection settings
  f32 fov;
  f32 aspect_ratio;
  f32 near;
  f32 far;
  // Matrices
  mat4 projection;
  mat4 view;
  // Projection mode
  VGFX_CameraMode mode;
} VGFX_Camera;

// VGFX_Camera
// -----------

VGFX_Camera *vgfx_camera_new(f32 fov, f32 width, f32 height, f32 near, f32 far,
                             VGFX_CameraMode mode);

void vgfx_camera_free(VGFX_Camera *camera);

// Camera Projection & View functions
// ----------------------------------

void vgfx_camera_update_projection(VGFX_Camera *camera, f32 fov, f32 width,
                                   f32 height, f32 near, f32 far,
                                   VGFX_CameraMode mode);

void vgfx_camera_update_view(VGFX_Camera *camera);

// Camera helper functions
// -----------------------

void vgfx_camera_get_matrix(VGFX_Camera *camera, mat4 dest);
