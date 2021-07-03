del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

copy "..\..\tsu_dataset\tu_sample\“Œ–k"\*.* /v/ y

Release\faultDefromationSolv.exe CalculationParameters.txt

:end