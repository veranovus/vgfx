#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <vstd/vstd.h>

#define VGFX_UNUSED(expr) ((void)(expr))

// =============================================
//
//
// Abort & Assertions
//
//
// =============================================

#ifndef NDEBUG
#define VGFX_ENABLE_DEBUG_ASSERTIONS 1
#else
#define VGFX_ENABLE_DEBUG_ASSERTIONS 0
#endif

#ifndef VGFX_ABORT
#define VGFX_ABORT(...)                                                        \
  do {                                                                         \
    _vgfx_abort(__FILE__, __LINE__, __VA_ARGS__);                              \
  } while (0)
#endif

#ifndef VGFX_ASSERT
#define VGFX_ASSERT(cond, ...)                                                 \
  do {                                                                         \
    if (!(cond))                                                               \
      _vgfx_assert_failed(#cond, __FILE__, __LINE__, __VA_ARGS__);             \
  } while (0)
#endif

#if VGFX_ENABLE_DEBUG_ASSERTIONS
#define VGFX_DEBUG_ASSERT VGFX_ASSERT
#else
#define VGFX_DEBUG_ASSERT(cond, ...) (void)(cond)
#endif

#define VGFX_ASSERT_NON_NULL(val) VGFX_ASSERT(val, "%s can't be NULL.", #val)
#define VGFX_ASSERT_NON_ZERO(val) VGFX_ASSERT(val, "%s can't be zero.", #val)

_Noreturn void _vgfx_abort(const char *file, i32 line, const char *msg, ...);

void _vgfx_assert_failed(const char *cond, const char *file, i32 line,
                         const char *msg, ...);

// =============================================
//
//
// Debug Print
//
//
// =============================================

#ifndef NDEBUG
#define VGFX_ENABLE_DEBUG_PRINTS 1
#else
#define VGFX_ENABLE_DEBUG_PRINTS 0
#endif

#ifndef VGFX_DEBUG_PRINT
#define VGFX_DEBUG_PRINT(...)                                                  \
  do {                                                                         \
    if (VGFX_ENABLE_DEBUG_PRINTS)                                              \
      _vgfx_debug_print(__VA_ARGS__);                                          \
  } while (0)
#endif

#ifndef VGFX_DEBUG_WARN
#define VGFX_DEBUG_WARN(...)                                                   \
  do {                                                                         \
    if (VGFX_ENABLE_DEBUG_PRINTS) {                                            \
      _vgfx_debug_warn(__VA_ARGS__);                                           \
    }                                                                          \
  } while (0)
#endif

void _vgfx_debug_print(const char *msg, ...);

void _vgfx_debug_warn(const char *msg, ...);
