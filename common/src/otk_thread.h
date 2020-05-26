#ifndef OTK_THREAD_H
#define OTK_THREAD_H

// Headers for all Standard Library facilities.
#ifndef _WIN32
#include <pthread.h>
#endif
#include <stdio.h>
#include <time.h>

// Third party library headers.
#ifdef _WIN32
#include "uv.h"
#endif

// OTKit headers.

#ifndef _WIN32
#define otk_thread_t pthread_t
typedef struct otk_thread_cond_t {
    pthread_cond_t the_condition;
    volatile char the_flag;
} otk_thread_cond_t;
#define otk_thread_mutex_t pthread_mutex_t
#define otk_thread_func_return_type void*
#define otk_thread_func_return_value return NULL
#else
#define otk_thread_t uv_thread_t
typedef struct otk_thread_cond_t {
    uv_cond_t the_condition;
    volatile char the_flag;
} otk_thread_cond_t;
#define otk_thread_mutex_t uv_mutex_t
#define otk_thread_func_return_type void
#define otk_thread_func_return_value
#endif

#if defined(__cplusplus)
extern "C" {
#endif

int
otk_thread_create(otk_thread_t *thread, otk_thread_func_return_type (*start_routine)(void *), void *arg);

int
otk_thread_join(otk_thread_t thread);

int
otk_thread_mutex_lock(otk_thread_mutex_t *mutex);

int
otk_thread_mutex_unlock(otk_thread_mutex_t *mutex);

int
otk_thread_cond_signal(otk_thread_cond_t *cond);

int
otk_thread_mutex_init(otk_thread_mutex_t *mutex);

int
otk_thread_mutex_destroy(otk_thread_mutex_t *mutex);

int
otk_thread_equal(otk_thread_t t1, otk_thread_t t2);

int
otk_thread_cond_init(otk_thread_cond_t *cond);

int
otk_thread_cond_wait(otk_thread_cond_t *cond,
                     otk_thread_mutex_t *mutex);

int
otk_thread_cond_destroy(otk_thread_cond_t *cond);

otk_thread_t
otk_thread_self(void);

int
otk_thread_detach(otk_thread_t thread);
    
void
otk_thread_print_self_id(FILE *f);

// TODO-OPENTOK-38210 - Think about how to deal with interrupt and timed wait
int
otk_thread_cond_timedwait(otk_thread_cond_t* cond, otk_thread_mutex_t* mutex, struct timespec* to);

#if defined(__cplusplus)
}
#endif

#endif // OTK_THREAD_H
