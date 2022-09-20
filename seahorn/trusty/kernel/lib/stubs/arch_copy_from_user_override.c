#include "nondet.h"
#include "seahorn/seahorn.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <compiler.h>

#include <err.h>   // trusty errors definitions
#include <arch/usercopy.h> // trusty arch user copy definitions

/* 
status_t arch_copy_from_user(void *kdest, user_addr_t usrc, size_t len)
  // Disassemble the following arm assembly code
  cbz	x2, .Larch_copy_from_user_done
.Larch_copy_from_user_loop:
  set_fault_handler	.Larch_copy_from_user_fault
  ldtrb	w9, [x1]

  add	x1, x1, #1
  strb	w9, [x0], #1
  subs	x2, x2, #1
  b.hi	.Larch_copy_from_user_loop
.Larch_copy_from_user_done:
  mov	x0, #0
  ret
 */

status_t arch_copy_from_user(void *kdest, user_addr_t usrc, size_t len) {
  if (len == 0) {
    return 0;
  }
  if ((kdest != NULL) && (usrc != NULL)) {
    sassert(sea_is_dereferenceable(kdest, len));
    sassert(sea_is_dereferenceable((void *)usrc, len));
    __builtin_memcpy(kdest, (void *)usrc, len);
    return len;
  }
  return 0;
}