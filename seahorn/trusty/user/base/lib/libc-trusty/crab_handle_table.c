/**
   Implementation of table of handles to be used by IPC
 */

/** Global variables that keep current handle information */

#include "sea_handle_table.h"
#include <nondet.h>
#include <stdlib.h>
#include <seahorn/seahorn.h>

// The number of port handles
#define NUM_PHANDLE 3

// The number of channel handles
#define NUM_CHANDLE 3

/** Globals */
// Define a global collection of handle tables for crab
handle_t port_handles[NUM_PHANDLE] = { NULL, NULL, NULL };
handle_t chan_handles[NUM_CHANDLE] = { NULL, NULL, NULL };

// unsigned num_alloc_phandles = 0;
// unsigned num_alloc_chandles = 0;

unsigned num_active_phandles = 0;
unsigned num_active_chandles = 0;

/**************************** Port operations *************************/
/**
  Init a port handle
*/
void init_port_handle(handle_t ptr, bool secure) {
  ptr->type = IS_PORT;
  ptr->secure = secure;
  ptr->active = true;
  ptr->cookie = NULL;
  ptr->path = 0;
}

/**
   Returns the first port handle that is not active
   make it as active and set the path by the first character of path
   FIXME: the implemantation of path is brittle, we only store / compare
          the first character of path.

   Returns INVALID_IPC_HANDLE if no port handle is available
 */
handle_t sea_ht_new_port(bool secure, const char *path) {
  // create a new port
  if (num_active_phandles < NUM_PHANDLE) {
    handle_t ptr = (handle_t)malloc(sizeof(struct handle));
    init_port_handle(ptr, secure);
    port_handles[num_active_phandles] = ptr;
    num_active_phandles++;
    return ptr;
  }
  // if no port available
  return INVALID_IPC_HANDLE;
}

handle_t sea_ht_match_port(const char *path) {
  if (path) {
    // WARN: nondet return an active port
    int idx = nd_int();
    assume(idx >= 0);
    assume(idx < num_active_phandles);
    handle_t ptr = port_handles[idx];
    if (ptr && ptr->active) {
      return ptr;
    }
  }
  // if no port available
  return INVALID_IPC_HANDLE;
}

int sea_ht_free_port(handle_t ptr) {
  if (ptr) {
    ptr->active = false;
    --num_active_phandles;
    sassert(num_active_phandles >= 0);
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

/**
   Non-deterministically chooses an active port handle

   INVALID_IPC_HANDLE if no active handle is available
 */
handle_t sea_ht_choose_active_port_handle(void) {
  // nondet choose for an active port
  int idx = nd_int();
  assume(idx >= 0 && idx < num_active_phandles);
  handle_t ptr = port_handles[idx];
  if (ptr && ptr->active) {
    return ptr;
  }
  // if no port available
  return INVALID_IPC_HANDLE;
}

bool sea_ht_is_active_port(handle_t ptr) {
  if (ptr) {
    return ptr->active;
  }
  return false;
}

int sea_ht_set_cookie_port(handle_t ptr, void *cookie) {
  if (ptr) {
    ptr->cookie = cookie;
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

void *sea_ht_get_cookie_port(handle_t ptr) {
  if (ptr) {
    return ptr->cookie;
  }
  return NULL;
}

/**************************** Channel operations *************************/
/**
  Init a channel handle
*/
void init_channel_handle(handle_t ptr) {
  ptr->type = IS_CHAN;
  ptr->active = true;
  ptr->cookie = NULL;
  ptr->path = 0;
  ptr->msg_id = 0;
  ptr->msg_len = 0;
}

static handle_t s_first_available_channel_handle(void) {
  // nondet search for a new channel
  int idx = nd_int();
  assume(idx >= 0);
  assume(idx < num_active_chandles);
  handle_t ptr = chan_handles[idx];
  if (ptr && ptr->active) {
    return ptr;
  }

  // if no channel available
  return NULL;
}

handle_t sea_ht_new_channel(handle_t port) {
  // create a new channel
  if (num_active_chandles < NUM_CHANDLE) {
    handle_t ptr = (handle_t)malloc(sizeof(struct handle));
    init_channel_handle(ptr);
    chan_handles[num_active_chandles] = ptr;
    num_active_chandles++;
    return ptr;
  }
  // if no channel available
  return INVALID_IPC_HANDLE;
}

int sea_ht_free_channel(handle_t ptr) {
  if (ptr) {
    ptr->active = false;
    --num_active_chandles;
    sassert(num_active_chandles >= 0);
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

/**
   Non-deterministically chooses an active channel handle

   INVALID_IPC_HANDLE if no active handle is available
 */
handle_t sea_ht_choose_active_channel_handle(void) {
  // nondet choose for an active channel
  int idx = nd_int();
  assume(idx >= 0);
  assume(idx < num_active_chandles);
  handle_t ptr = chan_handles[idx];
  if (ptr && ptr->active) {
    return ptr;
  }
  return INVALID_IPC_HANDLE;
}

int sea_ht_set_cookie_channel(handle_t ptr, void *cookie) {
  if (ptr) {
    ptr->cookie = cookie;
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

void *sea_ht_get_cookie_channel(handle_t ptr) {
  if (ptr) {
    return ptr->cookie;
  }
  return NULL;
}

bool sea_ht_has_msg(handle_t ptr) {
  if (ptr) {
    return ptr->msg_id > INVALID_IPC_MSG_ID;
  }
  return false;
}

uint32_t sea_ht_get_msg_id(handle_t ptr) {
  if (ptr) {
    return ptr->msg_id;
  }
  return INVALID_IPC_MSG_ID;
}

void sea_ht_set_msg_id(handle_t ptr, uint32_t id) {
  if (ptr) {
    ptr->msg_id = id;
    return;
  }
}

size_t sea_ht_get_msg_len(handle_t ptr) {
  if (ptr) {
    return ptr->msg_len;
  }
  // return 0 length if handle not found
  return 0;
}

void sea_ht_set_msg_len(handle_t ptr, size_t len) {
  if (ptr) {
    ptr->msg_len = len;
    return;
  }
}

void sea_ht_new_nd_msg(handle_t ptr) {
  if (ptr) {
    ptr->msg_id = nd_msg_id();
    ptr->msg_len = nd_msg_len();
    assume(ptr->msg_id > INVALID_IPC_MSG_ID);
    return;
  }
}

/*************************** Miscellaneous operations *************************/
int sea_ht_set_cookie(handle_t handle, void *cookie) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_set_cookie_port(handle, cookie);
  } else if (IS_CHAN_IPC_HANDLE(handle)) {
    return sea_ht_set_cookie_channel(handle, cookie);
  } else {
    return ERR_BAD_HANDLE;
  }
}

void *sea_ht_get_cookie(handle_t handle) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_get_cookie_port(handle);
  } else if (IS_CHAN_IPC_HANDLE(handle)) {
    return sea_ht_get_cookie_channel(handle);
  } else {
    return NULL;
  }
}

int sea_ht_free(handle_t handle) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_free_port(handle);
  } else if (IS_CHAN_IPC_HANDLE(handle)) {
    return sea_ht_free_channel(handle);
  } else {
    return ERR_BAD_HANDLE;
  }
}

handle_t sea_ht_choose_active_handle(void) {
  // Non determinally choose from a port / channel
  bool b = nd_bool();
  handle_t handle = b ? sea_ht_choose_active_port_handle() : sea_ht_choose_active_channel_handle();
  if (handle == INVALID_IPC_HANDLE) {
    handle = !b ? sea_ht_choose_active_port_handle() : sea_ht_choose_active_channel_handle();
  }
  return handle;
}