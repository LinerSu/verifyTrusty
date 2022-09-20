#include <seahorn/seahorn.h>
#include <trusty_ipc.h>

int main(void) {

  handle_t h =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  #ifdef __CRAB__
  sassert(h);
  #else
  sassert(h == 2);
  #endif

  handle_t h2 =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- no more non-secure port handles
  #ifdef __CRAB__
  sassert(h2);
  #else
  sassert(h2 == INVALID_IPC_HANDLE);
  #endif

  h2 = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- expected secure handle handle
  #ifdef __CRAB__
  sassert(h2);
  #else
  sassert(h2 == 1);
  #endif

  handle_t h3 = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- no more secure handles
  #ifdef __CRAB__
  sassert(h3 == INVALID_IPC_HANDLE);
  #else
  sassert(h3 == INVALID_IPC_HANDLE);
  #endif

  // release handle
  close(h);
  h = INVALID_IPC_HANDLE;

  // request again
  h = port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);
  #ifdef __CRAB__
  sassert(h);
  #else
  sassert(h == 2);
  #endif

  return 0;
}
