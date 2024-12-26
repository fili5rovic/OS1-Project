//
// Created by fili5rovic on 9/13/24.
//


#include "../h/KMemoryAllocator.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/tcb.hpp"
#include "../h/workers.hpp"

#include "../h/Threads_C_API_test.hpp"


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

void testBasicAllocation() {
    KMemoryAllocator &allocator = KMemoryAllocator::getInstance();

    // Allocate 64 bytes of memory
    void* ptr = allocator.allocate(64);
    if (ptr != nullptr) {
        print("[SUCCESS] Basic Allocation Test Passed: Allocated 64 bytes.\n");
    } else {
        print("[FAIL] Basic Allocation Test Failed: Allocation failed.\n");
    }
}

void testAllocationAndFreeing() {
    KMemoryAllocator &allocator = KMemoryAllocator::getInstance();

    // Allocate 128 bytes of memory
    void* ptr1 = allocator.allocate(128);
    if (ptr1 != nullptr) {
        print("First Allocation Test Passed: Allocated 128 bytes.\n");
    } else {
        print("First Allocation Test Failed: Allocation failed.\n");
    }

    // Allocate another block of 256 bytes
    void* ptr2 = allocator.allocate(256);
    if (ptr2 != nullptr) {
        print("Second Allocation Test Passed: Allocated 256 bytes.\n");
    } else {
        print("Second Allocation Test Failed: Allocation failed.\n");
    }

    // Free the first block
    int freeResult1 = allocator.free(ptr1);
    if (freeResult1 == 0) {
        print("Freeing First Block Test Passed.\n");
    } else {
        print("Freeing First Block Test Failed.\n");
    }

    // Free the second block
    int freeResult2 = allocator.free(ptr2);
    if (freeResult2 == 0) {
        print("Freeing Second Block Test Passed.\n");
    } else {
        print("Freeing Second Block Test Failed.\n");
    }
}


void testSplittingAndMerging() {
    KMemoryAllocator &allocator = KMemoryAllocator::getInstance();

    // Allocate 512 bytes of memory
    void* ptr1 = allocator.allocate(512);

    if (ptr1 != nullptr) {
        print("First Allocation (512 bytes) Passed.\n");
    } else {
        print("First Allocation (512 bytes) Failed.\n");
        return;
    }

    // Allocate a smaller block, should split the original free block
    void* ptr2 = allocator.allocate(64);
    if (ptr2 != nullptr) {
        print("Second Allocation (64 bytes) Passed.\n");
    } else {
        print("Second Allocation (64 bytes) Failed.\n");
    }

    // Free both blocks
    allocator.free(ptr1);
    allocator.free(ptr2);
    print("Freeing Both Blocks Test Completed.\n");
}

void testAllocatingAllMemory() {
    KMemoryAllocator &allocator = KMemoryAllocator::getInstance();
    allocator.printStateWithMessage("Initial state: \n");
    // Allocate as much memory as possible
    size_t heapSize = reinterpret_cast<uint64>(HEAP_END_ADDR) - reinterpret_cast<uint64>(HEAP_START_ADDR);
    print("Real heap size: ");
    printInt(heapSize);
    printNewLine();

    heapSize -= sizeof(Node);
    // heapSize -= 64; // free ne radi sa vrednostima manjim od heapSize - 64
    print("Working heap size: ");
    printInt(heapSize);
    printNewLine();
    void* ptr = allocator.allocate(heapSize);
    allocator.printStateWithMessage("After *ptr: \n");
    if (ptr != nullptr)
        print("[SUCCESS] Allocation of the entire heap.\n");
    else
        print("[FAIL] Allocation of the entire heap.\n");


    // Free the entire heap
    int freeResult = allocator.free(ptr);
    allocator.printStateWithMessage("After free: \n");
    if (freeResult == 0)
        print("[SUCCESS] Freeing the entire heap succeeded.\n");
    else
        print("[FAIL] Freeing the entire heap failed.\n");
}

void threadTest() {
    TCB* threads[5];

    threads[0] = TCB::createThread(nullptr, nullptr);
    TCB::running = threads[0];

    threads[1] = TCB::createThread(workerBodyA, nullptr);
    // thread_create(&threads[1], workerBodyA, nullptr);
    print("ThreadA created\n");
    threads[2] = TCB::createThread(workerBodyB, nullptr);
    print("ThreadB created\n");
    threads[3] = TCB::createThread(workerBodyC, nullptr);
    print("ThreadC created\n");
    threads[4] = TCB::createThread(workerBodyD, nullptr);
    print("ThreadD created\n");

    while (!(threads[1]->isFinished() &&
             threads[2]->isFinished() &&
             threads[3]->isFinished() &&
             threads[4]->isFinished())) {
        thread_dispatch();
             }

    for (auto &thread: threads) {
        delete thread;
    }
    print("Finished\n");
}


int main() {
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // OTVARA tajmer


    TCB* mainThread = TCB::createThread(nullptr, nullptr);
    TCB::running = mainThread;
    Threads_C_API_test();

    // threadTest();

    // Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // uint64* addr = (uint64*) mem_alloc(sizeof(uint64));
    // *addr = 3;
    // printDebug("Value: ", *addr);
    //
    // mem_free(addr);
    //
    // print("Main finished...\n");
    return 0;
}
