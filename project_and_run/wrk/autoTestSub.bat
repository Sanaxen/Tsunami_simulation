

del debug_*.csv
del Initial_wave_dataDown.csv
del Initial_wave_dataUp.csv
del latitude_data.csv
del manninig_data.csv
del topography_data.csv
del water_depth_data.csv


:del /Q runup_image\*.*
del /Q image\*.*
del /Q image3D\*.*
del /Q obj\*.*
del /Q output\*.*


set OMP_NUM_THREADS=32
set RUNUP=0
if "%2"=="遡上考慮あり" set RUNUP=1

echo %2 >> 処理結果ステータス.log

if "%3"=="1" goto AA
..\bin\faultDefromationSolv.exe CalculationParameters.txt

..\bin\konseki.exe CalculationParameters.txt

:AA
..\bin\tsu.exe CalculationParameters.txt


copy %1.log %1_%2.log /v /y
..\bin\tail -n 30 %1.log > tmp.log
type tmp.log >> 処理結果ステータス.log
goto 2
del /Q tmp.log

:2
..\bin\reindex.exe image

ffmpeg  -r 6 -y -i image\output_W%%06d.bmp -qscale 7 %1_%2_simulation.swf
ffmpeg  -r 6 -y -i image\output_W%%06d.bmp -codec msmpeg4v2 -qscale 7 %1_%2_simulation.avi
:"C:\Program Files\ImageMagick-6.8.2-Q16\convert.exe"  -geometry 80%% image\output_W*.bmp simulation.gif
:"C:\Program Files\ImageMagick-6.8.2-Q16\convert.exe"  -geometry 80%% image\output_W*.bmp simulation.avi

type tsunami_template0.html > %1_%2_simulation.html
echo %1(%2) - simulation  >> %1_%2_simulation.html
type tsunami_template1.html >> %1_%2_simulation.html
echo %1_%2_simulation.swf >> %1_%2_simulation.html
type tsunami_template2.html >> %1_%2_simulation.html

