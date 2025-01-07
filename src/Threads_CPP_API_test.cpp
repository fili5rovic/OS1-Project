#include "../h/syscall_cpp.hpp"

#include "../h/print.hpp"

static volatile bool finishedA = false;
static volatile bool finishedB = false;
static volatile bool finishedC = false;
static volatile bool finishedD = false;

static uint64 fibonacci(uint64 n) {
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { thread_dispatch(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

class WorkerA: public Thread {
    void workerBodyA(void* arg);
public:
    WorkerA():Thread() {}

    void run() override {
        workerBodyA(nullptr);
    }
};

class WorkerB: public Thread {
    void workerBodyB(void* arg);
public:
    WorkerB():Thread() {}

    void run() override {
        workerBodyB(nullptr);
    }
};

class WorkerC: public Thread {
    void workerBodyC(void* arg);
public:
    WorkerC():Thread() {}

    void run() override {
        workerBodyC(nullptr);
    }
};

class WorkerD: public Thread {
    void workerBodyD(void* arg);
public:
    WorkerD():Thread() {}

    void run() override {
        workerBodyD(nullptr);
    }
};

void WorkerA::workerBodyA(void *arg) {
    for (uint64 i = 0; i < 10; i++) {
        print("A: i="); printInt(i); print("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    print("A finished!\n");
    finishedA = true;
}

void WorkerB::workerBodyB(void *arg) {
    for (uint64 i = 0; i < 16; i++) {
        print("B: i="); printInt(i); print("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 30000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    print("B finished!\n");
    finishedB = true;
    thread_dispatch();
}

void WorkerC::workerBodyC(void *arg) {
    uint8 i = 0;
    for (; i < 3; i++) {
        print("C: i="); printInt(i); print("\n");
    }

    print("C: dispatch\n");
    __asm__ ("li t1, 7");
    thread_dispatch();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    print("C: t1="); printInt(t1); print("\n");

    uint64 result = fibonacci(12);
    print("C: fibonaci="); printInt(result); print("\n");

    for (; i < 6; i++) {
        print("C: i="); printInt(i); print("\n");
    }

    print("A finished!\n");
    finishedC = true;
    thread_dispatch();
}

void WorkerD::workerBodyD(void* arg) {
    uint8 i = 10;
    for (; i < 13; i++) {
        print("D: i="); printInt(i); print("\n");
    }

    print("D: dispatch\n");
    __asm__ ("li t1, 5");
    thread_dispatch();

    uint64 result = fibonacci(16);
    print("D: fibonaci="); printInt(result); print("\n");

    for (; i < 16; i++) {
        print("D: i="); printInt(i); print("\n");
    }

    print("D finished!\n");
    finishedD = true;
    thread_dispatch();
}


void Threads_CPP_API_test() {
    Thread* threads[4];

    threads[0] = new WorkerA();
    print("ThreadA created\n");

    threads[1] = new WorkerB();
    print("ThreadB created\n");

    threads[2] = new WorkerC();
    print("ThreadC created\n");

    threads[3] = new WorkerD();
    print("ThreadD created\n");

    for(int i=0; i<4; i++) {
        threads[i]->start();
    }

    while (!(finishedA && finishedB && finishedC && finishedD)) {
        Thread::dispatch();
    }

    for (auto thread: threads) { delete thread; }
}