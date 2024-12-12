#ifndef C_SYSCALL_H
#define C_SYSCALL_H

#include "../lib/hw.h"

void* mem_alloc(size_t size);

int mem_free(void* ptr);

#endif
