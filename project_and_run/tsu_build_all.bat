if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
)
:pause
powershell Expand-Archive -Path bin\util_set.zip -DestinationPath .\bin -Force

msbuild.exe .\tsu.sln  /t:Rebuild /p:Configuration=Release /p:Platform=x64

copy x64\Release\*.exe .\bin /v /y


cd tu_viewer

call all_build.bat

copy freeglut-2.8.0\lib\freeglut.dll  ..\bin /v /y
copy glew-1.13.0\bin\Release\x64\*.dll ..\bin /v /y
copy glui-2.36\src\msvc\bin\64bit\*.dll ..\bin /v /y
copy TSUNAMI_Viewr3D\bin\64bit\*.exe ..\bin /v /y
copy TSUNAMI_Viewr3D\bin\64bit\*.dll ..\bin /v /y
copy WindowsFormsApplication1\bin\x64\Release\*.exe ..\bin /v /y

cd ..


