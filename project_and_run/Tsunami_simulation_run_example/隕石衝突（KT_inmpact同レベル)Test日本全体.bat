del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\日本列島\*.* /v /y
copy ..\..\tsu_dataset\tu_sample\隕石衝突（KT_inmpact同レベル)Test日本全体\*.* /v/ y

:%BIT%\faultDefromationSolv.exe CalculationParameters.txt
goto end


:end
