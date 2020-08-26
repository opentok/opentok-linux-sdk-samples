#include "otk_thread.h"

#include <assert.h>

int
otk_thread_create(otk_thread_t *thread, otk_thread_func_return_type (*start_routine)(void *), void *arg)
{
#ifndef _WIN32
    return pthread_create(thread, NULL, start_routine, arg);
#else
	return uv_thread_create(thread, start_routine, arg);
#endif
}

int
otk_thread_join(otk_thread_t thread)
{
#ifndef _WIN32
	void* throwaway = NULL;
    return pthread_join(thread, &throwaway);
#else
	return uv_thread_join(&thread);
#endif
}

int
otk_thread_mutex_lock(otk_thread_mutex_t *mutex)
{
#ifndef _WIN32
    return pthread_mutex_lock(mutex);
#else
	uv_mutex_lock(mutex);
	return 0;
#endif
}

int
otk_thread_mutex_unlock(otk_thread_mutex_t *mutex)
{
#ifndef _WIN32
    return pthread_mutex_unlock(mutex);
#else
	uv_mutex_unlock(mutex);
	return 0;
#endif
}

int
otk_thread_cond_signal(otk_thread_cond_t *cond)
{
    cond->the_flag = 1;
#ifndef _WIN32
    return pthread_cond_signal(&(cond->the_condition));
#else
	uv_cond_signal(&(cond->the_condition));
	return 0;
#endif
}

int
otk_thread_mutex_init(otk_thread_mutex_t *mutex)
{
#ifndef _WIN32
    return pthread_mutex_init(mutex, NULL);
#else
	return uv_mutex_init(mutex);
#endif
}

int
otk_thread_mutex_destroy(otk_thread_mutex_t *mutex)
{
#ifndef _WIN32
    return pthread_mutex_destroy(mutex);
#else
	uv_mutex_destroy(mutex);
	return 0;
#endif
}

int
otk_thread_equal(otk_thread_t t1, otk_thread_t t2)
{
#ifndef _WIN32
    return pthread_equal(t1, t2);
#else
	return uv_thread_equal(&t1, &
		t2);
#endif
}

int
otk_thread_cond_init(otk_thread_cond_t *cond)
{
    cond->the_flag = 0;
#ifndef _WIN32
    return pthread_cond_init(&(cond->the_condition), NULL);
#else
	return uv_cond_init(&(cond->the_condition));
#endif
}

int
otk_thread_cond_wait(otk_thread_cond_t *cond,
                     otk_thread_mutex_t *mutex)
{
#ifndef _WIN32
    int ret;
    do
    {
        ret = pthread_cond_wait(&(cond->the_condition), mutex);
        /* if(!cond->the_flag) */
        /* { */
        /*     OTK_NATIVE_SDK_DEBUG("otk_thread_cond_wait spurious wakeup"); */
        /* } */
    } while((0 == ret) && (!(cond->the_flag)));
    return ret;
#else
    do
    {
        uv_cond_wait(&(cond->the_condition), mutex);
        /* if(!cond->the_flag) */
        /* { */
        /*     OTK_NATIVE_SDK_DEBUG("otk_thread_cond_wait spurious wakeup"); */
        /* } */
    } while(!(cond->the_flag));
	return 0;
#endif
}

int
otk_thread_cond_destroy(otk_thread_cond_t *cond)
{
#ifndef _WIN32
    return pthread_cond_destroy(&(cond->the_condition));
#else
	uv_cond_destroy(&(cond->the_condition));
	return 0;
#endif
}

otk_thread_t
otk_thread_self(void)
{
#ifndef _WIN32
    return pthread_self();
#else
	return uv_thread_self();
#endif
}

int
otk_thread_detach(otk_thread_t thread)
{
#ifndef _WIN32
	return pthread_detach(thread);
#else
	// Well, if we have to do this on Windows, we should
	// close the handle (the thread keeps running).
	assert(0);
	return 0;
#endif
}

/* Thanks http://stackoverflow.com/questions/1759794/how-to-print-pthread-t */
void
otk_thread_print_self_id(FILE *f)
{
    otk_thread_t pt = otk_thread_self();
    unsigned char *ptc = (unsigned char*)(void*)(&pt);
    fprintf(f, "Thread 0x");
    for (size_t i = 0; i<sizeof(pt); i++) {
        fprintf(f, "%02x", (unsigned)(ptc[i]));
    }
}

// TODO-OPENTOK-38210 - Think about how to deal with interrupt and timed wait
int 
otk_thread_cond_timedwait(otk_thread_cond_t* cond, otk_thread_mutex_t* mutex, struct timespec* to)
{
#ifndef _WIN32
	struct timespec temp_to = *to;
	temp_to.tv_sec += time(NULL);
	return pthread_cond_timedwait(&(cond->the_condition), mutex, &temp_to);
#else
	// uv_cond_timedwait seems to take nanoseconds...
	return uv_cond_timedwait(&(cond->the_condition), mutex, (to->tv_sec * ((uint64_t) 1e9)) + to->tv_nsec);
#endif
}

