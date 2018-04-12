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

# LWComCon listener
LW_HOST=localhost
LW_PORT=16273

# Producer listener (from LWComCon)
# - LWComCon opens sender to this port, 
# - Producer opens receiver on this port.
PROD_L_HOST=localhost
PROD_L_PORT=16274

# Consumer listener (from LWComCon)
# - LWComCon opens sender to this port, 
# - Consumer opens receiver on this port.
CON_L_HOST=localhost
CON_L_PORT=16275

# Consumer listener (from Producer)
# - LWComCon opens sender to this port, 
# - Consumer opens receiver on this port.
CON_P_HOST=localhost
CON_P_PORT=16276

if [ "$1" == "lwcc" ] ; then

    # LWComCon has a receiver and two sender msg hdlrs - 
    # usage: ./LWComCon <listener_host_ifc> <listener_host_port>
    #                   <producer_host_ifc> <producer_host_port>
    #                   <consumer_host_ifc> <consumer_host_port>

    # DO NOT 'Push to background' on this one - won't be able
    # to enter quit choices (1-4)
    ./LWComConFull $LW_HOST $LW_PORT $PROD_L_HOST $PROD_L_PORT $CON_L_HOST $CON_L_PORT
    exit 0
fi

if [ "$1" == "prod" ] ; then

    # DivProd has a receiver (from lwcc), and a sender (to con) msg hdlr - 
    # usage: ./DivisibleProducer <lwcc_host_list_ifc> <lwcc_host_list_port>
    #                            <consumer_dest_host_ifc> <consumer_dest_host_port>

    # 'Push to backgroud' allows shell access
    ./DivisibleProducer $PROD_L_HOST $PROD_L_PORT $CON_P_HOST $CON_P_PORT  &
    exit 0
fi

if [ "$1" == "con" ] ; then

    # DivCon has a receiver (from lwcc), and a receiver (from prod) msg hdlr - 
    # usage: ./DivisibleConsumer <lwcc_host_list_ifc> <lwcc_host_list_port>
    #                            <prod_host_list_ifc> <prod_host_list_port>

    # 'Push to backgroud' allows shell access
    ./DivisibleConsumer $CON_L_HOST $CON_L_PORT $CON_P_HOST $CON_P_PORT  &
    exit 0
fi

show_usage
exit 1

