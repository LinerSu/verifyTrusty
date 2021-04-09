// RUN: %sea bpf -m64 -O3 --entry=handle_msg --bmc=mono --horn-bv2=true --inline  --horn-bv2-ptr-size=8 --horn-bv2-word-size=8 --log=opsem "%s" 2>&1 | OutputCheck %s
// CHECK-L: unsat
/*
 * Based on ipc msg handler code from /trusty/app/keymaster/ipc/keymaster_ipc.cpp 
 */


#include "seahorn/seahorn.h"
#include "keymaster_ipc.h"

#include <lk/macros.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

#include <interface/keymaster/keymaster.h>

#include <keymaster/UniquePtr.h>

#include "trusty_keymaster.h"
#include "trusty_logger.h"
#include "nondet.h"

using namespace keymaster;
extern TrustyKeymaster* device;

struct keymaster_srv_ctx {
    handle_t port_secure;
    handle_t port_non_secure;
};

long keymaster_ipc_init(keymaster_srv_ctx* ctx);

void dispatch_event(const uevent_t* ev);

int main(void) {
    long rc;
    uevent_t event;

    device = new TrustyKeymaster(new TrustyKeymasterContext, 16);

    TrustyLogger::initialize();

    LOG_I("Initializing", 0);

    keymaster_srv_ctx ctx;
    rc = keymaster_ipc_init(&ctx);
    if (rc < 0) {
        LOG_E("failed (%d) to initialize keymaster", rc);
        return rc;
    }

    /* enter main event loop */
    // Unroll the while loop only iterating twice
    int unroll_time = 0;
    while (unroll_time < 2) {
        event.handle = INVALID_IPC_HANDLE;
        event.event = 0;
        event.cookie = NULL;

        rc = wait_any(&event, INFINITE_TIME);
        if (rc < 0) {
            LOG_E("wait_any failed (%d)", rc);
            break;
        }

        if (rc == NO_ERROR) { /* got an event */
            dispatch_event(&event);
        }
        unroll_time ++;
    }

    return 0;
}

