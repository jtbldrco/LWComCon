#!/bin/bash

# run_test2_demo.sh

echo ""
echo "Test will run with output captured to file, then output will be dumped to screen."

# Start receiver in background
sleep 1
./msg_sock_hdlr_test_02 receive 16273 > test2_recv.out 2>&1 &

# Start sender in foreground
sleep 1
./msg_sock_hdlr_test_02 send localhost 16273 > test2_send.out 2>&1


sleep 1
echo ""
echo "====================="
echo "SENDER Console Output"
echo "====================="
cat test2_send.out
sleep 1
echo ""
echo "======================="
echo "RECEIVER Console Output"
echo "======================="
cat test2_recv.out
echo ""

rm test2_send.out
rm test2_recv.out
