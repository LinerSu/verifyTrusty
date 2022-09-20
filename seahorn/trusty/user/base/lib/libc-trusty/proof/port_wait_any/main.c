#include "sea_handle_table.h"
#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

int main(void) {

  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  #ifdef __CRAB__
  sassert(port);
  #else
  sassert(port == 2);
  #endif

  uevent_t event = {.handle = INVALID_IPC_HANDLE, .event = 0, .cookie = NULL};

  int rc = wait_any(&event, 0);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    /* got an event */
    sassert(event.handle == port);
    // No cookie set
    sassert(!event.cookie);
  }

  uuid_t peer_uuid;
  handle_t chan = accept(port, &peer_uuid);
  if (chan == INVALID_IPC_HANDLE) {
    return 0;
  }
  sassert(IS_CHAN_IPC_HANDLE(chan));

  // send a message
  uint8_t snd_buf[32];
  struct iovec snd_iov = {.iov_base = snd_buf, .iov_len = sizeof(snd_buf)};
  ipc_msg_t snd_msg = {.num_iov = 1, .iov = &snd_iov, .num_handles = 1, .handles = NULL};

  ssize_t r = send_msg(chan, &snd_msg);
  assume(r >= 0);

  rc = wait_any(&event, 0);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    if (event.handle == chan && (event.event & IPC_HANDLE_POLL_MSG) > 0) {
      struct ipc_msg_info msg_info;
      rc = get_msg(chan, &msg_info);
      sassert(rc <= NO_ERROR);
      if (rc == NO_ERROR) {
        sassert(msg_info.id != 0);

        char buf[4096];
        struct iovec iov = {.iov_base = buf, .iov_len = sizeof(buf)};
        struct ipc_msg msg = {.num_iov = 1, .iov = &iov};
        r = read_msg(chan, msg_info.id, 0, &msg);
        assume(r >= NO_ERROR);
        sassert(r <= 32);

        rc = put_msg(chan, msg_info.id);
        assume(rc == NO_ERROR);
        rc = read_msg(chan, msg_info.id, 0, &msg);
        sassert(rc < NO_ERROR);
      }
    }
  }

  return 0;
}
