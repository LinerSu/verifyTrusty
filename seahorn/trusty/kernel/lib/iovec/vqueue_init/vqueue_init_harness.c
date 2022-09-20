/*
 * Copyright (c) 2013, Google, Inc. All rights reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <assert.h>
#include <err.h>
#include <lib/trusty/uio.h>

#include <arch/arch_ops.h>
#include <kernel/vm.h>
#include <vqueue.h>
#include <lib/trusty/uio.h>

#include <seahorn/seahorn.h>
#include "nondet.h"

/**
 * vqueue_init - Return 0 if no error; otherwise, a negative value (see err.h).
 * @vq:            Virtual memory queue.
 * @id:            The vqueue id.
 * @client_id:     Id of external entity where the memory originated. 
 * @shared_mem_id: Id of shared memory opbject to lookup and return.
 * @num:           Number of memory blocks in vring.
 * @align:         Alignment for contiguous allocation.
 * @priv:          A private pointer for the parent driver.
 * @notify_cb:     A method when the vq is kicked *from* the other side.
 * @kick_cb:       A method when the vq kicks the other side.
 *
 * Return: 0 if no error; otherwise, a negative value (see err.h).
 * Signature: 
 * int vqueue_init(struct vqueue* vq,
 *              uint32_t id,
 *              ext_mem_client_id_t client_id,
 *              ext_mem_obj_id_t shared_mem_id,
 *              uint num,
 *              ulong align,
 *              void* priv,
 *              vqueue_cb_t notify_cb,
 *              vqueue_cb_t kick_cb);
 */

#define MAX_RING_NUM 10
#define MAX_BUFFER_LEN 128
#define MAX_MSG_SIZE 16

static int vq_notify_cb(struct vqueue* vq, void* priv) {
    return 0;
}

static int vq_kick_cb(struct vqueue* vq, void* priv) {
    return 0;
}

/**
   entry point
*/
int main(void) {

  struct vqueue* vq = (struct vqueue*)malloc(sizeof(struct vqueue));
  uint32_t num = nd_uint32_t();
  assume(num > 0);
  assume(num <= MAX_RING_NUM);
  uint32_t id = nd_uint32_t();
  ext_mem_client_id_t client_id = nd_uint64_t();
  ext_mem_obj_id_t shared_mem_id = nd_uint64_t();
  size_t sz = nd_size_t();
  assume(sz > 0);
  void *priv = malloc(sz);
  vqueue_init(vq, id, client_id, shared_mem_id, num, 0, priv, &vq_notify_cb, &vq_kick_cb);

  return 0;
}
