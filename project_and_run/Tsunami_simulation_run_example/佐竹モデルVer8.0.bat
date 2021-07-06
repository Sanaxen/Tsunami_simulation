del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp

call init.bat
call clear.bat
call “Œ–k‘¾•½—m‰«v2-2.bat

copy "..\..\tsu_dataset\tu_sample\²’|ƒ‚ƒfƒ‹Ver8.0"\*.* /v/ y

:..\bin\faultDefromationSolv.exe CalculationParameters.txt

