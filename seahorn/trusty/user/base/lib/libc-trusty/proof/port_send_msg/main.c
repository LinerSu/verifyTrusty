#include "sea_handle_table.h"
#include <nondet.h>
#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h>
/* Documentation from trusty API:
  send_msg()
Sends a message over a specified channel.

long send_msg(uint32_t handle, ipc_msg_t *msg);
[in] handle: Handle to the channel over which to send the message.
[in] msg: Pointer to the ipc_msg_t structure describing the message.

[retval]: Total number of bytes sent on success; a negative error otherwise.
*/

# define MAX_ECHO_MSG_SIZE 64

int main(void) {

  /* server side */
  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  #ifdef __CRAB__
  sassert(port);
  #else
  sassert(port == 2);
  #endif

  /* client side */
  handle_t c_chan;
  c_chan = connect("ta.seahorn.com", IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
  // check if use a valid connection
  sassert(IS_CHAN_IPC_HANDLE(c_chan));

  // send a message
  uint8_t snd_buf[32];
  struct iovec snd_iov = {.iov_base = snd_buf, .iov_len = sizeof(snd_buf)};
  ipc_msg_t snd_msg = {.num_iov = 1, .iov = &snd_iov, .num_handles = 1, .handles = NULL};

  ssize_t rc = send_msg(c_chan, &snd_msg);
  assume(rc >= 0);

  /* server side */
  uevent_t event = {.handle = INVALID_IPC_HANDLE, .event = 0, .cookie = NULL};
  int wait_rc = wait(port, &event, INFINITE_TIME);
  sassert(wait_rc <= NO_ERROR);
  if (wait_rc == NO_ERROR) {
    /* got an event */
    sassert(event.handle == port);
    // No cookie set
    sassert(!event.cookie);

    // handle port event
    uuid_t peer_uuid;
    if (event.event & IPC_HANDLE_POLL_READY) {
      handle_t chan = accept(port, &peer_uuid);
      assume(chan != INVALID_IPC_HANDLE);
      sassert(IS_CHAN_IPC_HANDLE(chan));

      uevent_t cev;
      cev.handle = INVALID_IPC_HANDLE;
      cev.event = 0;
      cev.cookie = NULL;

      wait_rc = wait(chan, &cev, INFINITE_TIME);
      sassert(wait_rc <= NO_ERROR);
      if (wait_rc == NO_ERROR) {
        if (cev.event & IPC_HANDLE_POLL_MSG) {
          ipc_msg_info_t msg_info;
          wait_rc = get_msg(chan, &msg_info);
          assume(wait_rc == NO_ERROR);
          sassert(msg_info.id != 0);

          uint8_t rcv_buf[MAX_ECHO_MSG_SIZE];
          struct iovec rcv_iov = {.iov_base = rcv_buf, .iov_len = sizeof(rcv_buf)};
          ipc_msg_t rcv_msg = {.num_iov = 1, .iov = &rcv_iov, .num_handles = 1, .handles = NULL};
          rc = read_msg(chan, msg_info.id, 0, &rcv_msg);
          assume(rc >= NO_ERROR);
          sassert(rc <= MAX_ECHO_MSG_SIZE);

          /* cleanup */
          rc = put_msg(chan, msg_info.id);
          assume(rc == NO_ERROR);
          rc = close(chan);
          sassert(rc == NO_ERROR);
        }
      }
    }
  }

  

  return 0;
}
