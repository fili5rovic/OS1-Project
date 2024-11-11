//
// Created by os on 10/15/24.
//
#include "../h/KMemoryAllocator.hpp"
#include "../lib/mem.h"

void* operator new(const uint64 n) {
    return KMemoryAllocator::getInstance().allocate(n);
}

void* operator new[](const uint64 n) {
    return KMemoryAllocator::getInstance().allocate(n);
}

void operator delete(void* p) noexcept {
    KMemoryAllocator::getInstance().free(p);
}

void operator delete[](void* p) noexcept {
    KMemoryAllocator::getInstance().free(p);
}
