set ldm=..\..\bin\bitmapedit.exe

del tmp\*.* /Q
del �V�~�������[�V�����p���̓f�[�^\*.* /Q

copy ..\�n�`�f�[�^\E140N40-90\*.* tmp /v /y

set SX=1
set EX=860
set SY=5309
set EY=6610


%ldm% -clip %SX% %EX% %SY% %EY% tmp\aa.bmp
copy cliped.bmp aa_cliped.bmp

%ldm% -clip %SX% %EX% %SY% %EY% tmp\bb.bmp
copy cliped.bmp bb_cliped.bmp

%ldm% -clip %SX% %EX% %SY% %EY% tmp\cc.bmp
copy cliped.bmp cc_cliped.bmp

%ldm% -clip_tr %SX% %EX% %SY% %EY% tmp\E140N40-90.raw
copy cliped.csv �V�~�������[�V�����p���̓f�[�^\elevation_data2.csv

:%ldm% -clip_tr %SX% %EX% %SY% %EY% tmp\E140N40-90_.raw
:copy cliped.csv �V�~�������[�V�����p���̓f�[�^\water_depth_data2.csv


%ldm% -e7_t tmp\E140N40-90_point.raw lon.raw lat.raw hi.raw
 
%ldm% -clip_tr %SX% %EX% %SY% %EY% hi.raw
copy cliped.csv �V�~�������[�V�����p���̓f�[�^\water_depth_data2.csv

%ldm% -clip_tr %SX% %EX% %SY% %EY% lon.raw "LON"
copy parameter.txt lon.txt /v /y

%ldm% -clip_tr %SX% %EX% %SY% %EY% lat.raw "LAT"
copy parameter.txt lat.txt /v /y

type lon.txt > �V�~�������[�V�����p���̓f�[�^\�o�x�ܓx���.txt
type lat.txt >> �V�~�������[�V�����p���̓f�[�^\�o�x�ܓx���.txt

call _�f�[�^�쐬.bat
