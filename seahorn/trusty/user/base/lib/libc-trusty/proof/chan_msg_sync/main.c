#include <seahorn/seahorn.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <uapi/err.h>
#include <nondet.h>

#include "sea_handle_table.h"

/* A unit proof for verifying multple messages send/read
*/
extern void sea_dsa_alias(const void *p, ...);

# define MAX_MSG_SIZE 25
# define NUM_IOV 2

void sync_ipc_send_msg(handle_t c_chan,
                          struct iovec *tx_iovecs,
                          unsigned int tx_iovec_count,
                          struct iovec* rx_iovecs,
                          unsigned int rx_iovec_count) {
  // send a message
  ipc_msg_t snd_msg = {.num_iov = tx_iovec_count, .iov = tx_iovecs};

  ssize_t r = send_msg(c_chan, &snd_msg);
  if (r != NO_ERROR) {
    return;
  }

  /* server side */
  uevent_t event;
  event.handle = INVALID_IPC_HANDLE;
  event.event = 0;
  event.cookie = NULL;
  int rc = wait(c_chan, &event, INFINITE_TIME);
  if (rc == NO_ERROR) {
    if (event.event & IPC_HANDLE_POLL_MSG) {
      ipc_msg_info_t msg_info;
      rc = get_msg(c_chan, &msg_info);
      if (rc != NO_ERROR) {
        return;
      }
      assume(rc == NO_ERROR);
      sassert(msg_info.id != 0);

      ipc_msg_t rcv_msg = {.num_iov = rx_iovec_count, .iov = rx_iovecs};
      r = read_msg(c_chan, msg_info.id, 0, &rcv_msg);
      if (rc != NO_ERROR) {
        return;
      }
      sassert(rc <= MAX_MSG_SIZE);

      /* cleanup */
      rc = put_msg(c_chan, msg_info.id);
      assume(rc == NO_ERROR);
    }
  }
  return;
}

void init_iovecs(struct iovec *iovecs, size_t num_iov) {
  size_t sz = nd_size_t();
  assume(sz >= 1);
  assume(sz <= MAX_MSG_SIZE);
  for (unsigned i = 0; i < num_iov; ++i) {
    uint8_t* buf = (uint8_t*)malloc(sizeof(uint8_t)*sz);
    iovecs[i].iov_base = buf;
    iovecs[i].iov_len = sz;
  }
}

void msg_loop(handle_t c_chan) {
  size_t num_iov = nd_size_t();
  // assume(num_iov >= 1);
  // assume(num_iov <= NUM_IOV);
  num_iov = 1;

  // #pragma unroll 2
  // while (true)
  // {
    struct iovec *iovecs1 = malloc(sizeof(struct iovec)*num_iov);
    init_iovecs(iovecs1, num_iov);

    struct iovec *iovecs2 = malloc(sizeof(struct iovec)*num_iov);
    init_iovecs(iovecs2, num_iov);

    struct iovec *tx_iovecs;
    struct iovec *rx_iovecs;
    // The following way is the same procedure to force iovecs1 and iovecs2
    // share the same DSA node.
    // sea_dsa_alias(iovecs1, iovecs2);
    if (nd_bool()) {
      tx_iovecs = iovecs1;
      rx_iovecs = iovecs2;
    } else {
      tx_iovecs = iovecs2;
      rx_iovecs = iovecs1;
    }

    sync_ipc_send_msg(c_chan, tx_iovecs, num_iov, rx_iovecs, num_iov);
  //   if (nd_bool()) break;
  // }
}

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

  msg_loop(c_chan);

  int rc = close(c_chan);
  sassert(rc == NO_ERROR);

  return 0;
}