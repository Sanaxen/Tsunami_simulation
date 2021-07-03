del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

call init.bat
call clear.bat
call •ó‰i’nkv2.bat
copy ..\..\tsu_dataset\tu_sample\’†‰›–hĞ‰ï‹cCASE1_new\*.* /v/ y
copy ..\..\tsu_dataset\tu_sample\’†‰›–hĞ‰ï‹cCASE1_new\CalculationParameters.txt /v/ y

:Release\faultDefromationSolv.exe CalculationParameters.txt

:end
