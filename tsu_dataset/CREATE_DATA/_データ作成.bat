set ldm=..\\project_and_run\bin\bin\bitmapedit.exe

%ldm% -e1 bb_cliped.bmp
%ldm% -e2 e1.bmp aa_cliped.bmp

copy aa_cliped.bmp �V�~�������[�V�����p���̓f�[�^\water_depth_data.bmp
copy e2.bmp �V�~�������[�V�����p���̓f�[�^\topography_data.bmp

%ldm% -e3 e2.bmp bb_cliped.bmp
copy e3.bmp �V�~�������[�V�����p���̓f�[�^\topography_data_mask.bmp

%ldm% -e4 e1.bmp cc_cliped.bmp
%ldm% -e5 e4.bmp
copy e5.bmp �V�~�������[�V�����p���̓f�[�^\elevation_data.bmp

copy �V�~�������[�V�����p���̓f�[�^\water_depth_data2.csv �V�~�������[�V�����p���̓f�[�^\water_depth_data3.csv

del *.bmp /Q
del *.csv /Q
del *.raw /Q
del *.txt /Q
