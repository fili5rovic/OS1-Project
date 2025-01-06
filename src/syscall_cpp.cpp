#include "../h/syscall_cpp.hpp"

Thread::Thread(void(* body)(void*), void* arg): body(body), arg(arg) {
    thread_create(&this->myHandle, body, arg);
}

Thread::~Thread() { delete myHandle; }

int Thread::start() {
    return thread_start(myHandle);
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
