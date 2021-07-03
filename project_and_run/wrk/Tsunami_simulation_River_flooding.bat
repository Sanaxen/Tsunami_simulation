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
del /Q obj\vtk\*.*
del /Q output\*.*
del /Q chk\*.*
del /Q google_earth\*.*

copy ..\..\tsu_dataset\KONSEKI_DATA\*.csv /v /y

set ELV_SCALE=6
set DEPTH_SCALE=6
set WAVE_SCALE=1
set WAVE_SCALE_COEF=1

:set MIN_DEPTH=5
:set MAX_DEPTH=5

set KML_EXPORT=1
set DAMAGE_ESTIMATION=1

set DEPTH_DOWN=-1

:set CRUSTAL=1

:set EXCLUSION_RANGE_MIN=0.5
:set EXCLUSION_RANGE_MAX=2.5

:set WATER_DEPTH_SMOOTHING=2
:set MAX_MARKER=1
:set WINDVECTOR_PLOT=1

:set VECTOR_PLOT=8
set XY_DISPLACEMENT=1

set VTK_EXPORT=0
:set DEBUG_RIVER_INFO=1
set OMP_NUM_THREADS=7
set RUNUP=1

:set ACC=_gpu_float
set ACC=
set AMP_GPU_DEVICE_NO=0
..\bin\faultDefromationSolv%ACC%.exe CalculationParameters.txt

..\bin\konseki.exe CalculationParameters.txt

..\bin\tsu.exe CalculationParameters.txt

call ..\bin\cnv_move2D.bat
call ..\bin\cnv_move3D.bat
