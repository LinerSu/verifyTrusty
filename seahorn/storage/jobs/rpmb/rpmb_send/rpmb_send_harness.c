/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <rpmb.h>
#include <rpmb_protocol.h>
#include <uapi/err.h>

#include "ipc.h"

#include "seahorn/seahorn.h"
#include "sea_ipc_helper.h"
#include "sea_handle_table.h"
#define assert sassert

#include "tipc_limits.h"
#include <interface/storage/storage.h>
#include <nondet.h>

#define MAX_ITER_TIME 10000

size_t nd_pos_size_t() {
  size_t res = nd_size_t();
  assume(res > 0);
  return res;
}

/**
   entry point
*/
int main(void) {

  /* server side */
  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(IS_PORT_IPC_HANDLE(port));

  /* client side */
  handle_t c_chan;
  c_chan = connect("ta.seahorn.com", IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
  // check if use a valid connection
  sassert(IS_CHAN_IPC_HANDLE(c_chan));

  struct rpmb_packet *cmd;
  struct rpmb_packet *rcmd;
  struct rpmb_packet *res;
  size_t count =  nd_pos_size_t();
  assume(count < MAX_ITER_TIME);

  #pragma unroll 2
  for (size_t i1 = 0; i1 < count; i1++) {
    size_t i = nd_pos_size_t() * sizeof(struct rpmb_packet);
    size_t j = nd_pos_size_t() * sizeof(struct rpmb_packet);
    assume(i < j);
    cmd = malloc(i);
    rcmd = nd_bool() ? malloc(j) : cmd;
    res = nd_bool() ? malloc(j) : rcmd;
    int ret = rpmb_send(&c_chan, cmd, i, rcmd, i, res, i, nd_bool(), nd_bool());
    if (ret != NO_ERROR && nd_bool()) break;
  }

  return 0;
}
