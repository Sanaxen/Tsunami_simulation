copy freeglut-2.8.0\include\GL\*.* TSUNAMI_Viewr3D\include\GL /v /y
copy freeglut-2.8.0\lib\x86\*.lib TSUNAMI_Viewr3D\libs\32bit /v /y
copy freeglut-2.8.0\lib\x86\*.dll TSUNAMI_Viewr3D\bin\32bit /v /y

copy freeglut-2.8.0\lib\x64\*.lib TSUNAMI_Viewr3D\libs\64bit /v /y
copy freeglut-2.8.0\lib\x64\*.dll TSUNAMI_Viewr3D\bin\64bit /v /y

set ver=1.13.0
powershell Expand-Archive -Path glew-%ver%-win32.zip -DestinationPath glew-%ver%\.. -Force

copy glew-%ver%\include\GL\*.* TSUNAMI_Viewr3D\include\GL /v /y
copy glew-%ver%\bin\Release\Win32\*.* TSUNAMI_Viewr3D\bin\32bit /v /y
copy glew-%ver%\lib\Release\Win32\*.* TSUNAMI_Viewr3D\libs\32bit /v /y
copy glew-%ver%\bin\Release\x64\*.* TSUNAMI_Viewr3D\bin\64bit /v /y
copy glew-%ver%\lib\Release\x64\*.* TSUNAMI_Viewr3D\libs\64bit /v /y

copy glui-2.36\src\include\GL\*.* TSUNAMI_Viewr3D\include\GL /v /y
copy glui-2.36\src\msvc\bin\32bit\*.* TSUNAMI_Viewr3D\bin\32bit /v /y
copy glui-2.36\src\msvc\lib\32bit\*.* TSUNAMI_Viewr3D\libs\32bit /v /y
copy glui-2.36\src\msvc\bin\64bit\*.* TSUNAMI_Viewr3D\bin\64bit /v /y
copy glui-2.36\src\msvc\lib\64bit\*.* TSUNAMI_Viewr3D\libs\64bit /v /y

:msbuild.exe TSUNAMI_Viewr3D\glsl1.vcxproj  /t:Rebuild /p:Configuration="Release" /p:Platform=Win32
msbuild.exe TSUNAMI_Viewr3D\glsl1.vcxproj  /t:Rebuild /p:Configuration="Release" /p:Platform=x64