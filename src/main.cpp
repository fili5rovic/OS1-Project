//
// Created by os on 9/13/24.
//

#include "../h/KMemoryAllocator.h"
#include "../lib/console.h"


int main() {
    void* a = KMemoryAllocator::getInstance().allocate(sizeof(int));
    int* b = static_cast<int*>(a);
    *b = 305;

    a = KMemoryAllocator::getInstance().allocate(sizeof(int));
    int* c = static_cast<int*>(a);
    *c = *b + 3;

    KMemoryAllocator::getInstance().free(b);
    KMemoryAllocator::getInstance().free(a);
    // KMemoryAllocator::getInstance().free(c);
    return 0;
}
