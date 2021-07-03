%comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"

msbuid.exe freeglut-2.8.0\VisualStudio\2019\freeglut.vcxproj /ttarget:freeglut /t:Rebuild /p:Configuration=Release