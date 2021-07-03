set CPDIR=tu_sample

copy water_depth_data.bmp %CPDIR% /v /y
copy topography_data_mask.bmp %CPDIR% /v /y
copy topography_data.bmp %CPDIR% /v /y
copy Initial_wave_data.bmp %CPDIR% /v /y
copy samplingLine_data.bmp %CPDIR% /v /y
copy elevation_data.bmp %CPDIR% /v /y

copy water_depth_data.csv %CPDIR% /v /y
copy topography_data.csv %CPDIR% /v /y
copy Initial_wave_data.csv %CPDIR% /v /y

copy CalculationParameters.txt %CPDIR% /v /y
copy render_template.pov %CPDIR% /v /y
copy render.gif %CPDIR% /v /y
copy simulation.gif  %CPDIR% /v /y

copy elevation_data2.csv %CPDIR% /v /y
copy water_depth_data2.csv %CPDIR% /v /y