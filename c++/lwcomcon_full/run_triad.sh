#!/bin/bash

# File: run_triad.sh

# Use this script to run any ONE of the three processes required
# to create a triad of - 
#
#    - LWComCon process
#    - Divisible Producer (test producer process)
#    - Divisible Consumer (test consumer process)
#
function show_usage() {

    echo "Run any of lwcc/producer/consumer, all on localhost -"
    echo "    usage: ./run_triad.sh <triad_piece>"
    echo "example 1: ./run_triad.sh lwcc"
    echo "example 2: ./run_triad.sh prod"
    echo "example 3: ./run_triad.sh con"

}

if [ $# -ne 1 ] ; then
    show_usage
    exit 1
fi

LW_HOST=localhost
LW_PORT=16273
PROD_HOST=localhost
PROD_PORT=16274
CON_HOST=localhost
CON_PORT=16275

if [ "$1" == "lwcc" ] ; then

    #  usage: ./LWComCon <listener_host_ifc> <listener_host_port>
    #                    <producer_host_ifc> <producer_host_port>
    #                    <consumer_host_ifc> <consumer_host_port>

    # 'Push to backgroud' allows shell access
    ./LWComConFull $LW_HOST $LW_PORT $PROD_HOST $PROD_PORT $CON_HOST $CON_PORT  &
    exit 0
fi

if [ "$1" == "prod" ] ; then

    #  usage: ./DivisibleProducer <listener_host_ifc> <listener_host_port>
    #                             <consumer_host_ifc> <consumer_host_port>

    # 'Push to backgroud' allows shell access
    ./DivisibleProducer $PROD_HOST $PROD_PORT $CON_HOST $CON_PORT  &
    exit 0
fi

if [ "$1" == "con" ] ; then

    #  usage: ./DivisibleConsumer <listener_host_ifc> <listener_host_port>
    #                             <com-con_host_ifc> <com-con_host_port>

    # 'Push to backgroud' allows shell access
    ./DivisibleConsumer $CON_HOST $CON_PORT $LW_HOST $LW_PORT  &
    exit 0
fi

show_usage
exit 1

