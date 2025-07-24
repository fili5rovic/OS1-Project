//
// Created by fili5rovic on 9/13/24.
//

#ifndef KMEMORYALLOCATOR_H
#define KMEMORYALLOCATOR_H

#include "../lib/hw.h"
#include "print.hpp"

struct Node {
    Node* next;
    Node* prev;
    uint64 size;
};


class KMemoryAllocator {
public:
    KMemoryAllocator() = delete;
    KMemoryAllocator(KMemoryAllocator const &) = delete;
    KMemoryAllocator(KMemoryAllocator &&) = delete;
    KMemoryAllocator &operator=(KMemoryAllocator const &) = delete;
    KMemoryAllocator &operator=(KMemoryAllocator &&) = delete;

    static void init();

    static void* malloc(size_t _size);

    static int free(void* ptr);

private:
    static Node* freeHead;

    static void mergeAdjacent(Node* node);
};


#endif //KMEMORYALLOCATOR_H
