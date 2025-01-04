#include "../h/syscall_c.h"

#include "../h/riscv.hpp"

void* mem_alloc(size_t size) {
    if (size == 0)
        return nullptr;

    __asm__ volatile("mv a1, %0" : : "r"(size));
    __asm__ volatile("li a0, 0x01");

    __asm__ volatile("ecall");

    void* ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int mem_free(void* ptr) {
    if (!ptr)
        return 0;

    __asm__ volatile("mv a1, %0" : : "r"(ptr));
    __asm__ volatile("li a0, 0x02");

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;

}

int thread_create(thread_t* handle, void(* start_routine)(void*), void* arg) {
    if (!handle) { return -1; }
    if (!start_routine) { return -2; }

    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stack) {
        return -3;
    }


    __asm__ volatile("mv a4, %0" : : "r"(stack));
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x11");

    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

int thread_exit() {
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return ret;
}

void thread_dispatch() {
    __asm__ volatile("li a0, 0x13"); // 13 should be here
    __asm__ volatile("ecall");
}
