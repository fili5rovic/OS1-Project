#include "../h/syscall_cpp.hpp"

Thread::Thread(void(* body)(void*), void* arg): body(body), arg(arg) {
    thread_create_no_start(&this->myHandle, body, arg);
}

Thread::~Thread() { delete this->myHandle; }

int Thread::start() {
    return thread_start(this->myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t) {
    return 0;
}

Thread::Thread() : body(nullptr), arg(nullptr) {
    thread_create_no_start(&this->myHandle, wrapper, this);
}

void Thread::run() {
    // Default is empty
}

Semaphore::Semaphore(unsigned init) {
    sem_open(&this->myHandle, init);
}

Semaphore::~Semaphore() { delete myHandle;}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t) {
    return 0;
}

int Semaphore::tryWait() {
    return sem_trywait(this->myHandle);
}
