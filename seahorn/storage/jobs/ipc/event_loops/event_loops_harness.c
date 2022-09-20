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
#include <uapi/err.h>

#include "ipc.h"

#include "handle_table.h"
#include "seahorn/seahorn.h"
#include "sea_ipc_helper.h"
#define assert sassert

#include "tipc_limits.h"
#include <interface/storage/storage.h>

static void* msg_buf;
/*
 *  Message handler
 */
static int handle_msg(handle_t chan) {
  int rc;
  struct iovec iov;
  ipc_msg_t msg;
  ipc_msg_info_t msg_inf;

  iov.iov_base = msg_buf;
  iov.iov_len = sizeof(msg_buf);

  msg.num_iov = 1;
  msg.iov = &iov;
  msg.num_handles = 0;
  msg.handles = NULL;

  /* get message info */
  rc = get_msg(chan, &msg_inf);
  if (rc == ERR_NO_MSG)
    return NO_ERROR; /* no new messages */

  if (rc != NO_ERROR) {
    return rc;
  }

  /* read msg content */
  rc = read_msg(chan, msg_inf.id, 0, &msg);
  if (rc < 0) {
    return rc;
  }

  /* update number of bytes received */
  iov.iov_len = (size_t) rc;

  /* send message back to the caller */
  rc = send_msg(chan, &msg);
  if (rc < 0) {
    return rc;
  }

  /* retire message */
  rc = put_msg(chan, msg_inf.id);
  if (rc != NO_ERROR) {
    return rc;
  }

  return NO_ERROR;
}

/*
 *  Channel event handler
 */
static void handle_channel_event(const uevent_t * ev) {
  int rc;

  if (ev->event & IPC_HANDLE_POLL_MSG) {
    rc = handle_msg(ev->handle);
    if (rc != NO_ERROR) {
      /* report an error and close channel */
      close(ev->handle);
    }
    return;
  }
  if (ev->event & IPC_HANDLE_POLL_HUP) {
    /* closed by peer. */
    close(ev->handle);
    return;
  }
}

/*
 *  Port event handler
 */
static void handle_port_event(const uevent_t * ev) {
  uuid_t peer_uuid;
  handle_t chan_handle;

  if ((ev->event & IPC_HANDLE_POLL_ERROR) ||
    (ev->event & IPC_HANDLE_POLL_HUP) ||
    (ev->event & IPC_HANDLE_POLL_MSG) ||
    (ev->event & IPC_HANDLE_POLL_SEND_UNBLOCKED)) {
    /* should never happen with port handles */
    return;
  }
  if (ev->event & IPC_HANDLE_POLL_READY) {
    /* incoming connection: accept it */
    int rc = accept(ev->handle, &peer_uuid);
    if (rc < 0) {
      return;
    }
    handle_t chan = (handle_t) rc;
    while (true){
      struct uevent cev;
      cev.handle = INVALID_IPC_HANDLE;
      cev.event = 0;
      cev.cookie = NULL;

      rc = wait(chan, &cev, INFINITE_TIME);
      if (rc < 0) {
         return;
      }
      handle_channel_event(&cev);
      if (cev.event & IPC_HANDLE_POLL_HUP) {
        return;
      }
    }
  }
  return;
}

/**
   entry point
*/
int main(void) {
  // handle_table_init(INVALID_IPC_HANDLE, INVALID_IPC_HANDLE, INVALID_IPC_HANDLE);
  int rc = port_create(STORAGE_DISK_PROXY_PORT, 1, STORAGE_MAX_BUFFER_SIZE,
                      IPC_PORT_ALLOW_TA_CONNECT | IPC_PORT_ALLOW_NS_CONNECT);

  if (rc < 0) {
    return rc;
  }

  handle_t port = (handle_t) rc;

  /* enter main event loop */
  while (true) {
    uevent_t ev;

    ev.handle = INVALID_IPC_HANDLE;
    ev.event = 0;
    ev.cookie = NULL;

    /* wait forever */
    rc = wait(port, &ev, INFINITE_TIME);
    if (rc == NO_ERROR) {
      /* got an event */
      handle_port_event(&ev);
    } else {
      break;
    }
  }

  // ipc_port_destroy(ctx);
  return 0;
}
