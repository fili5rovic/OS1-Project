#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

void TCB::startTCB(TCB* tcb) {
    Scheduler::put(tcb);
}

TCB *TCB::createThread(Body body, void* arg, void* stack) {
    TCB* thread = new TCB(body, arg, stack);
    Scheduler::put(thread);
    return thread;
}

TCB* TCB::createThreadNoStart(Body body, void* arg, void* stack) {
    TCB* thread = new TCB(body, arg, stack);
    return thread;
}

void TCB::yield() {
    Riscv::w_a0(0x13);
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    // timeSliceCounter = 0;
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    if (TCB::running->getPrivilege() == USER) {
        Riscv::popSppSpie();
    } else {
        Riscv::systemPopSppSpie();
    }
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}
