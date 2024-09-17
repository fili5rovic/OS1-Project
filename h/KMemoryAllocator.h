//
// Created by os on 9/13/24.
//

#ifndef KMEMORYALLOCATOR_H
#define KMEMORYALLOCATOR_H

#include "../lib/hw.h"
#include "../lib/console.h"

struct Node {
    Node* next;
    Node* prev;
    uint64 size;
};

class KMemoryAllocator {
public:
    static KMemoryAllocator &getInstance() {
        static KMemoryAllocator instance;
        return instance;
    }

    KMemoryAllocator(KMemoryAllocator const &) = delete;
    KMemoryAllocator(KMemoryAllocator &&) = delete;
    KMemoryAllocator &operator=(KMemoryAllocator const &) = delete;
    KMemoryAllocator &operator=(KMemoryAllocator &&) = delete;


    void* allocate(const size_t _size) {
        size_t newSize = _size + sizeof(Node);
        if (newSize % MEM_BLOCK_SIZE != 0) {
            newSize -= newSize % MEM_BLOCK_SIZE;
            newSize += MEM_BLOCK_SIZE;
        }
        nextAddress = 0;
        Node* current = freeHead;
        while (current != nullptr) {
            // first fit
            if (current->size >= newSize) {
                // if current has more space, make another free node from the rest
                const bool overLimit = reinterpret_cast<uint64>(current) + newSize + sizeof(Node) >= reinterpret_cast<
                                           uint64>(HEAP_END_ADDR);
                Node* nNode = nullptr;
                if (current->size - newSize >= MEM_BLOCK_SIZE && !overLimit) {
                    nNode = (current + newSize);
                    nNode->prev = current->prev;
                    nNode->next = current->next;
                    nNode->size = current->size - newSize;
                    if(current == freeHead)
                        freeHead = nNode;
                }

                if (current->prev != nullptr)
                    current->prev->next = nNode ? nNode : current->next;
                if (current->next != nullptr)
                    current->next->prev = nNode ? nNode : current->prev;

                current->next = nullptr;
                current->prev = nullptr;
                current->size = newSize;
                nextAddress = reinterpret_cast<uint64>(current) + sizeof(Node);
                break;
            }
            current = current->next;
        }
        if(nextAddress == 0)
            return nullptr;


        void* allocatedMemory = reinterpret_cast<void*>(nextAddress);

        return allocatedMemory;
    }

    int free(void* ptr) {
        if (ptr == nullptr)
            return 0;
        uint64 addr = reinterpret_cast<uint64>(ptr)-sizeof(Node);
        Node* node = reinterpret_cast<Node*>(addr);
        if (node == nullptr)
            return -1;
        addFreeNode(node);
        return 0;
    }

private:
    KMemoryAllocator() {
        nextAddress = reinterpret_cast<uint64>(HEAP_START_ADDR);

        freeHead = (Node*) ((char*) nextAddress);
        nextAddress += sizeof(Node);
        freeHead->next = nullptr;
        freeHead->prev = nullptr;
        freeHead->size = reinterpret_cast<uint64>(HEAP_END_ADDR) - nextAddress - sizeof(Node);
    }


    void addFreeNode(Node* node) {
        if (node == nullptr)
            return;
        Node* current = freeHead;
        while (current->next != nullptr && node < current->next) {
            current = current->next;
        }
        current->next = node;
        node->prev = current;
    }

    Node* freeHead;
    uint64 nextAddress;
};


#endif //KMEMORYALLOCATOR_H
