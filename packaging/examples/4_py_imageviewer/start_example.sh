#!/bin/sh

echo " "
echo " "
echo "---------------------------------------------------"
cd `dirname $0`

../../start_PeasyGUI.sh --protocol=tcp --ini="examples/4_py_imageviewer/PeasyGUI_viewer.ini" &

python3 imageviewer.py

echo "---------------------------------------------------"
echo " "
echo " "

exit