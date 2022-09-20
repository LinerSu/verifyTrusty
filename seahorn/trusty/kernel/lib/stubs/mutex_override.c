#include <kernel/mutex.h>
#include <sys/types.h>
#include "nondet.h"
#include "seahorn/seahorn.h"

status_t mutex_acquire(mutex_t *m)
{
    return nd_int();
}

/* does the current thread hold the mutex? */
bool is_mutex_held(mutex_t *m)
{
    return nd_bool();
}

void mutex_init(mutex_t *m) {
  m = (mutex_t *)malloc(sizeof(struct mutex));
}

void mutex_destroy(mutex_t *m) {
  free(m);
}
/* try to acquire the mutex with a timeout value */
status_t mutex_acquire_timeout(mutex_t *m, lk_time_t t) {
  return nd_int();
}

status_t mutex_release(mutex_t *m) {
  return nd_int();
}