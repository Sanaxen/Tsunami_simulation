%comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

msbuid.exe freeglut-2.8.0\VisualStudio\2022\freeglut.vcxproj /ttarget:freeglut /t:Rebuild /p:Configuration=Release