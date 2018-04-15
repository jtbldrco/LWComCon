#!/bin/bash


echo "This script will execute 'make clean' everywhere ..."
read -p "Enter Y to continue: " -n 1 response

echo ""

if env | grep -q ^GITHUB_LOCAL_LWCC_TOP=
then
    echo "env var GITHUB_LOCAL_LWCC_TOP is defined ..."
else
    echo "Env Var GITHUB_LOCAL_LWCC_TOP is NOT defined - will exit"
    echo "Use something like:"
    echo "    export GITHUB_LOCAL_LWCC_TOP=~/github-local-lwcc/LWComCon"
    echo " - exiting"
    exit 1
fi

### if [[ $response =~ ^[Yy]$ ]]
### then
###     find . -type f -name "*Test" | xargs rm --verbose
###     find . -type f -name "*.o" | xargs rm --verbose
###     find . -type f -name "*.a" | xargs rm --verbose
###     find . -type f -name "*.so" | xargs rm --verbose
### fi

CUR_DIR=`pwd`
for DIR in `find $GITHUB_LOCAL_LWCC_TOP/c++ -type d -name "*"` ; do
    cd $DIR
    echo "------------------------------------------------------"
    echo "Now in subdir $DIR ..."
    echo ""
    if [ -e ./Makefile ] ; then
        make -f Makefile clean;
    else
        echo "No Makefile here ... continuing"
    fi
done

cd $CUR_DIR
echo ""

