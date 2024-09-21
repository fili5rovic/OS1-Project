//
// Created by fili5rovic on 9/13/24.
//


#include "../h/KMemoryAllocator.h"
#include "../h/Helper.h"


void test1() {
    void* a;
    int *b, *c;

    KMemoryAllocator::getInstance().printStateWithMessage("Initial state: \n");
    a = KMemoryAllocator::getInstance().allocate(sizeof(int));
    KMemoryAllocator::getInstance().printStateWithMessage("After making *a:\n");
    b = static_cast<int*>(a);
    *b = 305;

    a = KMemoryAllocator::getInstance().allocate(sizeof(int));
    c = static_cast<int*>(a);
    *c = *b + 3;
    KMemoryAllocator::getInstance().printStateWithMessage("After making *a:\n");

    KMemoryAllocator::getInstance().free(b);
    KMemoryAllocator::getInstance().printStateWithMessage("After freeing *b:\n");

    KMemoryAllocator::getInstance().free(a);
    KMemoryAllocator::getInstance().printStateWithMessage("After freeing *a:\n");
}

int main() {
    test1();
    return 0;
}
