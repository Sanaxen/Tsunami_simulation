
copy "..\..\tsu_dataset\tu_sample\日本列島"\*.* /v/ y

del topography_data_edit.bmp 

@echo 現在のtopography_data_edit.bmpは日本列島全体になっていないので削除しました。
@echo CalculationParameters.txt のパラメータを変更して下さい
@echo CalculationParameters.txt内の LAT, LONの部分を
@echo 経度緯度情報.txt を転記するだけです。

pause

