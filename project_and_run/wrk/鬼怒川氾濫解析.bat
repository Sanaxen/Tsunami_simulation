del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\‹S“{ì”Ã—”‰ğÍ\*.* /v/ y

:..\bin\faultDefromationSolv.exe CalculationParameters.txt
goto end

copy faultLines.bmp ..\..\tsu_dataset\tu_sample\‹S“{ì”Ã—”‰ğÍ/v /y
copy FaultParameters*.bmp ..\..\tsu_dataset\tu_sample\‹S“{ì”Ã—”‰ğÍ/v /y
copy Initial_wave_data*.bmp ..\..\tsu_dataset\tu_sample\‹S“{ì”Ã—”‰ğÍ/v /y
copy Initial_wave_data*.csv ..\..\tsu_dataset\tu_sample\‹S“{ì”Ã—”‰ğÍ/v /y
copy dx_distance.txt ..\..\tsu_dataset\tu_sample\‹S“{ì”Ã—”‰ğÍ/v /y

:end
