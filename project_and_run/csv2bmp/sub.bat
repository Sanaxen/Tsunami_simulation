set ldm=C:\Users\neutral\Desktop\yamato_��p\tmp\�V�����t�H���_�[\Release\bmp2csv.exe

%ldm% -red_blue %2.bmp
copy red_blue.bmp out_%2.bmp /v /y
:copy Initial_wave_data2.csv Initial_wave_data3_%1.csv
