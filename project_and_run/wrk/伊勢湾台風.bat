del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\À­Cnkv2\*.* /v/ y

..\bin\faultDefromationSolv.exe CalculationParameters.txt

copy ..\..\tsu_dataset\tu_sample\É¨pä\*.* /v/ y

