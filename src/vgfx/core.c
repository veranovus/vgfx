#include "core.h"
#include "window.h"

#define B_STACKTRACE_IMPL
#include <b_stacktrace.h>

// =============================================
//
//
// Abort & Assertions
//
//
// =============================================

_Noreturn void _vgfx_abort(const char *file, i32 line, const char *msg, ...) {

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

void _vgfx_assert_failed(const char *cond, const char *file, i32 line,
                         const char *msg, ...) {

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

void _vgfx_debug_print(const char *msg, ...) {

  printf("Debug: ");

  va_list va_args;
  va_start(va_args, msg);
  vprintf(msg, va_args);
  va_end(va_args);
}

// =============================================
//
//
// UTF-8 & Unicode
//
//
// =============================================

// 1 byte -> 0xxxxxxx -------- -------- -------- : default   =>  7 bit
// 2 byte -> 110xxxxx 10xxxxxx -------- -------- : 5 + 6     => 11 bit
// 3 byte -> 1110xxxx 10xxxxxx 10xxxxxx -------- : 4 + 6 * 2 => 16 bit
// 4 byte -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx : 3 + 6 * 3 => 21 bit
// FORMULA : (6 - (b - 1)) + ((b - 1) * 6)

utf8_char vgfx_utf8_encode(u32 unicode) {

  // Find how many bits are used
  usize bit = 0;
  for (usize i = 31; i >= 0; --i) {
    if (!VGFX_CHECK_BIT(unicode, i)) {
      continue;
    }

    bit = i + 1;
    break;
  }

  // If the value is in ASCII range just return it as it is
  if (bit < 8) {
    return _vgfx_utf8_reverse_byte_order(unicode);
  }

  // Find required byte sized to fit
  usize byte = 0;
  for (usize i = 0; i < 4; ++i) {
    usize ab = (6 - i) + (i * 6);

    if (bit > ab) {
      continue;
    }

    byte = i + 1;
    break;
  }

  utf8_char tmp = 0;

  i32 rb = bit;

  for (i32 i = 3; i >= 0; --i) {
    for (i32 j = 7; j >= 0; --j) {
      i32 b = (i * 8) + j;

      // Place header markers and then skip
      if (i == 3) {
        if (j > 7 - (i32)byte) {
          tmp |= 1 << b;
          continue;
        } else if (j > 7 - ((i32)byte + 1)) {
          continue;
        }
      } else if (j == 7) {
        tmp |= 1 << b;
        continue;
      } else if (j == 6) {
        continue;
      }

      // Copy the each byte
      u32 bm = VGFX_CHECK_BIT(unicode, rb - 1);
      tmp |= bm << b;

      rb -= 1;

      // If there are no bits left to copy break the loop
      if (rb == 0) {
        break;
      }
    }

    if (rb == 0) {
      break;
    }
  }

  return tmp;
}

void vgfx_utf8_to_cstr(utf8_char c, char *out) {

  VGFX_DEBUG_ASSERT(out, "Can't convert utf8_char, C string is NULL.\n");

  utf8_char r = _vgfx_utf8_reverse_byte_order(c);

  memcpy(out, (&r), 4);
  out[4] = '\0';
}

utf8_char _vgfx_utf8_reverse_byte_order(utf8_char c) {

  utf8_char tmp = 0;
  for (usize i = 0; i < 4; ++i) {
    tmp |= ((c >> (i * 8)) & 0xFF) << ((4 * 8) - 8 - (i * 8));
  }

  return tmp;
}
