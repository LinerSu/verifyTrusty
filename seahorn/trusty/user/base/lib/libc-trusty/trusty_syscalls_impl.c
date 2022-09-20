#include "sea_handle_table.h"
#include "trusty_syscalls.h"

#include <nondet.h>
#include <seahorn/seahorn.h>
#include <uapi/err.h> // trusty errors definitions

handle_t _trusty_port_create(const char *path, uint32_t num_recv_bufs,
                             uint32_t recv_buf_size, uint32_t flags) {
  (void)path;
  (void)num_recv_bufs;
  (void)recv_buf_size;

  if (!path) return ERR_BAD_PATH;

  bool secure = (flags & IPC_PORT_ALLOW_NS_CONNECT) &&
                !(flags & IPC_PORT_ALLOW_TA_CONNECT);
  return sea_ht_new_port(secure, path);
}

handle_t _trusty_connect(const char *path, uint32_t flags) {
  handle_t port_handle = sea_ht_math_port(path);
  if (port_handle != INVALID_IPC_HANDLE) {
    return sea_ht_new_channel(port_handle);
  }
  return INVALID_IPC_HANDLE;
}

handle_t _trusty_accept(handle_t port_handle, uuid_t *peer_uuid) {
  (void)port_handle;
  handle_t chan = sea_ht_new_channel(port_handle);
  if (chan != INVALID_IPC_HANDLE) {
    if ((port_handle & IPC_PORT_ALLOW_TA_CONNECT) == 0) {
      // non-secure world
      peer_uuid->time_low = 0;
      peer_uuid->time_mid = 0;
      peer_uuid->time_hi_and_version = 0;
    }
    else {
      // define peer_uuid to a dummy value
      peer_uuid->time_low = nd_time_low();
      peer_uuid->time_mid = nd_time_mid();
      peer_uuid->time_hi_and_version = nd_time_hi_n_ver();
    }
  }
  return chan;
}

int _trusty_close(handle_t handle) {
  if (handle == INVALID_IPC_HANDLE) {
    return ERR_BAD_HANDLE;
  }
  sea_ht_free(handle);
  return NO_ERROR;
}

int _trusty_set_cookie(handle_t handle, void *cookie) {
  sea_ht_set_cookie(handle, cookie);
  return NO_ERROR;
}

handle_t _trusty_handle_set_create(void) { return INVALID_IPC_HANDLE; }

int _trusty_handle_set_ctrl(handle_t handle, uint32_t cmd, struct uevent *evt) {
  return ERR_GENERIC;
}

int _trusty_wait(handle_t handle, struct uevent *event,
                 uint32_t timeout_msecs) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  event->handle = handle;
  event->cookie = sea_ht_get_cookie(handle);
  event->event = nd_trusty_ipc_event();

  if (IS_PORT_IPC_HANDLE(handle)) {
    event->event = IPC_HANDLE_POLL_READY;
  }

  if (event->event & IPC_HANDLE_POLL_MSG) {
    // IPC_HANDLE_POLL_MSG indicates that
    // there is a pending message for this channel
    if (sea_ht_get_msg_id(handle) == INVALID_IPC_MSG_ID) {
      // pretend a message sent
      sea_ht_new_nd_msg(handle);
    }
  }

  return NO_ERROR;
}

int _trusty_wait_any(uevent_t *ev, uint32_t timeout_msecs) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  handle_t h = sea_ht_choose_active_handle();
  if (h == INVALID_IPC_HANDLE) {
    return h;
  }
  ev->handle = h;
  ev->cookie = sea_ht_get_cookie(h);
  ev->event = nd_trusty_ipc_event();

  if (IS_PORT_IPC_HANDLE(h)) {
    ev->event = IPC_HANDLE_POLL_READY;
  }

  if (ev->event & IPC_HANDLE_POLL_MSG) {
    // IPC_HANDLE_POLL_MSG indicates that
    // there is a pending message for this channel
    if (sea_ht_get_msg_id(h) == INVALID_IPC_MSG_ID) {
      // pretend a message sent
      sea_ht_new_nd_msg(h);
    }
  }

  return NO_ERROR;
}

int _trusty_get_msg(handle_t handle, struct ipc_msg_info *msg_info) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;
  if (!msg_info) {
    return ERR_GENERIC;
  }
  if (sea_ht_get_msg_id(handle) == INVALID_IPC_MSG_ID) {
    return ERR_NO_MSG;
  }

  msg_info->id = sea_ht_get_msg_id(handle);
  msg_info->len = sea_ht_get_msg_len(handle);

  return msg_info->id != INVALID_IPC_MSG_ID ? NO_ERROR : ERR_GENERIC;
}

ssize_t _trusty_read_msg(handle_t handle, uint32_t msg_id, uint32_t offset,
                         struct ipc_msg *msg) {
  if (sea_ht_get_msg_id(handle) != msg_id)
    return ERR_GENERIC;

  if (!msg)
    return ERR_GENERIC;

  if (sea_ht_get_msg_id(handle) == INVALID_IPC_MSG_ID) {
    return ERR_NO_MSG;
  }

  size_t msg_len = sea_ht_get_msg_len(handle);
  if (msg_len <= 0)
    return ERR_GENERIC;
  sassert(offset <= msg_len);

  msg_len -= offset;

  sassert(1 <= msg->num_iov);
  sassert(msg->num_iov <= MAX_IPC_MSG_NUM);
  size_t num_bytes_read = 0;
  for (size_t i = 0; i < msg->num_iov; ++i) {
    if (msg_len < msg->iov[i].iov_len) {
      sassert(sea_is_dereferenceable(msg->iov[i].iov_base, msg_len));
      memhavoc(msg->iov[i].iov_base, msg_len);
      num_bytes_read += msg_len;
      return num_bytes_read;
    }
    sassert(sea_is_dereferenceable(msg->iov[i].iov_base, msg->iov[i].iov_len));
    memhavoc(msg->iov[i].iov_base, msg->iov[i].iov_len);
    num_bytes_read += msg->iov[i].iov_len;
    // avoid msg_len underflow
    if (msg->iov[i].iov_len < msg_len) {
      msg_len -= msg->iov[i].iov_len;
    } else {
      msg_len = 0;
    }
  }

  return num_bytes_read;
}

int _trusty_put_msg(handle_t handle, uint32_t msg_id) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  sea_ht_set_msg_id(handle, INVALID_IPC_MSG_ID);
  sea_ht_set_msg_len(handle, 0);
  return NO_ERROR;
}

ssize_t _trusty_send_msg(handle_t handle, struct ipc_msg *msg) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;
  if (!msg) {
    return ERR_GENERIC;
  }

  sassert(1 <= msg->num_iov);
  sassert(msg->num_iov <= MAX_IPC_MSG_NUM);

  size_t sent_bytes = 0;
  sea_ht_new_nd_msg(handle);
  for (size_t i = 0; i < msg->num_iov; ++i) {
    sent_bytes += msg->iov[i].iov_len;
  }
  sea_ht_set_msg_len(handle, sent_bytes);
  return sent_bytes;
}
