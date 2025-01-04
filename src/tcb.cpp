#include "../h/tcb.hpp"
#include "../h/riscv.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body, void* arg, void* stack) {
    TCB* thread = new TCB(body, arg, stack);
    Scheduler::put(thread);
    return thread;
}

void TCB::yield() {
    Riscv::w_a0(0x13);
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();
    if (!running) {
        print("No runnable thread in dispatch.");
        return;
    }

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}
