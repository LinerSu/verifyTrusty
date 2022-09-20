#include "rpmb.h"
#include "rpmb_protocol.h"
#include <nondet.h>
#include <seahorn/seahorn.h>

/**
 * FUNCTION: rpmb_mac
 *
 * This function overrides the original implementation of the
 * rpmb_mac function from rpmb.c with a no_op.
 *
 * The original code is used to prove the message authetication
 * by given a message read from IPC channel.
 * The openSSL HMAC is used for computing hash code for message authentication.
 * 
 * It is necessary to stub out this function because openSSL is a library
 * without bitcode level compilation (although we can compile it through clang).
 * 
 * It is safe to stub out the following function because we donnot check the
 * the value of message but we CARE the memory safety.
 */

int rpmb_mac(struct rpmb_key key, struct rpmb_packet* packet,
             size_t packet_count, struct rpmb_key* mac) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;
  
  sassert(packet);
  sassert(mac);

  for (size_t i = 0; i < packet_count; i++) {
    sassert(sea_is_dereferenceable(packet[i].data, 284));
  }

  // HMAC_Final places the authentication code in mac->byte
  memhavoc(mac->byte, sizeof(mac->byte));
  return NO_ERROR;
}