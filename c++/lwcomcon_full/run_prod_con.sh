#!/bin/bash

source ./lwips.sh

if [ $# -ne 1 ] ; then
    echo "usage: ./run_prod_con.sh [ prod | con ]"
    exit 1 
fi

if [ "$1" == "prod" ] ; then
    ./DivisibleProducer $LWCC_HOST $LWCC_PORT $ConIP $ConPort $ProdIP $ProdPort
else
    ./DivisibleConsumer $LWCC_HOST $LWCC_PORT $ConIP $ConPort $ProdIP $ProdPort
fi
 
