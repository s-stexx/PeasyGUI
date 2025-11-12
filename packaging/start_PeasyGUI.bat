@echo off
echo ==============================================
echo Select an action by entering a number (0..9):
echo ----------------------------------------------
echo 0 Start PeasyGUI in Designer mode
echo.
echo 1 Start example "Python: TCP greet"
echo 2 Start example "Python: Filebased greet"
echo 3 Start example "C: greet"
echo 4 Start example "Python: ImageViewer"
echo 5 Start example "Python: FilePrefixer"
echo.
echo 6 Change GUI for "Python: TCP greet"
echo 7 Change GUI for "Python: Filebased greet"
echo 8 Change GUI for "C: greet"
echo 9 Change GUI for "Python: ImageViewer"
echo ==============================================
set /p choice=Your choice: 

if "%choice%"=="0" (
    start bin\PeasyGUI
)

if "%choice%"=="1" (
    start bin\PeasyGUI --protocol=tcp --ini="examples/1_py_tcp_greet/PeasyGUI_greet.ini"
    cd examples/1_py_tcp_greet
    python greet.py
)

if "%choice%"=="2" (
    cd examples/2_py_filebased_greet
    python greet.py
)

if "%choice%"=="3" (
    start bin\PeasyGUI --protocol=tcp --ini="examples/3_c_greet/PeasyGUI_greet.ini"
    cd examples/3_c_greet
    greet
)

if "%choice%"=="4" (
    start bin\PeasyGUI --protocol=tcp --ini="examples/4_py_imageviewer/PeasyGUI_viewer.ini"
    cd examples/4_py_imageviewer
    python imageviewer.py
)

if "%choice%"=="5" (
    start bin\PeasyGUI --protocol=tcp --ini="THIS_INI_SHALL_NOT_EXIST"
    cd examples/5_py_fileprefixer
    python fileprefixer.py
)

if "%choice%"=="6" (
    start bin\PeasyGUI --ini="examples/1_py_tcp_greet/PeasyGUI_greet.ini"
)

if "%choice%"=="7" (
    start bin\PeasyGUI --ini="examples/2_py_filebased_greet/PeasyGUI_greet.ini"
)

if "%choice%"=="8" (
    start bin\PeasyGUI --ini="examples/3_c_greet/PeasyGUI_greet.ini"
)

if "%choice%"=="9" (
    start bin\PeasyGUI --ini="examples/4_py_imageviewer/PeasyGUI_viewer.ini"
)

pause
