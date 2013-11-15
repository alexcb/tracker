SET PATH=c:\Program Files (x86)\CMake 2.8\bin\;%PATH%
SET QTROOT=c:\qt\Qt5.0.2\5.0.2\msvc2010

rd /s /q build
mkdir build
cd build
cmake -D CMAKE_PREFIX_PATH=%QTROOT% ..

