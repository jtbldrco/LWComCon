#!/bin/bash

# File: run.sh

set -x

SAMPLE_WORK=sample_work.txt
TERM_COMM_PORT=1789

java -cp ./ com/iwaytechnology/osp/basic_ex/BasicOspServer $TERM_COMM_PORT $SAMPLE_WORK

sleep 1
echo ""
echo "Now dumping file of recorded work efforts -"
echo ""
sleep 2
cat $SAMPLE_WORK
