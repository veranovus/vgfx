#pragma once

#include "common.h"

/*****************************************************************************
 * - VGFX Core
 * */

void vgfx_initialize();

void vgfx_terminate();

/*****************************************************************************
 * - OpenGL Helper Functions
 * */

void _vgfx_dump_hardware_info();

void _vgfx_glew_initialize();
