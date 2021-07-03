set ldm=..\..\64bit\bitmapedit.exe

del tmp\*.* /Q
del �V�~�������[�V�����p���̓f�[�^\*.* /Q

copy ..\�n�`�f�[�^\all\*.* tmp /v /y

set SX=3260
set EX=5600
set SY=5300
set EY=7700

%ldm% -clip %SX% %EX% %SY% %EY% tmp\aa.bmp
copy cliped.bmp aa_cliped.bmp

%ldm% -clip %SX% %EX% %SY% %EY% tmp\bb.bmp
copy cliped.bmp bb_cliped.bmp

%ldm% -clip %SX% %EX% %SY% %EY% tmp\cc.bmp
copy cliped.bmp cc_cliped.bmp

%ldm% -clip_tr %SX% %EX% %SY% %EY% tmp\all.raw
copy cliped.csv �V�~�������[�V�����p���̓f�[�^\elevation_data2.csv

:%ldm% -clip_tr %SX% %EX% %SY% %EY% tmp\all_.raw
:copy cliped.csv �V�~�������[�V�����p���̓f�[�^\water_depth_data2.csv


%ldm% -e7_t tmp\all_point.raw lon.raw lat.raw hi.raw
 
%ldm% -clip_tr %SX% %EX% %SY% %EY% hi.raw
copy cliped.csv �V�~�������[�V�����p���̓f�[�^\water_depth_data2.csv

%ldm% -clip_tr %SX% %EX% %SY% %EY% lon.raw "LON"
copy parameter.txt lon.txt /v /y
%ldm% -clip_tr %SX% %EX% %SY% %EY% lat.raw "LAT"
copy parameter.txt lat.txt /v /y

type lon.txt > �V�~�������[�V�����p���̓f�[�^\�o�x�ܓx���.txt
type lat.txt >> �V�~�������[�V�����p���̓f�[�^\�o�x�ܓx���.txt

call _�f�[�^�쐬.bat