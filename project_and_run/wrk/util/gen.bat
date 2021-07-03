set BMP2CSV_CONV_COEF=%5

if "%1"=="all" goto 2
copy topography_data%1.bmp out\Initial_wave_data3_%2.bmp /v /y
:bmp2csv -i3 topography_data%1.bmp %3 %4
:bmp2csv -i2 topography_data%1.bmp %3 %4
bmp2csv -i4 topography_data%1.bmp %3 %4
copy Initial_wave_data2.csv out\Initial_wave_data3_%2.csv /v /y
goto end

:2
copy topography_data_all.bmp out\Initial_wave_data2.bmp /v /y
:bmp2csv -i3 topography_data_all.bmp -10 10
bmp2csv -i2 topography_data_all.bmp -10 10
copy Initial_wave_data2.csv out\Initial_wave_data2.csv /v /y
goto end

:end

