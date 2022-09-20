#include <ipc.h>
#include <nondet.h>
#include <sea_handle_table.h>
#include <sea_ipc_helper.h>
#include <seahorn/seahorn.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

extern void handle_port(struct ipc_context *ctx, const struct uevent *ev);
extern void handle_channel(struct ipc_context *ctx, const struct uevent *ev);

void pretend_message_sent(handle_t chan_handle) {
  sassert(IS_CHAN_IPC_HANDLE(chan_handle));
  // pretend a message send
  uint8_t snd_buf[32];
  struct iovec snd_iov = {.iov_base = snd_buf, .iov_len = sizeof(snd_buf)};
  ipc_msg_t snd_msg = {.num_iov = 1, .iov = &snd_iov, .num_handles = 1, .handles = NULL};

  ssize_t rc = send_msg(chan_handle, &snd_msg);
  sassert(rc == 32);
}

void sea_dispatch_event(const uevent_t *ev) {
  sassert(ev);

  if (ev->event == IPC_HANDLE_POLL_NONE) {
    return;
  }

  struct ipc_context *context = ev->cookie;
  sassert(context);
  sassert(context->evt_handler);
  sassert(context->handle == ev->handle);
  sassert(context->handle != INVALID_IPC_HANDLE);

#ifdef __CRAB__
  if (IS_PORT_IPC_HANDLE(context->handle)) {
    handle_port(context, ev);
  } else {
    pretend_message_sent(context->handle);
    handle_channel(context, ev);
  }
#else
  context->evt_handler(context, ev);
#endif
}

static void sea_ipc_disconnect_handler(struct ipc_channel_context *context) {
  if (context)
    free(context);
}

static void init_iovecs(struct iovec *iovecs, size_t num_iov, size_t msg_size) {
  for (unsigned i = 0; i < num_iov; ++i) {
    iovecs[i].iov_base = malloc(msg_size);
    iovecs[i].iov_len = msg_size;
  }
}

static int sea_ipc_msg_handler(struct ipc_channel_context *context, void *msg,
                               size_t msg_size) {
  sassert(msg_size <= MSG_BUF_MAX_SIZE);
  size_t new_msg_size = nd_size_t();
  assume(new_msg_size <= msg_size);
  struct iovec *iov = malloc(sizeof(struct iovec));
  init_iovecs(iov, 1, new_msg_size);
  ipc_msg_t i_msg = {
      .iov = iov,
      .num_iov = 1,
  };
  int rc = send_msg(context->common.handle, &i_msg);
  if (rc < 0) {
    return rc;
  }
  return NO_ERROR;
}

static int sync_ipc_msg_handler(struct ipc_channel_context *context, void *msg,
                                size_t msg_size) {
  size_t num_iov = nd_size_t();
  num_iov = 2;

  size_t new_msg_size = nd_size_t();
  assume(new_msg_size <= msg_size);
  assume(new_msg_size > 0);
  struct iovec *iovecs1 = malloc(sizeof(struct iovec) * num_iov);
  init_iovecs(iovecs1, num_iov, new_msg_size);

  struct iovec *iovecs2 = malloc(sizeof(struct iovec) * num_iov);
  init_iovecs(iovecs2, num_iov, new_msg_size);

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

  int rc = sync_ipc_send_msg(context->common.handle, tx_iovecs, num_iov,
                                  rx_iovecs, num_iov);

  if (rc < 0)
    return rc;

  if (rc > 0)
    sassert(rc <= num_iov * new_msg_size);

  return NO_ERROR;
}

/*
 * directly return a channel context given uuid and chan handle
 */
static struct ipc_channel_context *
sea_channel_connect(struct ipc_port_context *parent_ctx,
                    const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sea_ipc_msg_handler;
  return pctx;
}

/*
 * directly return a channel context given uuid and chan handle
 */
struct ipc_channel_context *
sea_sync_channel_connect(struct ipc_port_context *parent_ctx,
                         const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sync_ipc_msg_handler;
  return pctx;
}

/*
 * constant variable of ipc_port_context
 */
static struct ipc_port_context port_ctx = {
    .ops = {.on_connect = sea_channel_connect,
            ._on_connect = sea_channel_connect},
};

struct ipc_port_context *create_port_context() {
  return &port_ctx;
}