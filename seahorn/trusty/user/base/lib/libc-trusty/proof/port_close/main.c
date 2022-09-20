#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h> // trusty errors definitions

int main(void) {

  handle_t ports[4];

  ports[0] =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  #ifdef __CRAB__
  sassert(ports[0]);
  #else
  sassert(ports[0] == 2);
  #endif

  ports[1] =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- no more non-secure port handles
  #ifdef __CRAB__
  sassert(ports[1]);
  #else
  sassert(ports[1] == INVALID_IPC_HANDLE);
  #endif

  ports[2] = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- expected secure handle handle
  #ifdef __CRAB__
  sassert(ports[2]);
  #else
  sassert(ports[2] == 1);
  #endif

  ports[3] = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- no more secure handles
  sassert(ports[3] == INVALID_IPC_HANDLE);

  int rc;

  // release handle
  for (int i = 0; i < 4; ++i) {
    rc = close(ports[i]);
    if (ports[i] == INVALID_IPC_HANDLE) {
      sassert(rc < NO_ERROR);
    } else {
      sassert(rc == NO_ERROR);
    }
    ports[i] = INVALID_IPC_HANDLE;
  }

  // check if a new port can be created after close
  ports[0] =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  #ifdef __CRAB__
  sassert(ports[0]);
  #else
  sassert(ports[0] == 2);
  #endif

  return 0;
}
