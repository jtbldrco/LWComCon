# Multi-Threading related C++ Classes <img style="float: right;" src="../../images/iwaytechnology284x60.gif" />

This subdirectory contains these important elements:

- A C++ implementation of a ThreadedWorker Design Pattern that simplifies and standardizes development of multi-threaded classes. 
- An example implementation of ThreadedWorker (derived class ExampleThreadedWorker).
- A convenience class, ThreadMapper, that simplifies debugging and validating of multi-threaded apps.
- A test program, ThreadedWorkerTest, which exercises and demonstrates all pieces as a whole.

## ThreadedWorker Design Pattern (class ThreadedWorker)

This design pattern simplifies and standardizes creation and management of C++ objects that must do their work each on a separate, native Thread.  The implementation provides thread-safety through encapsulation of lifecycle-event mutexing such as object/thread startup, external shutdown signaling, internal thread shutdown, and general C++ object destruction.  The pattern defines base- and derived-class roles and responsibilities enforced with a combination of C++ definitions of pure-virtual and final-implementation methods.  The pattern is tested and demonstrated with class ExampleThreadedWorker.

## Class ExampleThreadedWorker

This simple class, employed by app ThreadedWorkerTest.cpp, fully illustrates the division of labor in using the design pattern ThreadedWorker.  It can be built with debugging enabled (see Makefile) to pepper the test session console output with messages that identify the program flow *clearly indicating which native thread is executing each function*.

## ThreadMapper Convenience Class

Class ThreadMapper will provide shortened thread names - for example, instead of seeing thread ids ```140411889002304, 140411871180544, 140411862787840``` you would see thread ids ```A1, A2, A3```.  Also included is a preprocessor macro that automatically inserts new native thread ids into the map, transforms and returns these mapped names with compact notation (```MY_TID```) - the macro requires *no* arguments.  Subsequent use of the macro returns the original mapped name with no alternate syntax for first-definition vs subsequent use (see below).

The implementations included in this subdirectory utilize this class extensively.  For example, here is the syntax to both initially define a mapped thread id and/or recall its previously mapped name:

Code: 
```
std::cout << "Object " << getInstanceName() << " mainLoop(), running on thread " << MY_TID << std::endl;
```
Result:
```
Object Worker-B mainLoop(), running on thread A3
```
Class ThreadMapper will map the id of each of the first 260 threads created into the names ```A1..Z0```.  Additional thread ids are correctly handled but returned with the native id itself as a string by macro ```MY_TID```.  Using this thread-id mapping technique, it's simple to verify which thread is executing which code and trouble-shoot common multi-threading issues.

A separate test program, ThreadMapperTest.cpp, exercises and illustrates capabilities of this utility including the 'overflow' scenario of more than 260 ids.

## Test Program ThreadedWorkerTest.cpp

This program puts all the pieces together in a simple, easy-to-understand demonstration of the ThreadedWorker Design Pattern and ThreadMapper - illustrating the creation, launching, execution and shutdown of two Worker thread objects.

Below is a full capture of all program output when run *without* this following Makefile line *uncommented* (which would add further debug output):

```
# CPPFLAGS += -DDEBUG -DDEBUG_THREADEDWORKER
```
Remove the # comment delineator from the above line in the Makefile to enable further debugging output beyond what is shown below.  Note that the above change would cause the main thread to be mapped to A1, and alter the output which appears below so that the thread running Worker-A is renamed A2 and Worker-B named A3.  (The Makefile change above does not alter the underlying threading behavior of the test program or the ThreadedWorker design pattern! The mapped names simply shift.)

```
$ make
$ ./ThreadedWorkerTest

virtual void ExampleThreadedWorker::mainLoop() object Worker-A doing work here (for 3 seconds) ... on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-B doing work here (for 3 seconds) ... on thread A2

Sleeping main thread for 10 seconds.

virtual void ExampleThreadedWorker::mainLoop() object Worker-A checking for shutdown signal, on thread A1
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-A doing work here (for 3 seconds) ... on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-B checking for shutdown signal, on thread A2
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A2
virtual void ExampleThreadedWorker::mainLoop() object Worker-B doing work here (for 3 seconds) ... on thread A2
virtual void ExampleThreadedWorker::mainLoop() object Worker-A checking for shutdown signal, on thread A1
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-A doing work here (for 3 seconds) ... on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-B checking for shutdown signal, on thread A2
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A2
virtual void ExampleThreadedWorker::mainLoop() object Worker-B doing work here (for 3 seconds) ... on thread A2
virtual void ExampleThreadedWorker::mainLoop() object Worker-A checking for shutdown signal, on thread A1
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-A doing work here (for 3 seconds) ... on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-B checking for shutdown signal, on thread A2
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A2
virtual void ExampleThreadedWorker::mainLoop() object Worker-B doing work here (for 3 seconds) ... on thread A2

Function main() shutting down Worker-B.

virtual void ExampleThreadedWorker::mainLoop() object Worker-A checking for shutdown signal, on thread A1
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-A doing work here (for 3 seconds) ... on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-B checking for shutdown signal, on thread A2
virtual void ExampleThreadedWorker::mainLoop() shutdown signaled, leaving, on thread A2

Sleeping main thread for 4 more seconds.

virtual void ExampleThreadedWorker::mainLoop() object Worker-A checking for shutdown signal, on thread A1
virtual void ExampleThreadedWorker::mainLoop() shutdown not signaled, not leaving yet, on thread A1
virtual void ExampleThreadedWorker::mainLoop() object Worker-A doing work here (for 3 seconds) ... on thread A1

Function main() shutting down Worker-A.

virtual void ExampleThreadedWorker::mainLoop() object Worker-A checking for shutdown signal, on thread A1
virtual void ExampleThreadedWorker::mainLoop() shutdown signaled, leaving, on thread A1
```
