@echo off
if not exist ��i�n�kv2_chkpoint.txt (
	echo -------------------
	echo CHKPOINT�ŕ�i�n�kv2_chkpoint.txt���p���邽��
	echo ��x ��i�n�kv2.bat ��1�X�e�b�v�ȏ�v�Z���ă`�F�b�N�|�C���g���쐬����K�v������܂�
	echo -------------------
	pause
)

del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\���{��\*.* /v /y
copy ..\..\tsu_dataset\tu_sample\覐ΏՓˁiKT_inmpact�����x��)Test���{�S��\*.* /v/ y

:%BIT%\faultDefromationSolv.exe CalculationParameters.txt
goto end


:end
