#pragma once

#include "core.h"

// =============================================
//
//
// Camera
//
//
// =============================================

typedef i32 VGFX_RD_CameraMode;
enum VGFX_RD_CameraMode {
  VGFX_RD_CAMERA_MODE_ORTHO,
  VGFX_RD_CAMERA_MODE_PERSP,
};

typedef struct VGFX_RD_CameraPS VGFX_RD_CameraPS;
struct VGFX_RD_CameraPS {
  f32                near;
  f32                far;
  union {
    // VGFX_RD_CAMERA_MODE_ORTHO
    struct {
      f32            ortho_width;
      f32            ortho_height;
    };
    // VGFX_RD_CAMERA_MODE_PERSP
    struct {
      f32            persp_fov;
      f32            persp_aspect_ratio;
    };
  };
};

typedef struct VGFX_RD_CameraDesc VGFX_RD_CameraDesc;
struct VGFX_RD_CameraDesc {
  VGFX_RD_CameraMode mode;
  VGFX_RD_CameraPS   projection_settings;
  vec3               position;
};

typedef struct VGFX_RD_Camera VGFX_RD_Camera;
struct VGFX_RD_Camera {
  VGFX_RD_CameraMode mode;
  VGFX_RD_CameraPS   projection_settings;
  // Vectors
  vec3               position;
  vec3               front;
  vec3               right;
  vec3               up;
  vec3               world_up;
  // Matrices
  mat4               projection;
  mat4               view;
};

VGFX_RD_Camera *
vgfx_rd_camera_new(VGFX_RD_CameraDesc *desc);

void
vgfx_rd_camera_free(VGFX_RD_Camera *camera);

void
vgfx_rd_camera_projection_settings(VGFX_RD_Camera *camera, VGFX_RD_CameraPS *ps);

void
vgfx_rd_camera_update_view(VGFX_RD_Camera *camera);

void
vgfx_rd_camera_combined_matrix(VGFX_RD_Camera *camera, mat4 dest);
