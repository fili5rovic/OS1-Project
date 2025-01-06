#ifndef SYSCALL_CPP_H
#define SYSCALL_CPP_H

#include "syscall_c.h"

class Thread {
public:
    Thread(void (*body)(void*), void* arg);

    virtual ~Thread();

    int start();

    static void dispatch();

    static int sleep(time_t);



protected:
    Thread();

    virtual void run();

private:

    static void wrapper(void* thread) {
        if (thread != nullptr)
            ((Thread*)thread)->run();
    }

    thread_t myHandle;

    void (*body)(void*);

    void* arg;
};

#endif //SYSCALL_CPP_H
