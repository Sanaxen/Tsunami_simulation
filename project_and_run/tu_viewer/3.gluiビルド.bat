copy freeglut-2.8.0\include\GL\*.* glui-2.36\src\include\GL /v /y
#$copy freeglut-2.8.0\lib\x86\*.lib glui-2.36\src\msvc\lib\32bit /v /y
copy freeglut-2.8.0\lib\x64\*.lib glui-2.36\src\msvc\lib\64bit /v /y

#msbuild.exe glui-2.36\src\msvc\gluidll.vcxproj  /t:Rebuild /p:Configuration="Release GLUIDLL" /p:Platform=Win32
msbuild.exe glui-2.36\src\msvc\gluidll.vcxproj  /t:Rebuild /p:Configuration="Release GLUIDLL" /p:Platform=x64