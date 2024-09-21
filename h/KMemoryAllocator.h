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
                    nNode = reinterpret_cast<Node*>(reinterpret_cast<char*>(current) + newSize);
                    nNode->prev = current->prev;
                    nNode->next = current->next;
                    nNode->size = current->size - newSize;
                    if (current == freeHead)
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
        if (nextAddress == 0)
            return nullptr;

        void* allocatedMemory = reinterpret_cast<void*>(nextAddress);
        return allocatedMemory;
    }

    int free(void* ptr) {
        if (ptr == nullptr)
            return 0;
        uint64 addr = reinterpret_cast<uint64>(ptr) - sizeof(Node);
        Node* node = reinterpret_cast<Node*>(addr);
        if (node == nullptr)
            return -1;
        return addFreeNode(node);
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


    int addFreeNode(Node* node) {
        if (freeHead == nullptr) {
            freeHead = node;
            return 0;
        }
        Node* current = freeHead;

        while (current != nullptr) {
            if (current == node) return -1;
            // add node before current if the current is bigger
            if (current > node) {
                swapNodes(current, node);
                tryToMergeNodes(node, current);
                return 0;
            }
            if (current->next == nullptr) break;
            current = current->next;
        }
        // add node to the end of the list
        current->next = node;
        node->prev = current;
        // check if you can merge them
        tryToMergeNodes(current, node);
        return 0;
    }

    void swapNodes(Node* node1, Node* node2) {
        __putc('S');
        __putc('W');
        __putc('A');
        __putc('P');
        __putc('\n');
        if (node1->prev != nullptr)
            node1->prev->next = node2;
        if (node1->next != nullptr)
            node1->next->prev = node2;
        node2->next = node1;
        node2->prev = node1->prev;
        node1->prev = node2;
        if (node1 == freeHead)
            freeHead = node2;
    }

    int tryToMergeNodes(Node* smallerNode, Node* largerNode) {
        if(smallerNode == nullptr || largerNode == nullptr)
            return 1;
        uint64 diff = reinterpret_cast<uint64>(largerNode) - reinterpret_cast<uint64>(smallerNode);
        if (diff == smallerNode->size) {
            __putc('M');
            __putc('E');
            __putc('R');
            __putc('G');
            __putc('E');
            __putc('\n');
            smallerNode->size += largerNode->size + sizeof(Node);
            smallerNode->next = largerNode->next;
            largerNode->next = nullptr;
            largerNode->prev = nullptr;
            largerNode->size = 0;
            return tryToMergeNodes(smallerNode->prev, smallerNode);
        }
        return 2;
    }

    Node* freeHead;
    uint64 nextAddress;
};


#endif //KMEMORYALLOCATOR_H
