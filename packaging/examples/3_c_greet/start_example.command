#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../PeasyGUI.app/Contents/MacOS/PeasyGUI --protocol=tcp --ini="examples/3_c_greet/PeasyGUI_greet.ini" &

./greet

echo "---------------------------------------------------"
echo " "
echo " "

exit