del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

copy ..\..\tsu_dataset\tu_sample\’†–hƒ‚ƒfƒ‹3˜A“®\*.* /v/ y

..bin\\faultDefromationSolv.exe CalculationParameters.txt

copy faultLines.bmp ..\..\tsu_dataset\tu_sample\’†–hƒ‚ƒfƒ‹3˜A“®/v /y
copy FaultParameters*.bmp ..\..\tsu_dataset\tu_sample\’†–hƒ‚ƒfƒ‹3˜A“®/v /y
copy Initial_wave_data*.bmp ..\..\tsu_dataset\tu_sample\’†–hƒ‚ƒfƒ‹3˜A“®/v /y
copy Initial_wave_data*.csv ..\..\tsu_dataset\tu_sample\’†–hƒ‚ƒfƒ‹3˜A“®/v /y