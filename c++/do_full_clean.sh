#!/bin/bash


echo "This script will delete all files of type: *Test, *.o, *.a, *.so !!!"

read -p "Enter Y to continue: " -n 1 response

echo ""
echo ""

if [[ $response =~ ^[Yy]$ ]]
then
    find . -type f -name "*Test" | xargs rm --verbose
    find . -type f -name "*.o" | xargs rm --verbose
    find . -type f -name "*.a" | xargs rm --verbose
    find . -type f -name "*.so" | xargs rm --verbose
fi
