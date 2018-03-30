#!/bin/bash

# File: stop.sh

set -x

TERM_COMM_PORT=1789

java -cp ./ com/iwaytechnology/osp/basic_ex/BasicOspServer $TERM_COMM_PORT stop
