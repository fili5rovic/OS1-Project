#ifndef SYSCALL_CPP_H
#define SYSCALL_CPP_H

#include "syscall_c.hpp"

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
            ((Thread*) thread)->run();
    }

    thread_t myHandle;

    void (*body)(void*);

    void* arg;
};


class Semaphore {
public:
    Semaphore(unsigned init = 1);

    virtual ~Semaphore();

    int wait();

    int signal();

    int timedWait(time_t);

    int tryWait();

private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {
public:
    void terminate() {}

protected:
    PeriodicThread(time_t period){}

    virtual void periodicActivation() {}

private:
    time_t period;
};

class Console {
public:
    static char getc() {
        return __getc();
    }

    static void putc(char c) {
        __putc(c);
    }
};

#endif //SYSCALL_CPP_H
