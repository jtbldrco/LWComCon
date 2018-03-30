#!/bin/bash

# File: stop.sh

set -x

TERMINATION_LISTENER_HOST=localhost
TERMINATION_REQUEST_PORT=1789

java -cp ./ com/iwaytechnology/osp/full_ex/FullOspServer $TERMINATION_LISTENER_HOST $TERMINATION_REQUEST_PORT stop
