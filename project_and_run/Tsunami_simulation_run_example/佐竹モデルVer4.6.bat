del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

call init.bat
call clear.bat
call ���k�����m��v2-2.bat

copy "..\..\tsu_dataset\tu_sample\���|���f��Ver4.6"\*.* /v/ y

..\bin\faultDefromationSolv.exe CalculationParameters.txt

copy faultLines.bmp ..\..\tsu_dataset\tu_sample\���|���f��Ver4.6/v /y
copy FaultParameters*.bmp ..\..\tsu_dataset\tu_sample\���|���f��Ver4.6/v /y
copy Initial_wave_data*.bmp ..\..\tsu_dataset\tu_sample\���|���f��Ver4.6/v /y
copy Initial_wave_data*.csv ..\..\tsu_dataset\tu_sample\���|���f��Ver4.6/v /y
