del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\��i�n�kv2\*.* /v /y
copy ..\..\tsu_dataset\tu_sample\覐ΏՓˁiKT_inmpact�����x��)Test\*.* /v/ y

:%BIT%\faultDefromationSolv.exe CalculationParameters.txt
goto end


:end
