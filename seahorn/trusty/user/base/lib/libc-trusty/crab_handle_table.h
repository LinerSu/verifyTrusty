#ifndef _CRAB_HANDLE_TABLE_H_
#define _CRAB_HANDLE_TABLE_H_

#include <stdbool.h>
#include "trusty_ipc.h"

#define IS_PORT_IPC_HANDLE(h) (h->type == IS_PORT)
#define IS_CHAN_IPC_HANDLE(h) (h->type == IS_CHAN)

#define INVALID_IPC_MSG_ID 0
#define MAX_IPC_MSG_NUM 10

handle_t sea_ht_new_port(bool secure, const char *path);
handle_t sea_ht_new_channel(handle_t port);

handle_t sea_ht_choose_active_handle(void);

bool sea_ht_is_active_port(handle_t handle);

void *sea_ht_get_cookie_port(handle_t handle);
void *sea_ht_get_cookie_channel(handle_t handle);
void *sea_ht_get_cookie(handle_t handle);

int sea_ht_set_cookie_port(handle_t handle, void *cookie);
int sea_ht_set_cookie_channel(handle_t handle, void *cookie);
int sea_ht_set_cookie(handle_t handle, void *cookie);
int sea_ht_free(handle_t handle);

bool sea_ht_has_msg(handle_t chan_handle);
uint32_t sea_ht_get_msg_id(handle_t chan_handle);
void sea_ht_set_msg_id(handle_t chan_handle, uint32_t id);
size_t sea_ht_get_msg_len(handle_t chan_handle);
void sea_ht_set_msg_len(handle_t chan_handle, size_t len);
void sea_ht_new_nd_msg(handle_t chan_handle);

handle_t sea_ht_match_port(const char *path);
#endif