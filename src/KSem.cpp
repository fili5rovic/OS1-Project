#include "../h/KSem.hpp"
#include "../h/syscall_c.hpp"
#include "../h/scheduler.hpp"

KSem* KSem::create(const int v) {
    return new KSem(v);
}

int KSem::signal() {
    if(closed) {
        return -1;
    }
    if(++val >= 0) {
        unblock();
    }
    return 0;
}

int KSem::wait() {
    if(closed) {
        return -1;
    }

    if(--val < 0) {
        block();

        if(closed) {
            return -1;
        }
    }
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
    print("Closing semaphore\n");
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
