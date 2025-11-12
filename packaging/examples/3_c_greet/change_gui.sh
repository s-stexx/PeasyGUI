#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../start_PeasyGUI.sh --ini="examples/3_c_greet/PeasyGUI_greet.ini" &

echo "---------------------------------------------------"
echo " "
echo " "

exit