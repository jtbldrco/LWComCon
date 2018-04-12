# #LWComCon <img style="float: right;" src="../images/iwaytechnology284x60.gif" />

## A Multi-Threaded, Multi-Process, Network-based</br>Command/Control Design Pattern in C++

MIT License -- Copyright 2018 iWay Technology LLC -- Boulder, Colorado  USA

**Status: Work In Progress ...**

This C++ repository folder contains materials to introduce and demonstrate a TCP network-based, Inter-process,
Multi-threaded Command and Control Methodology in C++.  Included are numerous additional utility design patterns such as -

- the C++ ThreadedWorker class which offers a simple multi-threading codification of the division of labor
between creator- and created-threads and the patterns of use for startup, operation and shutdown,
- a Thread-Safe, Pointer-based Message Queue - the C++ *templatized* class ThreadSafeMsgPtrQueue -
which demonstrates software *resource acquisition is initialization* (RAII) and makes trivial
(literally *invisible*) the thread-safe use of Standard Template Library collections (and also defines
a model for safe, responsible management of pointer-based C++ collections),
- Linux rsyslog logging techniques and utilities, and
- other utilities to ease C++ multi-threaded & network programming, testing, and trouble-shooting.

## Example C++ Use Case: A Full (yet simple) implementation and demonstration of Lightweight Command and Control

This subdirectory project employs all the pieces above in a working example that is intended to
be run from three console windows to illustrate the dynamics.  

See [./lwcomcon_full/](./lwcomcon_full/).
