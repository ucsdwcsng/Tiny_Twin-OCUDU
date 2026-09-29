/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef PTHREAD_UTILS_H
#define PTHREAD_UTILS_H

#include <pthread.h>
#define mutexinit(mutex)   {int ret=pthread_mutex_init(&mutex,NULL); \
                            AssertFatal(ret==0,"ret=%d\n",ret);}
#define condinit(signal)   {int ret=pthread_cond_init(&signal,NULL); \
                            AssertFatal(ret==0,"ret=%d\n",ret);}
#define mutexlock(mutex)   {int ret=pthread_mutex_lock(&mutex); \
                            AssertFatal(ret==0,"ret=%d\n",ret);}
#define mutextrylock(mutex)   pthread_mutex_trylock(&mutex)
#define mutexunlock(mutex) {int ret=pthread_mutex_unlock(&mutex); \
                            AssertFatal(ret==0,"ret=%d\n",ret);}
#define condwait(condition, mutex) {int ret=pthread_cond_wait(&condition, &mutex); \
                                    AssertFatal(ret==0,"ret=%d\n",ret);}
#define condbroadcast(signal) {int ret=pthread_cond_broadcast(&signal); \
                               AssertFatal(ret==0,"ret=%d\n",ret);}
#define condsignal(signal)    {int ret=pthread_cond_signal(&signal); \
                               AssertFatal(ret==0,"ret=%d\n",ret);}
#define mutexdestroy(mutex) { int ret = pthread_mutex_destroy(&mutex);\
                              AssertFatal(ret==0,"ret=%d\n",ret);}
#define conddestroy(condition) { int ret = pthread_cond_destroy(&condition);\
                                 AssertFatal(ret==0,"ret=%d\n",ret);}
#endif
