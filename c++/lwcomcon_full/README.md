# C++ Lightweight Command/Control

<img align="right" src="../../images/iwaytechnology284x60.gif" /> 

## Status: Work In Progress

The software here is MUCH farther along than the README.

The programs here illustrate an entire implementation of interprocess communication using the LWComCon Framework.  While this is a contrived example, its purpose is to illustrate (and validate) the communcation with a sufficiently simple application to prevent hiding the Framework use and behavior in the process.

The shell scripts in this directory contain comments to direct the setup and running of the related executables.

For example, the Producer-Consumer test has completed over 500,000 round-trip networking exchanges with no memory issues emerging.

This subdirectory contains these important elements:

- A C++ Communications Manager implementing the ThreadedWorker Design Pattern (MsgCommHdlr).


