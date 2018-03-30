#!/bin/bash

# File: build.sh

set -x

javac -classpath ./ com/iwaytechnology/osp/util/IThreadTerminationMonitor.java
javac -classpath ./ com/iwaytechnology/osp/util/ITerminationMsgManager.java
javac -classpath ./ com/iwaytechnology/osp/basic_ex/BasicOspServer.java
