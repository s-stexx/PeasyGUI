#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../start_PeasyGUI.sh --protocol=tcp --ini="examples/1_py_tcp_greet/PeasyGUI_greet.ini" &

python3 greet.py

echo "---------------------------------------------------"
echo " "
echo " "

exit