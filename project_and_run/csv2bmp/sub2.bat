set ldm=C:\Users\neutral\Desktop\yamato_専用\tmp\新しいフォルダー\Release\bmp2csv.exe

%ldm% -red_blue2 out_%2.bmp
copy red_blue.bmp out_out_%2.bmp /v /y
:copy Initial_wave_data2.csv Initial_wave_data3_%1.csv
