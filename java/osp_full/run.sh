#!/bin/bash

# File: run.sh

set -x

SAMPLE_WORK_FILE=sample_work.txt
TERMINATION_REQUEST_PORT=1789

java -cp ./ com/iwaytechnology/osp/full_ex/FullOspServer $TERMINATION_REQUEST_PORT $SAMPLE_WORK_FILE

sleep 1
echo ""
echo "Dumping 'work record file' next ..."
echo ""
sleep 2

cat $SAMPLE_WORK_FILE
