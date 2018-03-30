#!/bin/bash

# File: build.sh

set -x

javac -classpath ./ com/iwaytechnology/osp/util/ITerminationMsgManager.java
javac -classpath ./ com/iwaytechnology/osp/util/IThreadTerminationMonitor.java
javac -classpath ./ com/iwaytechnology/osp/full_ex/WorkerThread.java
javac -classpath ./ com/iwaytechnology/osp/full_ex/FullOspServer.java
