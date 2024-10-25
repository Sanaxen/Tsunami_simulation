@echo off
if not exist 宝永地震v2_chkpoint.txt (
	echo -------------------
	echo CHKPOINTで宝永地震v2_chkpoint.txtを代用するため
	echo 一度 宝永地震v2.bat を1ステップ以上計算してチェックポイントを作成する必要があります
	echo -------------------
	pause
)

del /Q chkpoint_data.bmp
del Initial_wave_data*.bmp
del Initial_wave_data*.csv
del FaultParameters*.bmp
del dx_distance.txt

copy ..\..\tsu_dataset\tu_sample\日本列島\*.* /v /y
copy ..\..\tsu_dataset\tu_sample\隕石衝突（KT_inmpact同レベル)Test日本全体\*.* /v/ y

:%BIT%\faultDefromationSolv.exe CalculationParameters.txt
goto end


:end
