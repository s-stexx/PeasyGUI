#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../packaging/PeasyGUI.app/Contents/MacOS/PeasyGUI --ini=../misc/PeasyGUI_all_controls.ini

echo "---------------------------------------------------"
echo " "
echo " "

exit
