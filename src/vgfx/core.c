#include "core.h"

#define B_STACKTRACE_IMPL
#include <b_stacktrace.h>

// =============================================
//
//
// Abort & Assertions
//
//
// =============================================

_Noreturn void 
_vgfx_abort(const char *file, i32 line, const char *msg, ...) {

  fprintf(stderr, "Abort: ");

  va_list va_args;
  va_start(va_args, msg);
  vfprintf(stderr, msg, va_args);
  va_end(va_args);

  fprintf(stderr, "\nFile: %s:%u\n", file, line);
  char *stacktrace = b_stacktrace_get_string();
  fprintf(stderr, "Stacktrace:\n%s\n\n", stacktrace);
  abort();
}

void 
_vgfx_assert_failed(const char *cond, const char *file, i32 line, const char *msg, ...) {

  fprintf(stderr, "Assertion failed: %s\nMessage: ", cond);

  va_list va_args;
  va_start(va_args, msg);
  vfprintf(stderr, msg, va_args);
  va_end(va_args);

  fprintf(stderr, "\nFile: %s:%u\n", file, line);
  char *stacktrace = b_stacktrace_get_string();
  fprintf(stderr, "Stacktrace:\n%s\n\n", stacktrace);
  abort();
}

// =============================================
//
//
// Print
//
//
// =============================================

void 
_vgfx_debug_print(const char *msg, ...) {

  printf("Debug: ");

  va_list va_args;
  va_start(va_args, msg);
  vprintf(msg, va_args);
  va_end(va_args);
}

void 
_vgfx_debug_warn(const char *msg, ...) {

  printf("Warning: ");

  va_list va_args;
  va_start(va_args, msg);
  vprintf(msg, va_args);
  va_end(va_args);
}
