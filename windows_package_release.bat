SET QTROOT=c:\qt\Qt5.0.2\5.0.2\msvc2010

copy %QTROOT%\bin\D3DCompiler_43.dll   build\Release\.
copy %QTROOT%\bin\icudt49.dll          build\Release\.
copy %QTROOT%\bin\icuin49.dll          build\Release\.
copy %QTROOT%\bin\icuuc49.dll          build\Release\.
copy %QTROOT%\bin\libEGL.dll           build\Release\.
copy %QTROOT%\bin\libGLESv2.dll        build\Release\.
copy %QTROOT%\bin\Qt5Core.dll          build\Release\.
copy %QTROOT%\bin\Qt5Gui.dll           build\Release\.
copy %QTROOT%\bin\Qt5Widgets.dll       build\Release\.

copy dlls\*.dll                        build\Release\.

