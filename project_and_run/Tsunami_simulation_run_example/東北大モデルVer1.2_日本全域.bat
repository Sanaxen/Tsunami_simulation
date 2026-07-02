del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

copy ..\..\tsu_dataset\tu_sample\日本列島\*.* /v/ y
copy ..\..\tsu_dataset\tu_sample\東北大モデルVer1.2_日本全域\*.* /v/ y

Release\faultDefromationSolv.exe CalculationParameters.txt
