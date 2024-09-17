//
// Created by os on 9/13/24.
//

#ifndef KMEMORYALLOCATOR_H
#define KMEMORYALLOCATOR_H

#include "../lib/hw.h"
#include "../lib/console.h"

struct Node {
    Node *next;
    uint64 size;
    uint64 addr;

    Node(const uint64 s) {
        this->size = s;
        this->next = nullptr;
    }
};

class KMemoryAllocator {
public:
    static KMemoryAllocator &getInstance() {
        static KMemoryAllocator instance;
        return instance;
    }

    void *allocate(const size_t size) {
        size_t newSize = size;
        if (size % MEM_BLOCK_SIZE != 0) {
            newSize -= size % MEM_BLOCK_SIZE;
            newSize += MEM_BLOCK_SIZE;
        }

        Node* current = freeHead;
        while(current != nullptr) {
            if(current->size >= newSize) {
                nextAddress = current->addr;
                // sad treba da ga pomerim za size
                if(current->addr + newSize < current->size) {
                    current->addr += newSize;
                } else {
                    // todo delete current
                }
                // ----
                break;
            }
            current = current->next;
        }

        void* allocatedMemory = reinterpret_cast<void *>(nextAddress);
        nextAddress += newSize;

        return allocatedMemory;
    }

private:
    KMemoryAllocator() {
        nextAddress = reinterpret_cast<uint64>(HEAP_START_ADDR);

        freeHead = (Node*)((char*)HEAP_START_ADDR);
        freeHead->next = nullptr;
        freeHead->addr = nextAddress;
        freeHead->size = reinterpret_cast<uint64>(HEAP_END_ADDR) - nextAddress - sizeof(Node);
        nextAddress += sizeof(Node);
    }

    Node *freeHead;
    uint64 nextAddress;
};


#endif //KMEMORYALLOCATOR_H
