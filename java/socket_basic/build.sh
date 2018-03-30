#!/bin/bash

# File: build.sh

set -x

javac -classpath ./ Client.java
javac -classpath ./ Server.java
