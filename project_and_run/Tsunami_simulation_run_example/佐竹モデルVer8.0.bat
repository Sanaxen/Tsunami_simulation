del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

call init.bat
call clear.bat
call kΎ½m«v2-2.bat

copy "..\..\tsu_dataset\tu_sample\²|fVer8.0"\*.* /v/ y

:..\bin\faultDefromationSolv.exe CalculationParameters.txt

