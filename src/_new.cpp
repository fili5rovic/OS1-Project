//
// Created by os on 10/15/24.
//
#include "../h/KMemoryAllocator.hpp"
#include "../lib/mem.h"

void* operator new(const uint64 n) {
    return __mem_alloc(n);
    // return KMemoryAllocator::getInstance().allocate(n);
}

void* operator new[](const uint64 n) {
    return __mem_alloc(n);
    // return KMemoryAllocator::getInstance().allocate(n);
}

void operator delete(void* p) noexcept {
    __mem_free(p);
    // KMemoryAllocator::getInstance().free(p);
}

void operator delete[](void* p) noexcept {
    __mem_free(p);
    // KMemoryAllocator::getInstance().free(p);
}
