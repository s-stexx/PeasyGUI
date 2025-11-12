#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../PeasyGUI.app/Contents/MacOS/PeasyGUI --ini="examples/2_py_filebased_greet/PeasyGUI_greet.ini" &

echo "---------------------------------------------------"
echo " "
echo " "

exit