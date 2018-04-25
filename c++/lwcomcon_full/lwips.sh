#!/bin/bash

# The LWCC process need not be running for testing of Prod/Con
export LWCC_HOST=localhost
export LWCC_PORT=16273

# These IP Addresses are values of two sibling VMs -

export ProdIP=192.168.127.141
export ProdPort=16274
export ConIP=192.168.127.142
export ConPort=16275
