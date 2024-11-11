//
// Created by os on 11/11/24.
//


#include "../h/ccb.hpp"
#include "../h/riscv.hpp"

CCB* CCB::running = nullptr;

CCB* CCB::createCoroutine(Body body) {
    return new CCB(body);
}

void CCB::yield() {
    Riscv::pushRegisters();
    CCB::dispatch();
    Riscv::popRegisters();
}

void CCB::dispatch() {
    CCB* old = running;
    if(!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    CCB::contextSwitch(&old->context, &running->context);
}