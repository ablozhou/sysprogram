# system program

This repository mainly contains practices and examples of system program code based on Linux system. There are also some algorithm code, distributed database code and OS practice code.

- algorithm is algorithm python code
- ddbprj: distributed database python code to justify some timestamp ordering transactions deadlock.
- os: a simulation of traffic system using concurrency technique, such as multi-thread, multi-process and asynchorolizing and communicating technique, such as mutex, FIFO condition variables and semaphores.

Other directions are C code of example or practice code.
Include:

- file and directory reading and writing, tranversing directories.
- process creating: fork and exec
- multi-threading creating
- synchronization and communication mechanizims between threads and processes, such as mutex and condtion varialbles.
- IPC, include pipe, FIFO, message queue, semaphore, share memory, file locking etc.

# env

- Ubuntu 24.04
- gcc
- python 3.10

# build

- cc [aaa].c -o [aaa] -Werror
- cc [aaa].c -o [aaa] -Werror -lpthread -lrt

replace [aaa] to the actual c file name.
