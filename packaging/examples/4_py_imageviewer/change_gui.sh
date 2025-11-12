#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../start_PeasyGUI.sh --ini="examples/4_py_imageviewer/PeasyGUI_viewer.ini" &

echo "---------------------------------------------------"
echo " "
echo " "

exit