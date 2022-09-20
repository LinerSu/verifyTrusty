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
#include <stdlib.h>
#include <err.h>
#include <lib/trusty/uio.h>

#include <seahorn/seahorn.h>
#include "nondet.h"

/**
 * membuf_to_user_iovec - Return copied size from memory buffer to user iovec
 * @iov_uaddr: User iovec object (fields: a buffer and a size of the buffer).
 * @iov_cnt:   Number of iovec objects.
 * @buf:       Memory buffer. 
 * @len:       The size of memory buffer.
 *
 * Return: Copied size from a memory buffer to kernel iovec
 * Signature: 
 * ssize_t membuf_to_user_iovec(user_addr_t iov_uaddr,
 *                           uint iov_cnt,
 *                           const uint8_t* buf,
 *                           size_t len)
 */

#define MAX_IOVC_CNT 10
#define MAX_BUFFER_LEN 128
#define MAX_MSG_SIZE 16

void init_iovecs(struct iovec_user *iovecs, size_t num_iov) {
  size_t sz = nd_size_t();
  assume(sz >= 1);
  assume(sz <= MAX_MSG_SIZE);
  for (unsigned i = 0; i < num_iov; ++i) {
    uint8_t* buf = (uint8_t*)malloc(sizeof(uint8_t)*sz);
    iovecs[i].iov_base = buf;
    iovecs[i].iov_len = sz;
  }
}

/**
   entry point
*/
int main(void) {

  uint iov_cnt = nd_uint();
  assume(iov_cnt > 0);
  assume(iov_cnt <= MAX_IOVC_CNT);
  struct iovec_user* iovecs = (struct iovec_user*)malloc(sizeof(struct iovec_user) * iov_cnt);
  init_iovecs(iovecs, iov_cnt);

  size_t len = nd_size_t();
  assume(len <= MAX_BUFFER_LEN);
  uint8_t *buf = len == 0 ? NULL : (uint8_t *)malloc(sizeof(uint8_t) * len);

  ssize_t ret = membuf_to_user_iovec(iovecs, iov_cnt, buf, len);

  if (len == 0) {
    sassert(buf == NULL);
    sassert(ret == 0);
  } else {
    // len > 0;
    sassert(buf != NULL);
    sassert(ret > 0);
    sassert(ret <= (ssize_t)MAX_BUFFER_LEN);
  }

  return 0;
}
