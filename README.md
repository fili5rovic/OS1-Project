
# RISC-V Educational Kernel Project

This project implements a minimal, functional kernel for an educational RISC-V operating system. It is designed to support multithreading, memory management, semaphores, console I/O, and time-sharing in a simplified, embedded-style system.

## Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Features](#features)
- [Build Instructions](#build-instructions)
- [Run Instructions](#run-instructions)
- [File Structure](#file-structure)
- [Development Notes](#development-notes)
- [License](#license)

---

## Overview

The goal of this project is to build a single-address-space kernel in C++ and assembly for the RISC-V architecture, executed within a modified `xv6` host system in the QEMU emulator. The kernel supports threads, memory allocation, semaphores and console I/O using a low-level C/C++ interface and a hardware abstraction layer.

## System Architecture

- **Target**: RISC-V (RV64IMA)
- **Kernel Type**: Monolithic, preemptive
- **Execution Model**: Embedded system model (user code and kernel statically linked)
- **Runtime**: xv6-hosted QEMU virtual machine

## Features

-  **Threads**: Lightweight, cooperative or preemptive
-  **Memory Management**: Custom `mem_alloc` and `mem_free` for user-defined heap
-  **Semaphores**: For synchronization with optional timeouts
-  **Console I/O**: Blocking `getc` and `putc` with interrupt support
-  **Timer Interrupts**: Used for thread dispatch and sleep
-  **APIs**:
  - **ABI**: System call via software interrupt (ecall)
  - **C API**: `syscall_c.hpp`
  - **C++ OO API**: `syscall_cpp.hpp`

## Build Instructions

1. Install [VMware Workstation Player](https://www.vmware.com/products/workstation-player.html)
2. Download the virtual machine from the course website
3. Open the project in **CLion** or any compatible IDE
4. Place source files into the `src/` and headers into `h/`
5. Run:
   ```bash
   make qemu
   ```

## Run Instructions

- Emulator: **QEMU**
- Targets:
  - `make qemu`: Build and run
  - `make qemu-gdb`: Build and run with GDB debugging
  - `make clean`: Remove build artifacts
- Stop emulator using the “Stop” option or by writing `0x5555` to `0x100000`
## Development Notes

- **Assembly**: Use `.S` files for RISC-V specific routines (e.g. context switching)
- **Testing**: Use `app.lib` test programs or write custom ones
- **Memory**: All allocation is within `[HEAP_START_ADDR, HEAP_END_ADDR)`
- **Restrictions**: No usage of standard libraries or host system calls

