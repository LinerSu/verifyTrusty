#include <seahorn/seahorn.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#define INLINE __attribute__((always_inline))

#define MAX_BUFFER_SIZE 10

INLINE int bcmp(const void *s1, const void *s2, size_t n) {
  size_t i;

  size_t max_buffer_size = MAX_BUFFER_SIZE;
  const uint8_t *p1;
  const uint8_t *p2;
  p1 = s1;
  p2 = s2;

  if (p1 == p2)
    return 0;
  if (p1 == NULL || p2 == NULL)
    return 1;
  /* pre-unroll the loop for MAX_BUFFER_SIZE */
  for (i = 0; i < max_buffer_size; i++) {
    if (i < n) {
      if (p1[i] != p2[i]) {
        return 1;
      }
    }
  }
  /* unroll the rest, if any */
  for (i = max_buffer_size; i < n; i++) {
    if (p1[i] != p2[i])
      return 1;
  }

  return 0;
}

INLINE int memcmp(const void *s1, const void *s2, size_t n) {
  size_t i;
  size_t max_buffer_size = MAX_BUFFER_SIZE;

  const uint8_t *p1;
  const uint8_t *p2;
  p1 = s1;
  p2 = s2;

  if (p1 == p2)
    return 0;

  /* pre-unroll the loop for MAX_BUFFER_SIZE */
  for (i = 0; i < max_buffer_size; i++) {
    if (i < n) {
      if (p1[i] != p2[i]) {
        return p1[i] < p2[i] ? -1 : 1;
      }
    }
  }
  /* unroll the rest, if any */
  for (i = max_buffer_size; i < n; i++) {
    if (p1[i] != p2[i])
      return p1[i] < p2[i] ? -1 : 1;
  }

  return 0;
}

INLINE void *__memcpy_chk(void *dest, const void *src, size_t len,
                          size_t dstlen) {
  sassert(!(dstlen < len));
  return __builtin_memcpy(dest, src, len);
}

INLINE void *__memset_chk(void *dest, int c, size_t len, size_t dstlen) {
  sassert(!(dstlen < len));
  return __builtin_memset(dest, c, len);
}