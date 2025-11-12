#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../start_PeasyGUI.sh --protocol=tcp --ini="THIS_INI_SHALL_NOT_EXIST" &

python3 fileprefixer.py

echo "---------------------------------------------------"
echo " "
echo " "

exit