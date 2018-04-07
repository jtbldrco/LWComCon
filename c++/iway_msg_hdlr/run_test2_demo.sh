#!/bin/bash

# run_test2_demo.sh


./msg_sock_hdlr_test_02 receive 16273 & > test2_recv.out 2>&1 

sleep 2

./msg_sock_hdlr_test_02 send localhost 16273 & > test2_send.out 2>&1 


sleep 2
cat test2_recv.out
sleep 2
cat test2_send.out

