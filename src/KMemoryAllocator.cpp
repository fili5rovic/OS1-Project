//
// Created by os on 10/15/24.
//

#include "../h/KMemoryAllocator.hpp"

Node* KMemoryAllocator::freeHead = nullptr;


void KMemoryAllocator::init() {
    freeHead = (Node*) (HEAP_START_ADDR);
    freeHead->size = (char*) HEAP_END_ADDR - (char*) HEAP_START_ADDR - sizeof(Node);
    freeHead->next = nullptr;
    freeHead->prev = nullptr;
}

void* KMemoryAllocator::malloc(const size_t _size) {
    if (_size <= 0)
        return nullptr;

    size_t size = MEM_BLOCK_SIZE * ((_size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);

    Node* current = freeHead;
    while (current != nullptr) {
        if (current->size >= size) {
            break;
        }
        current = current->next;
    }
    if (!current)
        return nullptr;

    if (current->size > size + sizeof(Node)) {
        Node* leftoverNode = (Node*) ((char*) current + size + sizeof(Node));
        leftoverNode->size = current->size - size - sizeof(Node);
        leftoverNode->prev = current->prev;
        leftoverNode->next = current->next;

        current->size = size;

        if (current->prev)
            current->prev->next = leftoverNode;
        else freeHead = leftoverNode;
        if (current->next)
            current->next->prev = leftoverNode;
    } else { // iste velicine
        if (current->prev) {
            current->prev->next = current->next;
        } else freeHead = freeHead->next;
        if (current->next) {
            current->next->prev = current->prev;
        }
    }
    return (void*) ((uint64) current + sizeof(Node));
}

int KMemoryAllocator::free(void* ptr) {
    if (!ptr) return -1;

    if ((uint64) ptr < (uint64) HEAP_START_ADDR || (uint64) ptr >= (uint64) HEAP_END_ADDR)
        return -2;

    Node* freedNode = (Node*) ((uint64) ptr - sizeof(Node));

    Node* current = freeHead;
    Node* prev = nullptr;

    while (current && current < freedNode) {
        prev = current;
        current = current->next;
    }

    freedNode->next = current;
    freedNode->prev = prev;

    if (prev) prev->next = freedNode;
    else freeHead = freedNode;

    if (current) current->prev = freedNode;

    mergeAdjacent(freedNode);
    return 0;
}

void KMemoryAllocator::mergeAdjacent(Node* node) {
    if (node->next && (char*) node + sizeof(Node) + node->size == (char*) node->next) {
        node->size += sizeof(Node) + node->next->size;
        node->next = node->next->next;
        if (node->next)
            node->next->prev = node;
    }

    if (node->prev && (char*) node->prev + sizeof(Node) + node->prev->size == (char*) node) {
        node->prev->size += sizeof(Node) + node->size;
        node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
    }
}
