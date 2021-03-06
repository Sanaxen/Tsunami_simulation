del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

call init.bat
call clear.bat
call 東北太平洋沖v2-2.bat
copy "..\..\tsu_dataset\tu_sample\東北大モデルVer1.2"\*.* /v/ y

..\bin\faultDefromationSolv.exe CalculationParameters.txt
goto end

copy faultLines.bmp ..\..\\tu_sample\東北大モデルVer1.2/v /y
copy FaultParameters*.bmp ..\..\\tu_sample\東北大モデルVer1.2/v /y
copy Initial_wave_data*.bmp ..\..\\tu_sample\東北大モデルVer1.2/v /y
copy Initial_wave_data*.csv ..\..\\tu_sample\東北大モデルVer1.2/v /y

:end