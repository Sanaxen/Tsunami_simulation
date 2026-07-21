del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\安政東海地震v2\*.* /v/ y

..\bin\faultDefromationSolv.exe CalculationParameters.txt

copy ..\..\tsu_dataset\tu_sample\伊勢湾台風\*.* /v/ y

call 地形を日本列島に変更.bat

copy ..\..\tsu_dataset\tu_sample\伊勢湾台風_日本f列島全域\*.* /v/ y
