#ifndef C_SYSCALL_H
#define C_SYSCALL_H

#include "KSem.hpp"
#include "tcb.hpp"
#include "../lib/hw.h"
#include "../lib/console.h"

void* mem_alloc(size_t size);

int mem_free(void* ptr);

/**
 * Threads
 */
typedef TCB* thread_t;

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_start(TCB* tcb);

int thread_create_no_start (thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

int time_sleep(time_t t);

/**
 * Semaphore
 */
typedef KSem* sem_t;

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int sem_trywait(sem_t id);

/**
 * Console
 */

char getc();

void putc(char c);

#endif
