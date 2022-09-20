#include "nondet.h"
#include "seahorn/seahorn.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <compiler.h>

#include <err.h>   // trusty errors definitions
#include <arch/usercopy.h> // trusty arch user copy definitions

/*
status_t arch_copy_to_user(user_addr_t udest, const void *ksrc, size_t len)
  // Disassemble the following arm assembly code
  cbz	x2, .Larch_copy_to_user_done
.Larch_copy_to_user_loop:
  ldrb	w9, [x1], #1

  set_fault_handler	.Larch_copy_to_user_fault
  sttrb	w9, [x0]

  add	x0, x0, #1
  subs	x2, x2, #1
  b.hi	.Larch_copy_to_user_loop
.Larch_copy_to_user_done:
  mov	x0, #0
  ret
 */

status_t arch_copy_to_user(user_addr_t udest, const void *ksrc, size_t len) {
  if (len == 0) {
    return 0;
  }
  if ((udest != NULL) && (ksrc != NULL)) {
    sassert(sea_is_dereferenceable(udest, len));
    sassert(sea_is_dereferenceable(ksrc, len));
    __builtin_memcpy(udest, ksrc, len);
    return len;
  }
  return 0;
}