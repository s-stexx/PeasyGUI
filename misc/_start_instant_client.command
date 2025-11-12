#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

alias python3="/opt/homebrew/bin/python3.12"

python3 instant_client.py

echo "---------------------------------------------------"
echo " "
echo " "

exit
