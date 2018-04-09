#!/bin/bash


echo "Warning: this is a work in progress yet!"

sleep 10

./MsgCommHdlrTestReceiver &

./MsgCommHdlrTestSender &

sleep 2
echo "DONE"

