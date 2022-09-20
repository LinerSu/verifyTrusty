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
#include <uapi/err.h>

#include "ipc.h"

#include "seahorn/seahorn.h"
#include "sea_ipc_helper.h"
#include "sea_handle_table.h"
#define assert sassert

#include "tipc_limits.h"
#include <interface/storage/storage.h>
#include <nondet.h>

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

  // precond: rpmb_init requires a handle channel for msg send / read
  struct rpmb_state* state = NULL;
  /* init rpmb */
  rpmb_init(&state, &c_chan);
  struct rpmb_key key;
  memhavoc(&key.byte, sizeof(uint8_t)*32);

  rpmb_set_key(state, &key);

  // task
  rpmb_program_key(state, &key);

  // free memory
  rpmb_uninit(state);
  return 0;
}
