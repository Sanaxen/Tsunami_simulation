del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

copy ..\..\tsu_dataset\tu_sample\中防モデル3連動\*.* /v/ y

..bin\\faultDefromationSolv.exe CalculationParameters.txt

copy faultLines.bmp ..\..\tsu_dataset\tu_sample\中防モデル3連動/v /y
copy FaultParameters*.bmp ..\..\tsu_dataset\tu_sample\中防モデル3連動/v /y
copy Initial_wave_data*.bmp ..\..\tsu_dataset\tu_sample\中防モデル3連動/v /y
copy Initial_wave_data*.csv ..\..\tsu_dataset\tu_sample\中防モデル3連動/v /y