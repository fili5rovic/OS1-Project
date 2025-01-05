#include "../h/KSem.hpp"

#include "../h/syscall_c.h"

KSem* KSem::create(const uint64 v) {
    return new KSem(v);
}

int KSem::signal() {
    if (closed)
        return -1;

    val++;
    if (int(val) >= 1) {
        unblock();
    }
    return 0;
}

int KSem::wait() {
    if (closed)
        return -1;

    val--;
    if (int(val) <= 0) {
        block();
    }
    // closed check after dispatching
    if (closed)
        return -1;
    return 0;
}

int KSem::trywait() {
    if (closed)
        return -1;
    return val>0;
}

int KSem::close() {
    if (closed)
        return -1;
    closed = true;

    TCB* current = this->blocked.removeFirst();

    while (current) {
        current->setBlocked(false);
        Scheduler::put(current);
        current = this->blocked.removeFirst();
    }
    return 0;
}

void KSem::block() {
    TCB::running->setBlocked(true);
    this->blocked.addLast(TCB::running);
    thread_dispatch();
}

void KSem::unblock() {
    TCB* unblocked = this->blocked.removeFirst();
    if (!unblocked) {
        return;
    }
    unblocked->setBlocked(false);
    Scheduler::put(unblocked);
}
